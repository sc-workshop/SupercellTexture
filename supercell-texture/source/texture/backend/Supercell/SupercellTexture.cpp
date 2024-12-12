#include "SupercellTexture.h"

#include "ScPixel.hpp"
#include "texture/SCTX/TextureData_generated.h"
#include "texture/SCTX/MipMapData_generated.h"

#include "compression/compression.h"

using namespace wk;

namespace sc::texture
{
	SupercellTexture::SupercellTexture(uint16_t width, uint16_t height, ScPixel::Type type, Ref<wk::MemoryStream> buffer) :
		m_pixel_type(type), m_data(buffer)
	{
		m_width = width;
		m_height = height;
		std::vector<uint8_t> hash;
		m_levels.emplace_back(width, height, 0, hash);
	}

	SupercellTexture::SupercellTexture(std::filesystem::path path)
	{
		m_stream = wk::CreateRef<StreamT>(path);
		read_streaming_data();
	}

	void SupercellTexture::read_streaming_data()
	{
		uint32_t texture_data_length = m_stream->read_unsigned_int();
		MemoryStream texture_data(texture_data_length);
		m_stream->read(texture_data.data(), texture_data_length);

		{
			flatbuffers::Verifier verifier((const uint8_t*)texture_data.data(), texture_data.length());
			bool is_valid_sctx = SCTX::VerifyTextureDataBuffer(verifier);
			if (!is_valid_sctx)
			{
				throw wk::Exception("Failed to verify SCTX integrity");
			}
		}
		
		uint32_t levels_count;

		{
			const SCTX::TextureData* texture = SCTX::GetTextureData(texture_data.data());
			m_pixel_type = (ScPixel::Type)texture->pixel_type();
			m_width = texture->width();
			m_height = texture->height();
			levels_count = texture->levels_count();
			unknown_integer = texture->unk4(); // usually 12/11/10
			m_texture_data_length = texture->texture_length();

			auto* variants = texture->variants();
			if (variants)
			{
				auto ids = variants->streaming_id();
				auto variants_data = variants->streaming_textures();
				if (ids)
				{
					streaming_ids = IdArray(ids->begin(), ids->end());
				}

				if (variants_data)
				{
					streaming_variants = VariantsArray();
					streaming_variants->reserve(variants_data->size());

					for (std::size_t i = 0; variants_data->size() > i; i++)
					{
						auto texture_variant = variants_data->Get(i);
						auto texture_variant_data = texture_variant->data();
						Ref<MemoryStream> variant_stream = CreateRef<MemoryStream>(texture_variant_data->size());
						Memory::copy(
							(uint8_t*)texture_variant_data->data(),
							(uint8_t*)variant_stream->data(),
							texture_variant_data->size()
						);

						streaming_variants->emplace_back(
							texture_variant->width(), texture_variant->height(),
							(ScPixel::Type)texture_variant->pixel_type(), variant_stream
						);
					}
				}
			}
		}
		
		uint32_t mip_maps_data_length = m_stream->read_unsigned_int();
		size_t texture_data_offset = m_stream->position() + mip_maps_data_length;

		{
			m_levels.reserve(levels_count);
			for (uint32_t i = 0; levels_count > i; i++)
			{
				uint32_t mip_map_data_length = m_stream->read_unsigned_int();
				wk::MemoryStream mip_map_buffer(mip_map_data_length);
				m_stream->read(mip_map_buffer.data(), mip_map_data_length);

				auto mip_map_data = SCTX::GetMipMap(mip_map_buffer.data());

				std::vector<uint8_t> hash(mip_map_data->hash()->begin(), mip_map_data->hash()->end());
				m_levels.emplace_back(mip_map_data->width(), mip_map_data->height(), mip_map_data->offset(), hash);
			}
		}

		m_stream->seek(texture_data_offset);
	}

	bool SupercellTexture::read_data()
	{
		if (!m_stream) return false;
		if (m_texture_data_length == 0) return false;

		m_data = wk::CreateRef<wk::MemoryStream>(m_texture_data_length);
		bool is_compressed = ZstdDecompressor::validate(*m_stream);

		if (is_compressed)
		{
			ZstdDecompressor decompressor;
			decompressor.decompress(*m_stream, *m_data);
		}
		else
		{
			m_stream->read(m_data->data(), m_texture_data_length);
		}

		m_stream.reset();
		return true;
	}

	Image::BasePixelType SupercellTexture::base_type() const
	{
		return ScPixel::get_base_pixel_type(m_pixel_type);
	}

	Image::ColorSpace SupercellTexture::colorspace() const
	{
		return ScPixel::get_colorspace(m_pixel_type);
	}

	Image::PixelDepth SupercellTexture::depth() const
	{
		return ScPixel::get_depth(m_pixel_type);
	}

	std::size_t SupercellTexture::data_length() const
	{
		return data_length(0);
	}

	std::uint8_t* SupercellTexture::data() const
	{
		return data(0);
	}

	std::size_t SupercellTexture::data_length(size_t level_index) const
	{
		if (!m_data) return 0;

		const ScTextureLevel& level = get_level(level_index);
		return m_data->length() - level.offset;
	}

	std::uint8_t* SupercellTexture::data(size_t level_index) const
	{
		if (!m_data) return nullptr;

		const ScTextureLevel& level = get_level(level_index);
		return (std::uint8_t*)m_data->data_ref() + level.offset;
	}

	void SupercellTexture::clear()
	{
		m_data.reset();
	}

	bool SupercellTexture::is_compressed() const
	{
		return true;
	}

	size_t SupercellTexture::level_count() const
	{
		return m_levels.size();
	}

	const ScPixel::Type SupercellTexture::pixel_type() const
	{
		return m_pixel_type;
	}

	const ScTextureLevel& SupercellTexture::get_level(size_t index) const
	{
		return m_levels[index];
	}

	std::size_t SupercellTexture::decompressed_data_length(size_t level_idx)
	{
		return Image::calculate_image_length(m_width, m_height, depth());
	}

	void SupercellTexture::decompress_data(wk::Stream& buffer, size_t level_idx)
	{
		if (!m_data) return;

		const ScTextureLevel& level = get_level(level_idx);
		wk::SharedMemoryStream stream((uint8_t*)m_data->data() + level.offset, m_data->length() - level.offset);

		SupercellTexture::decompress_data(m_width, m_height, m_pixel_type, stream, buffer);
	}

	std::size_t SupercellTexture::decompressed_data_length()
	{
		return decompressed_data_length(0);
	}

	void SupercellTexture::decompress_data(wk::Stream& buffer)
	{
		decompress_data(buffer, 0);
	}

	void SupercellTexture::write(wk::Stream& buffer)
	{
		write(buffer, false);
	}

	void SupercellTexture::write(wk::Stream& buffer, bool compress_data)
	{
		using namespace flatbuffers;
		FlatBufferBuilder builder;
		builder.TrackMinAlign(16);

		// Data chunk
		{
			Offset<SCTX::TextureVariants> off_texture_variants = 0;
			if (streaming_variants.has_value() && streaming_ids.has_value())
			{
				std::vector<Offset<SCTX::StreamingTextureDescriptor>> off_streaming_textures;
				off_streaming_textures.reserve(streaming_variants->size());

				for (auto& streaming_variant : streaming_variants.value())
				{
					Offset<Vector<uint8_t>> off_streaming_variant_data = builder.CreateVector(streaming_variant.data(), streaming_variant.data_length());
					Offset<SCTX::StreamingTextureDescriptor> off_streaming_variant =
						SCTX::CreateStreamingTextureDescriptor(
							builder,
							(uint32_t)streaming_variant.pixel_type(), streaming_variant.width(), streaming_variant.height(),
							off_streaming_variant_data
						);

					off_streaming_textures.push_back(off_streaming_variant);
				}

				off_texture_variants = SCTX::CreateTextureVariantsDirect(builder, &streaming_ids.value(), &off_streaming_textures);
			}

			Offset<SCTX::TextureData> off_texture_data = SCTX::CreateTextureData(
				builder, 0,
				(uint32_t)m_pixel_type, m_width, m_height, m_levels.size(),
				0, unknown_integer, (uint32_t)m_data->length(), 0, 0, off_texture_variants
			);

			builder.FinishSizePrefixed(off_texture_data, SCTX::TextureDataIdentifier());
			
			auto texture_data = builder.GetBufferSpan();
			buffer.write(texture_data.data(), (uint32_t)texture_data.size_bytes());
			builder.Clear();
		}

		// Mip maps chunk
		{
			wk::BufferStream mip_maps;
			for (const ScTextureLevel& level : m_levels)
			{
				Offset<SCTX::MipMap> off_mip_map = SCTX::CreateMipMapDirect(builder, level.width, level.height, level.offset, &level.hash);
				builder.FinishSizePrefixed(off_mip_map);

				auto mip_map_data = builder.GetBufferSpan();
				mip_maps.write(mip_map_data.data(), mip_map_data.size_bytes());
				builder.Clear();
			}

			buffer.write_unsigned_int(mip_maps.length());
			buffer.write(mip_maps.data(), mip_maps.length());
		}
		
		// Data itself
		buffer.write(m_data->data(), m_data->length());
	}

	void SupercellTexture::decompress_data(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output)
	{
		ScPixel::Compression compression = ScPixel::get_compression(type);

		input.seek(0);
		switch (compression)
		{
		case ScPixel::Compression::ASTC:
			decompress_astc(width, height, type, input, output);
			break;
		default:
			throw wk::Exception("Unsupported compression!");
		}
	}

	void SupercellTexture::decompress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output)
	{
		auto [x, y, z] = ScPixel::get_astc_blocks(type);

		ASTCDecompressor::Props props;
		props.blocks_x = x;
		props.blocks_y = y;
		props.profile = ScPixel::get_colorspace(type) == ColorSpace::Linear ? astc::Profile::PRF_LDR : astc::Profile::PRF_LDR_SRGB;

		ASTCDecompressor context(props);
		context.decompress(
			width, height, Image::BasePixelType::RGBA,
			input, output
		);
	}
}
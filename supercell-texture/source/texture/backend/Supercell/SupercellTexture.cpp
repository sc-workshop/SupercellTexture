#include "SupercellTexture.h"

#include "ScPixel.hpp"
#include "texture/SCTX/TextureData_generated.h"
#include "texture/SCTX/MipMapData_generated.h"

#include "compression/compression.h"
#include "core/crypto/fnv.h"

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

	SupercellTexture::SupercellTexture(wk::RawImage& image, ScPixel::Type type, bool mip_maps)
	{
		// Writable dynamic buffer
		m_data = wk::CreateRef<wk::BufferStream>();

		m_pixel_type = type;
		m_width = image.width();
		m_height = image.height();

		// Level 0
		create_new_level(image);

		if (mip_maps)
		{
			generate_mip_maps(image);
		}
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
			m_texture_data_length = texture->texture_length();

			set_data_flags((uint32_t)texture->flags());

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

					for (flatbuffers::Vector<SCTX::TextureVariants>::size_type i = 0; variants_data->size() > i; i++)
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

	void SupercellTexture::generate_mip_maps(wk::RawImage& image)
	{
		float width = (float)image.width();
		float height = (float)image.height();
		while (true)
		{

			width = width / 2.0f;
			height = height / 2.0f;
			if (1.0f > width && 1.0f > height) break;

			wk::RawImage mip_map(
				1.0f > width ? 1 : (uint16_t)std::ceil(width),
				1.0f > height ? 1 : (uint16_t)std::ceil(height),
				image.depth(), image.colorspace()
			);

			image.copy(mip_map);
			create_new_level(mip_map);
		}
	}

	void SupercellTexture::create_new_level(wk::RawImage& image)
	{
		if (!m_data) return;
		if (!m_data->is_writable()) return;

		ScPixel::Compression compression = ScPixel::get_compression(m_pixel_type);
		auto do_compress = [&compression, this](wk::RawImage& image)
			{
				wk::SharedMemoryStream input(image.data(), image.data_length());
				switch (compression)
				{
				case ScPixel::Compression::ASTC:
					SupercellTexture::compress_astc(image.width(), image.height(), m_pixel_type, input, *m_data);
					break;
				default:
					throw wk::Exception("Unsupported compression!");
					break;
				}
			};

		size_t data_begin = m_data->position();

		if (ScPixel::get_depth(m_pixel_type) != image.depth())
		{
			wk::RawImage temp(image.width(), image.height(), ScPixel::get_depth(m_pixel_type), image.colorspace());
			image.copy(temp);

			do_compress(temp);
		}
		else
		{
			do_compress(image);
		}
		
		size_t data_end = m_data->position();
		size_t level_index = m_levels.size();

		ScTextureLevel& level = m_levels.emplace_back();
		level.width = image.width();
		level.height = image.height();
		level.offset = (uint32_t)data_begin;

		uint64_t texture_hash = Fnv::fnv1a64(data(level_index), data_end - data_begin);
		level.hash.resize(sizeof(uint64_t));
		wk::Memory::copy(&texture_hash, level.hash.data(), level.hash.size());
	}

	bool SupercellTexture::read_data()
	{
		if (!m_stream) return false;
		if (m_texture_data_length == 0) return false;

		if (use_padding)
		{
			m_stream->seek((m_stream->position() + 15) & ~15);
		}

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
		return (std::uint8_t*)m_data->data() + level.offset;
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
		if (!m_data) return 0;

		const ScTextureLevel& level = get_level(level_idx);

		return Image::calculate_image_length(level.width, level.height, depth());
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
		using namespace flatbuffers;
		FlatBufferBuilder builder(2048);

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
				(uint32_t)m_pixel_type, m_width, m_height, (uint8_t)m_levels.size(),
				0, (SCTX::TextureFlags)get_data_flags(), (uint32_t)m_data->length(), 0, 0,
				off_texture_variants
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

			buffer.write_unsigned_int((uint32_t)mip_maps.length());
			buffer.write(mip_maps.data(), mip_maps.length());
		}

		if (use_padding)
		{
			size_t pad_bytes = ((buffer.position() + 15) & ~15) - buffer.position();
			for (size_t i = 0; pad_bytes > i; i++)
			{
				buffer.write_unsigned_byte(0);
			}
		}

		// Data itself
		if (use_compression)
		{
			ZstdCompressor::Props props;
			props.compression_level = 16;
			props.checksum_flag = false;
			props.content_size_flag = true;

			ZstdCompressor compressor(props);
			m_data->seek(0);
			compressor.compress(*m_data, buffer);
		}
		else
		{
			buffer.write(m_data->data(), m_data->length());
		}
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

	void SupercellTexture::set_data_flags(uint32_t flags)
	{
		using SCTX::TextureFlags;

		use_compression = (flags & (uint32_t)TextureFlags::use_compression) > 0;
		unknown_flag1 = (flags & (uint32_t)TextureFlags::unknown_flag2) > 0;
		unknown_flag2 = (flags & (uint32_t)TextureFlags::unknown_flag3) > 0;
		use_padding = (flags & (uint32_t)TextureFlags::use_padding) > 0;
	}

	uint32_t SupercellTexture::get_data_flags() const
	{
		using SCTX::TextureFlags;

		uint8_t result = 0;

		if (use_compression) result |= (uint32_t)TextureFlags::use_compression;
		if (unknown_flag1) result |= (uint32_t)TextureFlags::unknown_flag2;
		if (unknown_flag2) result |= (uint32_t)TextureFlags::unknown_flag3;
		if (use_padding) result |= (uint32_t)TextureFlags::use_padding;

		return result;
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

	void SupercellTexture::compress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output)
	{
		auto [x, y, z] = ScPixel::get_astc_blocks(type);

		ASTCCompressor::Props props;
		props.blocks_x = x;
		props.blocks_y = y;

		ASTCCompressor context(props);
		context.compress(width, height, ScPixel::get_base_pixel_type(type), input, output);
	}
}
#include "KhronosTexture.h"
#include "KhronosTexture1.h"
#include "KhronosTexture2.h"

using namespace wk;

namespace sc::texture
{
#pragma region Static Functions
	Image::PixelDepth KhronosTexture::format_depth(glInternalFormat format)
	{
		switch (format)
		{
		case sc::texture::KhronosTexture::glInternalFormat::GL_RGBA8:
			break;
		case sc::texture::KhronosTexture::glInternalFormat::GL_RGB8:
			break;
		case sc::texture::KhronosTexture::glInternalFormat::GL_LUMINANCE:
			break;
		case sc::texture::KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			break;
		
		default:
			break;
		}
		switch (format)
		{
		case KhronosTexture::glInternalFormat::GL_RGBA8:
			return PixelDepth::RGBA8;
		case KhronosTexture::glInternalFormat::GL_RGB8:
			return PixelDepth::RGB8;
		case KhronosTexture::glInternalFormat::GL_LUMINANCE:
			return PixelDepth::LUMINANCE8;
		case KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return PixelDepth::LUMINANCE8_ALPHA8;

		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x12:
			return PixelDepth::RGBA8;

		default:
			// assert(0 && "Unknown glInternalFormat");
			return PixelDepth::RGBA8;
		}
	}

	Image::BasePixelType KhronosTexture::format_type(glFormat format)
	{
		switch (format)
		{
		case KhronosTexture::glFormat::GL_R:
			return BasePixelType::L;

		case KhronosTexture::glFormat::GL_RG:
			return BasePixelType::LA;

		case KhronosTexture::glFormat::GL_SRGB:
		case KhronosTexture::glFormat::GL_RGB:
			return BasePixelType::RGB;

		case KhronosTexture::glFormat::GL_SRGB_ALPHA:
		case KhronosTexture::glFormat::GL_RGBA:
			return BasePixelType::RGBA;

		default:
			//assert(0 && "Unknown glFormat");
			return BasePixelType::RGBA;
		}
	}

	Image::ColorSpace KhronosTexture::format_colorspace(glFormat format)
	{
		switch (format)
		{
		case KhronosTexture::glFormat::GL_RGBA:
		case KhronosTexture::glFormat::GL_RGB:
		case KhronosTexture::glFormat::GL_RG:
		case KhronosTexture::glFormat::GL_R:
			return ColorSpace::Linear;

		case KhronosTexture::glFormat::GL_SRGB:
		case KhronosTexture::glFormat::GL_SRGB_ALPHA:
			return ColorSpace::sRGB;

		default:
			//assert(0 && "Unknown glFormat");
			return ColorSpace::Linear;
		}
	}

	KhronosTexture::KhronosTextureCompression KhronosTexture::format_compression_type(glInternalFormat format)
	{
		switch (format)
		{
		case KhronosTexture::glInternalFormat::GL_RGBA8:
		case KhronosTexture::glInternalFormat::GL_RGB8:
		case KhronosTexture::glInternalFormat::GL_LUMINANCE:
		case KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return KhronosTextureCompression::None;

		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x12:
			return KhronosTextureCompression::ASTC;
		default:
			//assert(0 && "Unknown glInternalFormat");
			return KhronosTextureCompression::None;
		}
	}

	KhronosTexture::glFormat KhronosTexture::type(glInternalFormat format)
	{
		switch (format)
		{
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x4:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x5:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x6:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x8:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x10:
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x12:
		case KhronosTexture::glInternalFormat::GL_RGBA8:
			return glFormat::GL_RGBA;

		case KhronosTexture::glInternalFormat::GL_RGB8:
			return glFormat::GL_RGB;
		case KhronosTexture::glInternalFormat::GL_LUMINANCE:
			return glFormat::GL_RG;
		case KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
			return glFormat::GL_R;

		default:
			//assert(0 && "Unknown glInternalFormat");
			return glFormat::GL_RGBA;
		}
	}

	Image::PixelDepth KhronosTexture::format_depth(VkFormat format)
	{
		switch (format)
		{
		case VkFormat::R8_UNORM:
		case VkFormat::R8_SRGB:
			return Image::PixelDepth::LUMINANCE8;
		case VkFormat::R8G8_UNORM:
		case VkFormat::R8G8_SRGB:
			return Image::PixelDepth::LUMINANCE8_ALPHA8;
		case VkFormat::R8G8B8_UNORM:
		case VkFormat::R8G8B8_SRGB:
			return Image::PixelDepth::RGB8;
		case VkFormat::R8G8B8A8_UNORM:
		case VkFormat::R8G8B8A8_SRGB:
			return Image::PixelDepth::RGBA8;
		case VkFormat::ASTC_4x4_UNORM_BLOCK:
		case VkFormat::ASTC_4x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x4_UNORM_BLOCK:
		case VkFormat::ASTC_5x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x5_UNORM_BLOCK:
		case VkFormat::ASTC_5x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x5_UNORM_BLOCK:
		case VkFormat::ASTC_6x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x6_UNORM_BLOCK:
		case VkFormat::ASTC_6x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x5_UNORM_BLOCK:
		case VkFormat::ASTC_8x5_SRGB_BLOCK:
		case VkFormat::ASTC_8x6_UNORM_BLOCK:
		case VkFormat::ASTC_8x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x8_UNORM_BLOCK:
		case VkFormat::ASTC_8x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x5_UNORM_BLOCK:
		case VkFormat::ASTC_10x5_SRGB_BLOCK:
		case VkFormat::ASTC_10x6_UNORM_BLOCK:
		case VkFormat::ASTC_10x6_SRGB_BLOCK:
		case VkFormat::ASTC_10x8_UNORM_BLOCK:
		case VkFormat::ASTC_10x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x10_UNORM_BLOCK:
		case VkFormat::ASTC_10x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x10_UNORM_BLOCK:
		case VkFormat::ASTC_12x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x12_UNORM_BLOCK:
		case VkFormat::ASTC_12x12_SRGB_BLOCK:
			return Image::PixelDepth::RGBA8;
		default:
			return Image::PixelDepth::RGBA8;
		}
	}

	Image::BasePixelType KhronosTexture::format_type(VkFormat format)
	{
		switch (format)
		{
		case VkFormat::R8_UNORM:
		case VkFormat::R8_SRGB:
			return Image::BasePixelType::L;
		case VkFormat::R8G8_UNORM:
		case VkFormat::R8G8_SRGB:
			return Image::BasePixelType::LA;
		case VkFormat::R8G8B8_UNORM:
		case VkFormat::R8G8B8_SRGB:
			return Image::BasePixelType::RGB;
		case VkFormat::R8G8B8A8_UNORM:
		case VkFormat::R8G8B8A8_SRGB:
			return Image::BasePixelType::RGBA;
		case VkFormat::ASTC_4x4_UNORM_BLOCK:
		case VkFormat::ASTC_4x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x4_UNORM_BLOCK:
		case VkFormat::ASTC_5x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x5_UNORM_BLOCK:
		case VkFormat::ASTC_5x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x5_UNORM_BLOCK:
		case VkFormat::ASTC_6x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x6_UNORM_BLOCK:
		case VkFormat::ASTC_6x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x5_UNORM_BLOCK:
		case VkFormat::ASTC_8x5_SRGB_BLOCK:
		case VkFormat::ASTC_8x6_UNORM_BLOCK:
		case VkFormat::ASTC_8x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x8_UNORM_BLOCK:
		case VkFormat::ASTC_8x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x5_UNORM_BLOCK:
		case VkFormat::ASTC_10x5_SRGB_BLOCK:
		case VkFormat::ASTC_10x6_UNORM_BLOCK:
		case VkFormat::ASTC_10x6_SRGB_BLOCK:
		case VkFormat::ASTC_10x8_UNORM_BLOCK:
		case VkFormat::ASTC_10x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x10_UNORM_BLOCK:
		case VkFormat::ASTC_10x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x10_UNORM_BLOCK:
		case VkFormat::ASTC_12x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x12_UNORM_BLOCK:
		case VkFormat::ASTC_12x12_SRGB_BLOCK:
			return Image::BasePixelType::RGBA;
		default:
			return Image::BasePixelType::RGBA;
		}
	}

	Image::ColorSpace KhronosTexture::format_colorspace(VkFormat format)
	{
		switch (format)
		{
		case VkFormat::R8_UNORM:
		case VkFormat::R8G8_UNORM:
		case VkFormat::R8G8B8_UNORM:
		case VkFormat::R8G8B8A8_UNORM:
		case VkFormat::ASTC_4x4_UNORM_BLOCK:
		case VkFormat::ASTC_5x4_UNORM_BLOCK:
		case VkFormat::ASTC_5x5_UNORM_BLOCK:
		case VkFormat::ASTC_6x5_UNORM_BLOCK:
		case VkFormat::ASTC_6x6_UNORM_BLOCK:
		case VkFormat::ASTC_8x5_UNORM_BLOCK:
		case VkFormat::ASTC_8x6_UNORM_BLOCK:
		case VkFormat::ASTC_8x8_UNORM_BLOCK:
		case VkFormat::ASTC_10x5_UNORM_BLOCK:
		case VkFormat::ASTC_10x6_UNORM_BLOCK:
		case VkFormat::ASTC_10x8_UNORM_BLOCK:
		case VkFormat::ASTC_10x10_UNORM_BLOCK:
		case VkFormat::ASTC_12x10_UNORM_BLOCK:
		case VkFormat::ASTC_12x12_UNORM_BLOCK:
			return Image::ColorSpace::Linear;

		case VkFormat::R8_SRGB:
		case VkFormat::R8G8_SRGB:
		case VkFormat::R8G8B8_SRGB:
		case VkFormat::R8G8B8A8_SRGB:
		case VkFormat::ASTC_4x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x4_SRGB_BLOCK:
		case VkFormat::ASTC_5x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x5_SRGB_BLOCK:
		case VkFormat::ASTC_6x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x5_SRGB_BLOCK:
		case VkFormat::ASTC_8x6_SRGB_BLOCK:
		case VkFormat::ASTC_8x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x5_SRGB_BLOCK:
		case VkFormat::ASTC_10x6_SRGB_BLOCK:
		case VkFormat::ASTC_10x8_SRGB_BLOCK:
		case VkFormat::ASTC_10x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x10_SRGB_BLOCK:
		case VkFormat::ASTC_12x12_SRGB_BLOCK:
			return Image::ColorSpace::sRGB;

		default:
			return Image::ColorSpace::Linear;
		}
	}
	;
#pragma endregion

#pragma region
	void KhronosTexture::get_astc_blocks(glInternalFormat format, uint8_t& x, uint8_t& y, uint8_t& z)
	{
		switch (format)
		{
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x4:
			x = 5; y = 4; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x5:
			x = 6; y = 5; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x5:
			x = 8; y = 5; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x6:
			x = 8; y = 6; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x5:
			x = 10; y = 5; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x6:
			x = 10; y = 6; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x8:
			x = 10; y = 8; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x10:
			x = 10; y = 10; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x10:
			x = 12; y = 10; z = 0;
			break;
		case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x12:
			x = 12; y = 12; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
			x = 4; y = 4; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			x = 5; y = 5; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			x = 6; y = 6; z = 0;
			break;
		case glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			x = 8; y = 8; z = 0;
			break;
		default:
			x = 0; y = 0; z = 0;
			break;
		}
	}

	void KhronosTexture::decompress_astc(Stream& input, Stream& output, uint16_t width, uint16_t height)
	{
		uint8_t blocks_x;
		uint8_t blocks_y;
		uint8_t blocks_z;
		get_astc_blocks(internal_format(), blocks_x, blocks_y, blocks_z);

		ASTCDecompressor::Props props;
		props.blocks_x = blocks_x;
		props.blocks_y = blocks_y;
		props.profile = colorspace() == ColorSpace::Linear ? astc::Profile::PRF_LDR : astc::Profile::PRF_LDR_SRGB;

		ASTCDecompressor context(props);
		context.decompress(
			width, height, Image::BasePixelType::RGBA,
			input, output
		);
	}

	void KhronosTexture::compress_astc(Stream& input, Stream& output)
	{
		uint8_t blocks_x;
		uint8_t blocks_y;
		uint8_t blocks_z;
		get_astc_blocks(internal_format(), blocks_x, blocks_y, blocks_z);

		ASTCCompressor::Props props;
		props.blocks_x = blocks_x;
		props.blocks_y = blocks_y;

		ASTCCompressor context(props);
		context.compress(m_width, m_height, Image::BasePixelType::RGBA, input, output);
	}
#pragma endregion ASTC Compression

	wk::Ref<KhronosTexture> KhronosTexture::load_texture(wk::Stream& stream)
	{
		size_t position = stream.position();
		std::array<uint8_t, 12> magic;
		stream.read(magic.data(), 12);
		stream.seek(position);

		if (std::memcmp(magic.data(), &KhronosTexture1::FileIdentifier, 12) == 0)
		{
			return wk::CreateRef<KhronosTexture1>(stream);
		}
		else if (std::memcmp(magic.data(), &KhronosTexture2::FileIdentifier, 12) == 0)
		{
			return wk::CreateRef<KhronosTexture2>(stream);
		}
		else
		{
			throw wk::Exception("Wrong or damaged KTX data buffer");
		}
	}

	uint32_t KhronosTexture::level_count() const
	{
		return (uint32_t)m_levels.size();
	}

	size_t KhronosTexture::decompressed_data_length()
	{
		return decompressed_data_length(0);
	}

	size_t KhronosTexture::decompressed_data_length(uint32_t level_index)
	{
		return Image::calculate_image_length(
			m_width / (uint16_t)(pow(2, level_index)),
			m_height / (uint16_t)(pow(2, level_index)),
			depth()
		);
	}

	bool KhronosTexture::is_compressed() const
	{
		return compression_type() != KhronosTextureCompression::None;
	}

	void KhronosTexture::decompress_data(Stream& output)
	{
		return decompress_data(output, 0);
	}

	void KhronosTexture::decompress_data(Stream& output, uint32_t level_index)
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;
		auto& buffer = m_levels[level_index];

		uint16_t level_width = m_width / (uint16_t)pow(2, level_index);
		uint16_t level_height = m_height / (uint16_t)pow(2, level_index);

		buffer->seek(0);
		switch (compression_type())
		{
		case KhronosTextureCompression::ASTC:
			decompress_astc(*buffer, output, level_width, level_height);
			break;

		default:
			output.write(m_levels[level_index]->data(), m_levels[level_index]->length());
			break;
		}
	}

	void KhronosTexture::set_level_data(Stream& stream, Image::PixelDepth source_depth, uint32_t level_index)
	{
		// First, check if level index is ok
		// If index out of bound - create new buffer
		if (level_index >= m_levels.size())
		{
			level_index = static_cast<uint32_t>(m_levels.size());
			m_levels.resize(m_levels.size() + 1);
		};

		// Level final data buffer
		auto buffer = CreateRef<BufferStream>();
		BufferStream temp_buffer;
		bool use_temp_buffer = false;

		// Second, we need to convert data base type to current texture type
		Image::PixelDepth destination_depth = depth();

		if (source_depth != destination_depth)
		{
			temp_buffer.resize(
				Image::calculate_image_length(m_width, m_height, destination_depth)
			);

			Image::remap(
				(uint8_t*)stream.data(), (uint8_t*)temp_buffer.data(),
				m_width, m_height,
				source_depth, destination_depth
			);

			use_temp_buffer = true;
		}

		SharedMemoryStream input_image(
			use_temp_buffer ? (uint8_t*)temp_buffer.data() : (uint8_t*)stream.data(),
			use_temp_buffer ? temp_buffer.length() : stream.length()
		);

		switch (compression_type())
		{
		case KhronosTextureCompression::ASTC:
			compress_astc(input_image, *buffer);
			break;

		default:
			buffer->resize(input_image.length());
			Memory::copy(
				input_image.data(),
				buffer->data(),
				input_image.length()
			);
			break;
		}

		m_levels[level_index] = buffer;
	}

	void KhronosTexture::reset_level_data(uint32_t level_index)
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		m_levels.erase(m_levels.begin(), m_levels.begin() + level_index);
	}

	size_t KhronosTexture::data_length() const
	{
		return data_length(0);
	}

	size_t KhronosTexture::data_length(uint32_t level_index) const
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		return m_levels[level_index]->length();
	}

	uint8_t* KhronosTexture::data() const
	{
		auto buffer = data(0);

		if (buffer) return (uint8_t*)buffer->data();

		return nullptr;
	}

	const Ref<Stream> KhronosTexture::data(uint32_t level_index) const
	{
		if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

		return m_levels[level_index];
	}
}
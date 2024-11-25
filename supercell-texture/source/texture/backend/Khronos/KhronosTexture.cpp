#include "KhronosTexture.h"

using namespace wk;

namespace sc
{
	namespace texture
	{
#pragma region Static Functions
		Image::PixelDepth KhronosTexture::format_depth(glInternalFormat format)
		{
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
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
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

		bool KhronosTexture::format_compression(glInternalFormat format)
		{
			switch (format)
			{
			case KhronosTexture::glInternalFormat::GL_RGBA8:
			case KhronosTexture::glInternalFormat::GL_RGB8:
			case KhronosTexture::glInternalFormat::GL_LUMINANCE:
			case KhronosTexture::glInternalFormat::GL_LUMINANCE_ALPHA:
				return false;

			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
				return true;

			default:
				//assert(0 && "Unknown glInternalFormat");
				return false;
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
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
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
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
			case KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
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
		};
#pragma endregion

#pragma region
		void KhronosTexture::get_astc_blocks(glInternalFormat format, uint8_t& x, uint8_t& y, uint8_t& z)
		{
			switch (format)
			{
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
	}
}
#pragma once

#include "texture/interface/compressed_image.h"
#include "compression/backend/astc/astc.h"
#include "core/exception/exception.h"
#include "core/memory/ref.h"
#include "core/memory/memory.h"
#include "core/preprocessor/api.h"

#include "core/io/buffer_stream.h"
#include "core/io/memory_stream.h"
#include "core/image/raw_image.h"

#include "compression/compression.h"

namespace sc
{
	namespace texture
	{
		class WORKSHOP_API KhronosTexture : public CompressedImage
		{
			// Khronos shared definitions
		public:
			enum class KhronosTextureCompression
			{
				None = 0,
				ASTC
			};

			// Khronos V1 definitions
		public:
			enum class glInternalFormat : uint32_t {
				GL_RGBA8 = 0x8058,
				GL_RGB8 = 0x8051,
				GL_LUMINANCE = 0x1909,
				GL_LUMINANCE_ALPHA = 0x190A,

				// ASTC
				GL_COMPRESSED_RGBA_ASTC_4x4 = 0x93B0,
				GL_COMPRESSED_RGBA_ASTC_5x5 = 0x93B2,
				GL_COMPRESSED_RGBA_ASTC_6x6 = 0x93B4,
				GL_COMPRESSED_RGBA_ASTC_8x8 = 0x93B7,
			};

			enum class glType : uint32_t {
				COMPRESSED = 0,

				//GL_BYTE = 0x1400,
				GL_UNSIGNED_BYTE = 0x1401,
				//GL_SHORT = 0x1402,
				//GL_UNSIGNED_SHORT = 0x1403,
				//GL_HALF_FLOAT = 0x140B,
				//GL_FLOAT = 0x1406,
				//GL_FIXED = 0x140C,
			};

			enum class glFormat : uint32_t {
				UNKNOWN = 0,

				GL_R = 0x1903,
				GL_RG = 0x8227,
				GL_RGB = 0x1907,
				GL_RGBA = 0x1908,
				GL_SRGB = 0x8C40,
				GL_SRGB_ALPHA = 0x8C42
			};

		public:
			virtual KhronosTextureCompression compression_type() = 0;
			virtual uint32_t level_count() const = 0;

			// Khronos V1 Format Getters
		public:
			virtual glInternalFormat internal_format() const = 0;
			virtual glFormat format() const = 0;
			virtual glType type() const = 0;

			// Parent class data managers
		public:
			virtual std::size_t decompressed_data_length() = 0;
			virtual void decompress_data(wk::Stream& buffer) = 0;

			virtual std::size_t data_length() const = 0;
			virtual std::uint8_t* data() const = 0;

			// Khronos shared data managers
		public:
			virtual void decompress_data(wk::Stream& output, uint32_t level_index) = 0;
			virtual void set_level_data(wk::Stream& data, Image::PixelDepth data_format, uint32_t level_index) = 0;
			virtual void reset_level_data(uint32_t level_index) = 0;

			// Image functions for Khronos mip mapping system
		public:
			virtual size_t data_length(uint32_t level_index) const = 0;
			virtual const wk::BufferStream* data(uint32_t level_index) const = 0;

			// Khronos V1 format converters
		public:
			static Image::PixelDepth format_depth(glInternalFormat format);
			static Image::BasePixelType format_type(glFormat format);
			static Image::ColorSpace format_colorspace(glFormat format);
			static bool format_compression(glInternalFormat format);
			static KhronosTextureCompression format_compression_type(glInternalFormat format);
			static glFormat type(glInternalFormat format);

		protected:
#pragma region
			static void get_astc_blocks(glInternalFormat format, uint8_t& x, uint8_t& y, uint8_t& z);
			void decompress_astc(wk::Stream& input, wk::Stream& output, uint16_t width, uint16_t height);
			void compress_astc(wk::Stream& input, wk::Stream& output);

#pragma endregion ASTC
		};
	}
}

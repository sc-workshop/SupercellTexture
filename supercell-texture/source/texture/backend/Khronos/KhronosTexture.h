#pragma once

#include "texture/interface/compressed_image.h"
#include "compression/backend/astc/astc.h"
#include "core/exception/exception.h"
#include "core/memory/ref.h"
#include "core/memory/memory.h"
#include "core/preprocessor/api.h"

#include "core/io/shared_memory_stream.h"
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
			enum class Version
			{
				v1, // KTX 1.0
				v2  // KTX 2.0
			};

			enum class KhronosTextureCompression
			{
				None = 0,
				ASTC
			};

			// Khronos V1 definitions
		public:
			enum class glInternalFormat : uint32_t {
				UNKNOWN = 0,

				// Uncompressed formats

				RGBA8 = 0x8058,
				RGB8 = 0x8051,
				LUMINANCE = 0x1909,
				LUMINANCE_ALPHA = 0x190A,

				// ASTC

				COMPRESSED_RGBA_ASTC_4x4 = 0x93B0,
				COMPRESSED_RGBA_ASTC_5x4 = 0x93B1,
				COMPRESSED_RGBA_ASTC_5x5 = 0x93B2,
				COMPRESSED_RGBA_ASTC_6x5 = 0x93B3,
				COMPRESSED_RGBA_ASTC_6x6 = 0x93B4,
				COMPRESSED_RGBA_ASTC_8x5 = 0x93B5,
				COMPRESSED_RGBA_ASTC_8x6 = 0x93B6,
				COMPRESSED_RGBA_ASTC_8x8 = 0x93B7,
				COMPRESSED_RGBA_ASTC_10x5 = 0x93B8,
				COMPRESSED_RGBA_ASTC_10x6 = 0x93B9,
				COMPRESSED_RGBA_ASTC_10x8 = 0x93BA,
				COMPRESSED_RGBA_ASTC_10x10 = 0x93BB,
				COMPRESSED_RGBA_ASTC_12x10 = 0x93BC,
				COMPRESSED_RGBA_ASTC_12x12 = 0x93BD,

				COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 = 0x93D0,
				COMPRESSED_SRGB8_ALPHA8_ASTC_5x4 = 0x93D1,
				COMPRESSED_SRGB8_ALPHA8_ASTC_5x5 = 0x93D2,
				COMPRESSED_SRGB8_ALPHA8_ASTC_6x5 = 0x93D3,
				COMPRESSED_SRGB8_ALPHA8_ASTC_6x6 = 0x93D4,
				COMPRESSED_SRGB8_ALPHA8_ASTC_8x5 = 0x93D5,
				COMPRESSED_SRGB8_ALPHA8_ASTC_8x6 = 0x93D6,
				COMPRESSED_SRGB8_ALPHA8_ASTC_8x8 = 0x93D7,
				COMPRESSED_SRGB8_ALPHA8_ASTC_10x5 = 0x93D8,
				COMPRESSED_SRGB8_ALPHA8_ASTC_10x6 = 0x93D9,
				COMPRESSED_SRGB8_ALPHA8_ASTC_10x8 = 0x93DA,
				COMPRESSED_SRGB8_ALPHA8_ASTC_10x10 = 0x93DB,
				COMPRESSED_SRGB8_ALPHA8_ASTC_12x10 = 0x93DC,
				COMPRESSED_SRGB8_ALPHA8_ASTC_12x12 = 0x93DD,
			};	

			enum class glType : uint32_t {
				COMPRESSED = 0,

				//BYTE = 0x1400,
				UNSIGNED_BYTE = 0x1401,
				//SHORT = 0x1402,
				//UNSIGNED_SHORT = 0x1403,
				//HALF_FLOAT = 0x140B,
				//FLOAT = 0x1406,
				//FIXED = 0x140C,
			};

			enum class glFormat : uint32_t {
				UNKNOWN = 0,

				R = 0x1903,
				RG = 0x8227,
				RGB = 0x1907,
				RGBA = 0x1908,
				SRGB = 0x8C40,
				SRGB_ALPHA = 0x8C42
			};

			// Khronos V2 definitions
		public:
			enum class VkFormat : uint32_t
			{
				Unknown = 0,

				R8_UNORM = 9,
				R8_SRGB = 15,
				R8G8_UNORM = 16,
				R8G8_SRGB = 22,
				R8G8B8_UNORM = 23,
				R8G8B8_SRGB = 29,
				R8G8B8A8_UNORM = 37,
				R8G8B8A8_SRGB = 43,

				ASTC_4x4_UNORM_BLOCK = 157,
				ASTC_4x4_SRGB_BLOCK = 158,
				ASTC_5x4_UNORM_BLOCK = 159,
				ASTC_5x4_SRGB_BLOCK = 160,
				ASTC_5x5_UNORM_BLOCK = 161,
				ASTC_5x5_SRGB_BLOCK = 162,
				ASTC_6x5_UNORM_BLOCK = 163,
				ASTC_6x5_SRGB_BLOCK = 164,
				ASTC_6x6_UNORM_BLOCK = 165,
				ASTC_6x6_SRGB_BLOCK = 166,
				ASTC_8x5_UNORM_BLOCK = 167,
				ASTC_8x5_SRGB_BLOCK = 168,
				ASTC_8x6_UNORM_BLOCK = 169,
				ASTC_8x6_SRGB_BLOCK = 170,
				ASTC_8x8_UNORM_BLOCK = 171,
				ASTC_8x8_SRGB_BLOCK = 172,
				ASTC_10x5_UNORM_BLOCK = 173,
				ASTC_10x5_SRGB_BLOCK = 174,
				ASTC_10x6_UNORM_BLOCK = 175,
				ASTC_10x6_SRGB_BLOCK = 176,
				ASTC_10x8_UNORM_BLOCK = 177,
				ASTC_10x8_SRGB_BLOCK = 178,
				ASTC_10x10_UNORM_BLOCK = 179,
				ASTC_10x10_SRGB_BLOCK = 180,
				ASTC_12x10_UNORM_BLOCK = 181,
				ASTC_12x10_SRGB_BLOCK = 182,
				ASTC_12x12_UNORM_BLOCK = 183,
				ASTC_12x12_SRGB_BLOCK = 184,
			};

		public:
			virtual ~KhronosTexture() = default;
			static wk::Ref<KhronosTexture> load_texture(wk::Stream&);

		public:
			virtual Version version() const = 0;
			virtual KhronosTextureCompression compression_type() const = 0;
			uint32_t level_count() const;

			virtual size_t decompressed_data_length();
			size_t decompressed_data_length(uint32_t level_index);

			virtual bool is_compressed() const;

			// Khronos V1 Format Getters
		public:
			virtual glInternalFormat internal_format() const = 0;
			virtual glFormat format() const = 0;
			virtual glType type() const = 0;

		public:
			virtual size_t data_length() const;
			virtual size_t data_length(uint32_t level_index) const;

			virtual uint8_t* data() const;
			virtual const wk::Ref<wk::Stream> data(uint32_t level_index) const;

			virtual void decompress_data(wk::Stream& output);
			virtual void decompress_data(wk::Stream& output, uint32_t level_index);

			virtual void set_level_data(wk::Stream& data, Image::PixelDepth data_format, uint32_t level_index);
			virtual void reset_level_data(uint32_t level_index);

			// Khronos V1 format converters
		public:
			static Image::PixelDepth format_depth(glInternalFormat format);
			static Image::BasePixelType format_type(glFormat format);
			static Image::ColorSpace format_colorspace(glFormat format, glInternalFormat internal);
			static KhronosTextureCompression format_compression_type(glInternalFormat format);
			static glFormat type(glInternalFormat format);

			// Khronos V2 format converters
		public:
			static Image::PixelDepth format_depth(VkFormat format);
			static Image::BasePixelType format_type(VkFormat format);
			static Image::ColorSpace format_colorspace(VkFormat format);

		protected:
#pragma region
			static void get_astc_blocks(glInternalFormat format, uint8_t& x, uint8_t& y, uint8_t& z);
			void decompress_astc(wk::Stream& input, wk::Stream& output, uint16_t width, uint16_t height);
			void compress_astc(wk::Stream& input, wk::Stream& output);

#pragma endregion ASTC

			// Memory
		protected:
			std::vector<wk::Ref<wk::BufferStream>> m_levels;
		};
	}
}

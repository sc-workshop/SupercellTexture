#pragma once

#include "core/image/image.h"
#include "core/exception/exception.h"

namespace sc::texture
{
	class ScPixel
	{
	public:
		ScPixel() = delete; // Static class

	public:
		enum class Type
		{
			UNKNOWN = 0,

			// ETC2 / EAC
			EAC_R11 = 170,
			EAC_SIGNED_R11 = 172,
			EAC_RG11 = 174,
			EAC_SIGNED_RG11 = 176,
			ETC2_EAC_RGBA8 = 178,
			ETC2_EAC_SRGBA8 = 179,
			ETC2_RGB8 = 180,
			ETC2_SRGB8 = 181,
			ETC2_RGB8_PUNCHTHROUGH_ALPHA1 = 182,
			ETC2_SRGB8_PUNCHTHROUGH_ALPHA1 = 183,


			// ASTC SRGBA8
			ASTC_SRGBA8_4x4 = 186,
			ASTC_SRGBA8_5x4 = 187,
			ASTC_SRGBA8_5x5 = 188,
			ASTC_SRGBA8_6x5 = 189,
			ASTC_SRGBA8_6x6 = 190,
			ASTC_SRGBA8_8x5 = 192,
			ASTC_SRGBA8_8x6 = 193,
			ASTC_SRGBA8_8x8 = 194,
			ASTC_SRGBA8_10x5 = 195,
			ASTC_SRGBA8_10x6 = 196,
			ASTC_SRGBA8_10x8 = 197,
			ASTC_SRGBA8_10x10 = 198,
			ASTC_SRGBA8_12x10 = 199,
			ASTC_SRGBA8_12x12 = 200,

			// ASTC RGBA8
			ASTC_RGBA8_4x4 = 204,
			ASTC_RGBA8_5x4 = 205,
			ASTC_RGBA8_5x5 = 206,
			ASTC_RGBA8_6x5 = 207,
			ASTC_RGBA8_6x6 = 208,
			ASTC_RGBA8_8x5 = 210,
			ASTC_RGBA8_8x6 = 211,
			ASTC_RGBA8_8x8 = 212,
			ASTC_RGBA8_10x5 = 213,
			ASTC_RGBA8_10x6 = 214,
			ASTC_RGBA8_10x8 = 215,
			ASTC_RGBA8_10x10 = 216,
			ASTC_RGBA8_12x10 = 217,
			ASTC_RGBA8_12x12 = 218,

			// ETC1
			ETC1_RGB8 = 263,
		};

		enum class Compression
		{
			UNKNOWN,
			EAC,
			ETC1,
			ETC2,
			ASTC
		};

		/*
		
		switch (type)
			{
			case ScPixel::Type::EAC_R11:
			case ScPixel::Type::EAC_SIGNED_R11:
			case ScPixel::Type::EAC_RG11:
			case ScPixel::Type::EAC_SIGNED_RG11:
			case ScPixel::Type::ETC2_EAC_RGBA8:
			case ScPixel::Type::ETC2_EAC_SRGBA8:
			case ScPixel::Type::ETC2_RGB8:
			case ScPixel::Type::ETC2_SRGB8:
			case ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ASTC_SRGBA8_4x4:
			case ScPixel::Type::ASTC_SRGBA8_5x4:
			case ScPixel::Type::ASTC_SRGBA8_5x5:
			case ScPixel::Type::ASTC_SRGBA8_6x5:
			case ScPixel::Type::ASTC_SRGBA8_6x6:
			case ScPixel::Type::ASTC_SRGBA8_8x5:
			case ScPixel::Type::ASTC_SRGBA8_8x6:
			case ScPixel::Type::ASTC_SRGBA8_8x8:
			case ScPixel::Type::ASTC_SRGBA8_10x5:
			case ScPixel::Type::ASTC_SRGBA8_10x6:
			case ScPixel::Type::ASTC_SRGBA8_10x8:
			case ScPixel::Type::ASTC_SRGBA8_10x10:
			case ScPixel::Type::ASTC_SRGBA8_12x10:
			case ScPixel::Type::ASTC_SRGBA8_12x12:
			case ScPixel::Type::ASTC_RGBA8_4x4:
			case ScPixel::Type::ASTC_RGBA8_5x5:
			case ScPixel::Type::ASTC_RGBA8_6x5:
			case ScPixel::Type::ASTC_RGBA8_6x6:
			case ScPixel::Type::ASTC_RGBA8_8x5:
			case ScPixel::Type::ASTC_RGBA8_8x6:
			case ScPixel::Type::ASTC_RGBA8_8x8:
			case ScPixel::Type::ASTC_RGBA8_10x5:
			case ScPixel::Type::ASTC_RGBA8_10x6:
			case ScPixel::Type::ASTC_RGBA8_10x8:
			case ScPixel::Type::ASTC_RGBA8_10x10:
			case ScPixel::Type::ASTC_RGBA8_12x10:
			case ScPixel::Type::ASTC_RGBA8_12x12:
			case ScPixel::Type::ETC1_RGB8:
			default:

			}
		*/

		static Compression get_compression(ScPixel::Type type)
		{
			switch (type)
			{
			case ScPixel::Type::EAC_R11:
			case ScPixel::Type::EAC_SIGNED_R11:
			case ScPixel::Type::EAC_RG11:
			case ScPixel::Type::EAC_SIGNED_RG11:
				return Compression::EAC;

			case ScPixel::Type::ETC2_EAC_RGBA8:
			case ScPixel::Type::ETC2_EAC_SRGBA8:
			case ScPixel::Type::ETC2_RGB8:
			case ScPixel::Type::ETC2_SRGB8:
			case ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
				return Compression::ETC2;

			case ScPixel::Type::ASTC_SRGBA8_4x4:
			case ScPixel::Type::ASTC_SRGBA8_5x4:
			case ScPixel::Type::ASTC_SRGBA8_5x5:
			case ScPixel::Type::ASTC_SRGBA8_6x5:
			case ScPixel::Type::ASTC_SRGBA8_6x6:
			case ScPixel::Type::ASTC_SRGBA8_8x5:
			case ScPixel::Type::ASTC_SRGBA8_8x6:
			case ScPixel::Type::ASTC_SRGBA8_8x8:
			case ScPixel::Type::ASTC_SRGBA8_10x5:
			case ScPixel::Type::ASTC_SRGBA8_10x6:
			case ScPixel::Type::ASTC_SRGBA8_10x8:
			case ScPixel::Type::ASTC_SRGBA8_10x10:
			case ScPixel::Type::ASTC_SRGBA8_12x10:
			case ScPixel::Type::ASTC_SRGBA8_12x12:
			case ScPixel::Type::ASTC_RGBA8_4x4:
			case ScPixel::Type::ASTC_RGBA8_5x4:
			case ScPixel::Type::ASTC_RGBA8_5x5:
			case ScPixel::Type::ASTC_RGBA8_6x5:
			case ScPixel::Type::ASTC_RGBA8_6x6:
			case ScPixel::Type::ASTC_RGBA8_8x5:
			case ScPixel::Type::ASTC_RGBA8_8x6:
			case ScPixel::Type::ASTC_RGBA8_8x8:
			case ScPixel::Type::ASTC_RGBA8_10x5:
			case ScPixel::Type::ASTC_RGBA8_10x6:
			case ScPixel::Type::ASTC_RGBA8_10x8:
			case ScPixel::Type::ASTC_RGBA8_10x10:
			case ScPixel::Type::ASTC_RGBA8_12x10:
			case ScPixel::Type::ASTC_RGBA8_12x12:
				return Compression::ASTC;

			case ScPixel::Type::ETC1_RGB8:
				return Compression::ETC2;

			default:
				throw wk::Exception("Unsupported pixel type!");
			}
		}

		static wk::Image::PixelDepth get_depth(ScPixel::Type type)
		{
			switch (type)
			{
			case ScPixel::Type::ETC2_RGB8:
			case ScPixel::Type::ETC2_SRGB8:
			case ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC1_RGB8:
				return wk::Image::PixelDepth::RGB8;

			case ScPixel::Type::ETC2_EAC_RGBA8:
			case ScPixel::Type::ETC2_EAC_SRGBA8:
			case ScPixel::Type::ASTC_SRGBA8_4x4:
			case ScPixel::Type::ASTC_SRGBA8_5x4:
			case ScPixel::Type::ASTC_SRGBA8_5x5:
			case ScPixel::Type::ASTC_SRGBA8_6x5:
			case ScPixel::Type::ASTC_SRGBA8_6x6:
			case ScPixel::Type::ASTC_SRGBA8_8x5:
			case ScPixel::Type::ASTC_SRGBA8_8x6:
			case ScPixel::Type::ASTC_SRGBA8_8x8:
			case ScPixel::Type::ASTC_SRGBA8_10x5:
			case ScPixel::Type::ASTC_SRGBA8_10x6:
			case ScPixel::Type::ASTC_SRGBA8_10x8:
			case ScPixel::Type::ASTC_SRGBA8_10x10:
			case ScPixel::Type::ASTC_SRGBA8_12x10:
			case ScPixel::Type::ASTC_SRGBA8_12x12:
			case ScPixel::Type::ASTC_RGBA8_4x4:
			case ScPixel::Type::ASTC_RGBA8_5x4:
			case ScPixel::Type::ASTC_RGBA8_5x5:
			case ScPixel::Type::ASTC_RGBA8_6x5:
			case ScPixel::Type::ASTC_RGBA8_6x6:
			case ScPixel::Type::ASTC_RGBA8_8x5:
			case ScPixel::Type::ASTC_RGBA8_8x6:
			case ScPixel::Type::ASTC_RGBA8_8x8:
			case ScPixel::Type::ASTC_RGBA8_10x5:
			case ScPixel::Type::ASTC_RGBA8_10x6:
			case ScPixel::Type::ASTC_RGBA8_10x8:
			case ScPixel::Type::ASTC_RGBA8_10x10:
			case ScPixel::Type::ASTC_RGBA8_12x10:
			case ScPixel::Type::ASTC_RGBA8_12x12:
				return wk::Image::PixelDepth::RGB8;

			default:
				throw wk::Exception("Unsupported pixel type!");
			}
		}

		static wk::Image::ColorSpace get_colorspace(ScPixel::Type type)
		{
			switch (type)
			{
			case ScPixel::Type::EAC_R11:
			case ScPixel::Type::EAC_SIGNED_R11:
			case ScPixel::Type::EAC_RG11:
			case ScPixel::Type::EAC_SIGNED_RG11:
			case ScPixel::Type::ETC2_EAC_RGBA8:
			case ScPixel::Type::ETC2_RGB8:
			case ScPixel::Type::ASTC_RGBA8_4x4:
			case ScPixel::Type::ASTC_RGBA8_5x4:
			case ScPixel::Type::ASTC_RGBA8_5x5:
			case ScPixel::Type::ASTC_RGBA8_6x5:
			case ScPixel::Type::ASTC_RGBA8_6x6:
			case ScPixel::Type::ASTC_RGBA8_8x5:
			case ScPixel::Type::ASTC_RGBA8_8x6:
			case ScPixel::Type::ASTC_RGBA8_8x8:
			case ScPixel::Type::ASTC_RGBA8_10x5:
			case ScPixel::Type::ASTC_RGBA8_10x6:
			case ScPixel::Type::ASTC_RGBA8_10x8:
			case ScPixel::Type::ASTC_RGBA8_10x10:
			case ScPixel::Type::ASTC_RGBA8_12x10:
			case ScPixel::Type::ASTC_RGBA8_12x12:
			case ScPixel::Type::ETC1_RGB8:
				return wk::Image::ColorSpace::Linear;

			case ScPixel::Type::ETC2_EAC_SRGBA8:
			case ScPixel::Type::ETC2_SRGB8:
			case ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ASTC_SRGBA8_4x4:
			case ScPixel::Type::ASTC_SRGBA8_5x4:
			case ScPixel::Type::ASTC_SRGBA8_5x5:
			case ScPixel::Type::ASTC_SRGBA8_6x5:
			case ScPixel::Type::ASTC_SRGBA8_6x6:
			case ScPixel::Type::ASTC_SRGBA8_8x5:
			case ScPixel::Type::ASTC_SRGBA8_8x6:
			case ScPixel::Type::ASTC_SRGBA8_8x8:
			case ScPixel::Type::ASTC_SRGBA8_10x5:
			case ScPixel::Type::ASTC_SRGBA8_10x6:
			case ScPixel::Type::ASTC_SRGBA8_10x8:
			case ScPixel::Type::ASTC_SRGBA8_10x10:
			case ScPixel::Type::ASTC_SRGBA8_12x10:
			case ScPixel::Type::ASTC_SRGBA8_12x12:
				return wk::Image::ColorSpace::sRGB;

			default:
				throw wk::Exception("Unsupported pixel type!");
			}
		}

		static wk::Image::BasePixelType get_base_pixel_type(ScPixel::Type type)
		{
			switch (type)
			{
			case ScPixel::Type::ETC1_RGB8:
			case ScPixel::Type::ETC2_RGB8:
				return wk::Image::BasePixelType::RGB;

			case ScPixel::Type::ETC2_EAC_RGBA8:
			case ScPixel::Type::ETC2_EAC_SRGBA8:
			case ScPixel::Type::ETC2_SRGB8:
			case ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
			case ScPixel::Type::ASTC_SRGBA8_4x4:
			case ScPixel::Type::ASTC_SRGBA8_5x4:
			case ScPixel::Type::ASTC_SRGBA8_5x5:
			case ScPixel::Type::ASTC_SRGBA8_6x5:
			case ScPixel::Type::ASTC_SRGBA8_6x6:
			case ScPixel::Type::ASTC_SRGBA8_8x5:
			case ScPixel::Type::ASTC_SRGBA8_8x6:
			case ScPixel::Type::ASTC_SRGBA8_8x8:
			case ScPixel::Type::ASTC_SRGBA8_10x5:
			case ScPixel::Type::ASTC_SRGBA8_10x6:
			case ScPixel::Type::ASTC_SRGBA8_10x8:
			case ScPixel::Type::ASTC_SRGBA8_10x10:
			case ScPixel::Type::ASTC_SRGBA8_12x10:
			case ScPixel::Type::ASTC_SRGBA8_12x12:
			case ScPixel::Type::ASTC_RGBA8_4x4:
			case ScPixel::Type::ASTC_RGBA8_5x4:
			case ScPixel::Type::ASTC_RGBA8_5x5:
			case ScPixel::Type::ASTC_RGBA8_6x5:
			case ScPixel::Type::ASTC_RGBA8_6x6:
			case ScPixel::Type::ASTC_RGBA8_8x5:
			case ScPixel::Type::ASTC_RGBA8_8x6:
			case ScPixel::Type::ASTC_RGBA8_8x8:
			case ScPixel::Type::ASTC_RGBA8_10x5:
			case ScPixel::Type::ASTC_RGBA8_10x6:
			case ScPixel::Type::ASTC_RGBA8_10x8:
			case ScPixel::Type::ASTC_RGBA8_10x10:
			case ScPixel::Type::ASTC_RGBA8_12x10:
			case ScPixel::Type::ASTC_RGBA8_12x12:
				return wk::Image::BasePixelType::RGBA;

			default:
				throw wk::Exception("Unsupported pixel type!");
			}
		}

		static std::tuple<uint8_t, uint8_t, uint8_t> get_astc_blocks(ScPixel::Type type)
		{
			uint8_t x, y, z;

			switch (type)
				{
				case ScPixel::Type::ASTC_SRGBA8_4x4:
				case ScPixel::Type::ASTC_RGBA8_4x4:
					x = 4; y = 4; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_5x5:
				case ScPixel::Type::ASTC_RGBA8_5x5:
					x = 5; y = 5; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_5x4:
				case ScPixel::Type::ASTC_RGBA8_5x4:
					x = 5; y = 4; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_6x5:
				case ScPixel::Type::ASTC_RGBA8_6x5:
					x = 6; y = 5; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_6x6:
				case ScPixel::Type::ASTC_RGBA8_6x6:
					x = 6; y = 6; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_8x5:
				case ScPixel::Type::ASTC_RGBA8_8x5:
					x = 8; y = 5; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_8x6:
				case ScPixel::Type::ASTC_RGBA8_8x6:
					x = 8; y = 6; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_8x8:
				case ScPixel::Type::ASTC_RGBA8_8x8:
					x = 8; y = 8; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_10x5:
				case ScPixel::Type::ASTC_RGBA8_10x5:
					x = 10; y = 5; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_10x6:
				case ScPixel::Type::ASTC_RGBA8_10x6:
					x = 0; y = 0; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_10x8:
				case ScPixel::Type::ASTC_RGBA8_10x8:
					x = 10; y = 8; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_10x10:
				case ScPixel::Type::ASTC_RGBA8_10x10:
					x = 10; y = 10; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_12x10:
				case ScPixel::Type::ASTC_RGBA8_12x10:
					x = 12; y = 10; z = 0;
					break;
				case ScPixel::Type::ASTC_SRGBA8_12x12:
				case ScPixel::Type::ASTC_RGBA8_12x12:
					x = 12; y = 12; z = 0;
					break;
				default:
					x = 0; y = 0; z = 0;
					break;
				}

			return std::make_tuple(x, y, z);
		}
	};
}
#pragma once

#include "core/generic/static.h"
#include "core/image/image.h"
#include "texture/backend/Khronos/KhronosTexture.h"
#include "core/exception/exception.h"

#include <unordered_map>

namespace sc::texture
{
	class ScPixel
	{
	public:
		WK_STATIC_CLASS(ScPixel);

	public:
		enum class Type
		{
			UNKNOWN = 0,

#pragma region A8
			A8Unorm = 1,
#pragma endregion

#pragma region R8
			R8Unorm = 10,
			R8Unorm_sRGB,
			R8Snorm,
			R8Uint,
			R8Sint,
#pragma endregion

#pragma region R16
			R16Unorm = 20,
			R16Snorm = 22,
			R16Uint,
			R16Sint,
			R16Float,
#pragma endregion

#pragma region RG8
			RG8Unorm = 30,
			RG8Unorm_sRGB,
			RG8Snorm,
			RG8Uint,
			RG8Sint,
#pragma endregion

#pragma region Packed types
			R5G6B5Unorm = 40,
			A1BGR5Unorm,
			ABGR4Unorm,
			BGR5A1Unorm,

			RGB10A2Unorm = 90,
			RGB10A2Uint, 
			RG11B10UFloat,
			BGR10A2Unorm = 94,
#pragma endregion

#pragma region R32
			R32Uint = 53,
			R32Sint,
			R32Float,
#pragma endregion

#pragma region RG16
			RG16Unorm = 60,
			RG16Snorm = 62,
			RG16Uint,
			RG16Sint,
			RG16Float,
#pragma endregion

#pragma region RGBA8
			RGBA8Unorm = 70,
			RGBA8Unorm_sRGB,
			RGBA8Snorm,
			RGBA8Uint,
			RGBA8Sint,
#pragma endregion

#pragma region BGRA8
			BGRA8Unorm = 80,
			BGRA8Unorm_sRGB,
#pragma endregion

#pragma region RG32
			RG32Uint = 103,
			RG32Sint,
			RG32Float,
#pragma endregion

#pragma region RGBA16
			RGBA16Unorm = 110,
			RGBA16Snorm = 112,
			RGBA16Uint,
			RGBA16Sint,
			RGBA16Float,
#pragma endregion

#pragma region RGBA32
			RGBA32Uint = 123,
			RGBA32Sint,
			RGBA32Float,
#pragma endregion

#pragma region PVRTC
			PVRTC_RGB_2BPP = 160,
			PVRTC_RGB_2BPP_sRGB,
			PVRTC_RGB_4BPP,
			PVRTC_RGB_4BPP_sRGB,
			PVRTC_RGBA_2BPP,
			PVRTC_RGBA_2BPP_sRGB,
			PVRTC_RGBA_4BPP,
			PVRTC_RGBA_4BPP_sRGB,
#pragma endregion

#pragma region ETC
			EAC_R11 = 170,
			EAC_SIGNED_R11 = 172,
			EAC_RG11 = 174,
			EAC_SIGNED_RG11 = 176,
			ETC2_EAC_RGBA8 = 178,
			ETC2_EAC_SRGBA8,
			ETC2_RGB8,
			ETC2_SRGB8,
			ETC2_RGB8_PUNCHTHROUGH_ALPHA1,
			ETC2_SRGB8_PUNCHTHROUGH_ALPHA1,
#pragma endregion

#pragma region Uniform SRGB ASTC Compression
			ASTC_SRGBA8_4x4 = 186,
			ASTC_SRGBA8_5x4,
			ASTC_SRGBA8_5x5,
			ASTC_SRGBA8_6x5,
			ASTC_SRGBA8_6x6,
#pragma endregion

#pragma region SRGB ASTC Compression
			ASTC_SRGBA8_8x5 = 192,
			ASTC_SRGBA8_8x6,
			ASTC_SRGBA8_8x8,
			ASTC_SRGBA8_10x5,
			ASTC_SRGBA8_10x6,
			ASTC_SRGBA8_10x8,
			ASTC_SRGBA8_10x10,
			ASTC_SRGBA8_12x10,
			ASTC_SRGBA8_12x12,
#pragma endregion

#pragma region Uniform ASTC Compression
			ASTC_RGBA8_4x4 = 204,
			ASTC_RGBA8_5x4,
			ASTC_RGBA8_5x5,
			ASTC_RGBA8_6x5,
			ASTC_RGBA8_6x6,
#pragma endregion

#pragma region ASTC Compression
			ASTC_RGBA8_8x5 = 210,
			ASTC_RGBA8_8x6,
			ASTC_RGBA8_8x8,
			ASTC_RGBA8_10x5,
			ASTC_RGBA8_10x6,
			ASTC_RGBA8_10x8,
			ASTC_RGBA8_10x10,
			ASTC_RGBA8_12x10,
			ASTC_RGBA8_12x12,
#pragma endregion

#pragma region Render Buffers
			Depth16Unorm = 250,
			Depth32Float,
			Stencil8,
			Depth16Unorm_Stencil8,
			Depth24Unorm_Stencil8,
			Depth32Float_Stencil8,
#pragma endregion

#pragma region ETC1
			ETC1_RGB8 = 263,
#pragma endregion

#pragma region Luminance
			Luminance = 264,
			LuminanceAlpha,
#pragma endregion

#pragma region RGB
			RGB8Unorm = 266,
			RGB8Unorm_sRGB,
#pragma endregion
		};

		enum class Compression
		{
			UNKNOWN,
			PVRTC,
			EAC,
			ETC1,
			ETC2,
			ASTC
		};

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

			case ScPixel::Type::PVRTC_RGB_2BPP:
			case ScPixel::Type::PVRTC_RGB_2BPP_sRGB:
			case ScPixel::Type::PVRTC_RGB_4BPP:
			case ScPixel::Type::PVRTC_RGB_4BPP_sRGB:
			case ScPixel::Type::PVRTC_RGBA_2BPP:
			case ScPixel::Type::PVRTC_RGBA_2BPP_sRGB:
			case ScPixel::Type::PVRTC_RGBA_4BPP:
			case ScPixel::Type::PVRTC_RGBA_4BPP_sRGB:
				return Compression::PVRTC;

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
				return wk::Image::PixelDepth::RGBA8;

			case ScPixel::Type::RGBA8Unorm:
			case ScPixel::Type::RGBA8Unorm_sRGB:
			case ScPixel::Type::RGBA8Snorm:
			//case ScPixel::Type::RGBA8Uint:
			//case ScPixel::Type::RGBA8Sint:
				return wk::Image::PixelDepth::RGBA8;

			default:
				throw wk::Exception("Unsupported pixel type!");
			}
		}

		static wk::Image::ColorSpace get_colorspace(ScPixel::Type type)
		{
			switch (type)
			{
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
			case ScPixel::Type::R8Unorm_sRGB:
			case ScPixel::Type::RG8Unorm_sRGB:
			case ScPixel::Type::RGBA8Unorm_sRGB:
			case ScPixel::Type::BGRA8Unorm_sRGB:
			case ScPixel::Type::PVRTC_RGB_2BPP_sRGB:
			case ScPixel::Type::PVRTC_RGB_4BPP_sRGB:
			case ScPixel::Type::PVRTC_RGBA_2BPP_sRGB:
			case ScPixel::Type::PVRTC_RGBA_4BPP_sRGB:
			case ScPixel::Type::RGB8Unorm_sRGB:
				return wk::Image::ColorSpace::sRGB;

			default:
				return wk::Image::ColorSpace::Linear;
			}
		}

		static wk::Image::BasePixelType get_base_pixel_type(ScPixel::Type type)
		{
			switch (type)
			{
			case ScPixel::Type::Luminance:
				return wk::Image::BasePixelType::L;

			case ScPixel::Type::LuminanceAlpha:
				return wk::Image::BasePixelType::LA;

			case ScPixel::Type::ETC1_RGB8:
			case ScPixel::Type::RG11B10UFloat:
			case ScPixel::Type::PVRTC_RGB_2BPP:
			case ScPixel::Type::PVRTC_RGB_2BPP_sRGB:
			case ScPixel::Type::PVRTC_RGB_4BPP:
			case ScPixel::Type::PVRTC_RGB_4BPP_sRGB:
			case ScPixel::Type::RGB8Unorm:
			case ScPixel::Type::RGB8Unorm_sRGB:
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
			case ScPixel::Type::A1BGR5Unorm:
			case ScPixel::Type::ABGR4Unorm:
			case ScPixel::Type::RGB10A2Unorm:
			case ScPixel::Type::RGB10A2Uint:
			case ScPixel::Type::BGR10A2Unorm:
			case ScPixel::Type::RGBA8Unorm:
			case ScPixel::Type::RGBA8Unorm_sRGB:
			case ScPixel::Type::RGBA8Snorm:
			case ScPixel::Type::RGBA8Uint:
			case ScPixel::Type::RGBA8Sint:
			case ScPixel::Type::BGRA8Unorm:
			case ScPixel::Type::BGRA8Unorm_sRGB:
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

		static std::string to_string(const ScPixel::Type type)
		{
			return ScPixel::string_map.at(type);
		}

		static ScPixel::Type from_string(std::string type)
		{
			auto result = std::find_if(ScPixel::string_map.begin(), ScPixel::string_map.end(), [&type](const std::pair<ScPixel::Type, std::string>& pair)
				{
					return pair.second == type;
				});

			if (result == ScPixel::string_map.end())
			{
				return ScPixel::Type::UNKNOWN;
			}
			else
			{
				return result->first;
			}
		}

		static ScPixel::Type from_gl_format(KhronosTexture::glInternalFormat type)
		{
			switch (type)
			{
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4:
				return ScPixel::Type::ASTC_RGBA8_4x4;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x4:
				return ScPixel::Type::ASTC_RGBA8_5x4;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_5x5:
				return ScPixel::Type::ASTC_RGBA8_5x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x5:
				return ScPixel::Type::ASTC_RGBA8_6x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6:
				return ScPixel::Type::ASTC_RGBA8_6x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x5:
				return ScPixel::Type::ASTC_RGBA8_8x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x6:
				return ScPixel::Type::ASTC_RGBA8_8x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_8x8:
				return ScPixel::Type::ASTC_RGBA8_8x8;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x5:
				return ScPixel::Type::ASTC_RGBA8_10x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x6:
				return ScPixel::Type::ASTC_RGBA8_10x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x8:
				return ScPixel::Type::ASTC_RGBA8_10x8;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_10x10:
				return ScPixel::Type::ASTC_RGBA8_10x10;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x10:
				return ScPixel::Type::ASTC_RGBA8_12x10;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_12x12:
				return ScPixel::Type::ASTC_RGBA8_12x12;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4:
				return ScPixel::Type::ASTC_SRGBA8_4x4;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4:
				return ScPixel::Type::ASTC_SRGBA8_5x4;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5:
				return ScPixel::Type::ASTC_SRGBA8_5x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5:
				return ScPixel::Type::ASTC_SRGBA8_6x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6:
				return ScPixel::Type::ASTC_SRGBA8_6x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5:
				return ScPixel::Type::ASTC_SRGBA8_8x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6:
				return ScPixel::Type::ASTC_SRGBA8_8x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8:
				return ScPixel::Type::ASTC_SRGBA8_8x8;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5:
				return ScPixel::Type::ASTC_SRGBA8_10x5;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6:
				return ScPixel::Type::ASTC_SRGBA8_10x6;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8:
				return ScPixel::Type::ASTC_SRGBA8_10x8;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10:
				return ScPixel::Type::ASTC_SRGBA8_10x10;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10:
				return ScPixel::Type::ASTC_SRGBA8_12x10;
			case sc::texture::KhronosTexture::glInternalFormat::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12:
				return ScPixel::Type::ASTC_SRGBA8_12x12;
			default:
				throw wk::Exception("Unsupported glInternalFormat");
				break;
			}
		}

	private:
			static const inline std::unordered_map<ScPixel::Type, std::string> string_map = {
				{ScPixel::Type::UNKNOWN, "Unknown"},

				{ScPixel::Type::A8Unorm, "A8Unorm"},
				{ScPixel::Type::R8Unorm, "R8Unorm"},
				{ScPixel::Type::R8Unorm_sRGB, "R8Unorm_sRGB"},
				{ScPixel::Type::R8Snorm, "R8Snorm"},
				{ScPixel::Type::R8Uint, "R8Uint"},
				{ScPixel::Type::R8Sint, "R8Sint"},
				{ScPixel::Type::R16Unorm, "R16Unorm"},
				{ScPixel::Type::R16Snorm, "R16Snorm"},
				{ScPixel::Type::R16Uint, "R16Uint"},
				{ScPixel::Type::R16Sint, "R16Sint"},
				{ScPixel::Type::R16Float, "R16Float"},
				{ScPixel::Type::RG8Unorm, "RG8Unorm"},
				{ScPixel::Type::RG8Unorm_sRGB, "RG8Unorm_sRGB"},
				{ScPixel::Type::RG8Snorm, "RG8Snorm"},
				{ScPixel::Type::RG8Uint, "RG8Uint"},
				{ScPixel::Type::RG8Sint, "RG8Sint"},
				{ScPixel::Type::R5G6B5Unorm, "R5G6B5Unorm"},
				{ScPixel::Type::A1BGR5Unorm, "A1BGR5Unorm"},
				{ScPixel::Type::ABGR4Unorm, "ABGR4Unorm"},
				{ScPixel::Type::R32Uint, "R32Uint"},
				{ScPixel::Type::R32Sint, "R32Sint"},
				{ScPixel::Type::R32Float, "R32Float"},
				{ScPixel::Type::RG16Unorm, "RG16Unorm"},
				{ScPixel::Type::RG16Snorm, "RG16Snorm"},
				{ScPixel::Type::RG16Uint, "RG16Uint"},
				{ScPixel::Type::RG16Sint, "RG16Sint"},
				{ScPixel::Type::RG16Float, "RG16Float"},
				{ScPixel::Type::RGBA8Unorm, "RGBA8Unorm"},
				{ScPixel::Type::RGBA8Unorm_sRGB, "RGBA8Unorm_sRGB"},
				{ScPixel::Type::RGBA8Snorm, "RGBA8Snorm"},
				{ScPixel::Type::RGBA8Uint, "RGBA8Uint"},
				{ScPixel::Type::RGBA8Sint, "RGBA8Sint"},
				{ScPixel::Type::BGRA8Unorm, "BGRA8Unorm"},
				{ScPixel::Type::RGB10A2Unorm, "RGB10A2Unorm"},
				{ScPixel::Type::RGB10A2Uint, "RGB10A2Uint"},
				{ScPixel::Type::RG11B10UFloat, "RG11B10UFloat"},
				{ScPixel::Type::BGR10A2Unorm, "BGR10A2Unorm"},
				{ScPixel::Type::RG32Uint, "RG32Uint"},
				{ScPixel::Type::RG32Sint, "RG32Sint"},
				{ScPixel::Type::RG32Float, "RG32Float"},
				{ScPixel::Type::RGBA16Unorm, "RGBA16Unorm"},
				{ScPixel::Type::RGBA16Snorm, "RGBA16Snorm"},
				{ScPixel::Type::RGBA16Uint, "RGBA16Uint"},
				{ScPixel::Type::RGBA16Sint, "RGBA16Sint"},
				{ScPixel::Type::RGBA16Float, "RGBA16Float"},
				{ScPixel::Type::RGBA32Uint, "RGBA32Uint"},
				{ScPixel::Type::RGBA32Sint, "RGBA32Sint"},
				{ScPixel::Type::RGBA32Float, "RGBA32Float"},
				{ScPixel::Type::PVRTC_RGB_2BPP, "PVRTC_RGB_2BPP"},
				{ScPixel::Type::PVRTC_RGB_2BPP_sRGB, "PVRTC_RGB_2BPP_sRGB"},
				{ScPixel::Type::PVRTC_RGB_4BPP, "PVRTC_RGB_4BPP"},
				{ScPixel::Type::PVRTC_RGB_4BPP_sRGB, "PVRTC_RGB_4BPP_sRGB"},
				{ScPixel::Type::PVRTC_RGBA_2BPP, "PVRTC_RGBA_2BPP"},
				{ScPixel::Type::PVRTC_RGBA_2BPP_sRGB, "PVRTC_RGBA_2BPP_sRGB"},
				{ScPixel::Type::PVRTC_RGBA_4BPP, "PVRTC_RGBA_4BPP"},
				{ScPixel::Type::PVRTC_RGBA_4BPP_sRGB, "PVRTC_RGBA_4BPP_sRGB"},
				{ScPixel::Type::Depth16Unorm, "Depth16Unorm"},
				{ScPixel::Type::Stencil8, "Stencil8"},
				{ScPixel::Type::Depth16Unorm_Stencil8, "Depth16Unorm_Stencil8"},
				{ScPixel::Type::Depth24Unorm_Stencil8, "Depth24Unorm_Stencil8"},
				{ScPixel::Type::Depth32Float_Stencil8, "Depth32Float_Stencil8"},
				{ScPixel::Type::Luminance, "Luminance"},
				{ScPixel::Type::LuminanceAlpha, "LuminanceAlpha"},
				{ScPixel::Type::RGB8Unorm, "RGB8Unorm"},
				{ScPixel::Type::RGB8Unorm_sRGB, "RGB8Unorm_sRGB"},

				{ScPixel::Type::EAC_R11, "EAC_R11"},
				{ScPixel::Type::EAC_SIGNED_R11, "EAC_SIGNED_R11"},
				{ScPixel::Type::EAC_RG11, "EAC_RG11"},
				{ScPixel::Type::ETC2_EAC_RGBA8, "ETC2_EAC_RGBA8"},
				{ScPixel::Type::ETC2_EAC_SRGBA8, "ETC2_EAC_SRGBA8"},
				{ScPixel::Type::ETC2_RGB8, "ETC2_RGB8"},
				{ScPixel::Type::ETC2_SRGB8, "ETC2_SRGB8"},
				{ScPixel::Type::ETC2_RGB8_PUNCHTHROUGH_ALPHA1, "ETC2_RGB8_PUNCHTHROUGH_ALPHA1"},
				{ScPixel::Type::ETC2_SRGB8_PUNCHTHROUGH_ALPHA1, "ETC2_SRGB8_PUNCHTHROUGH_ALPHA1"},

				{ScPixel::Type::ASTC_SRGBA8_4x4, "ASTC_SRGBA8_4x4"},
				{ScPixel::Type::ASTC_SRGBA8_5x4, "ASTC_SRGBA8_5x4"},
				{ScPixel::Type::ASTC_SRGBA8_5x5, "ASTC_SRGBA8_5x5"},
				{ScPixel::Type::ASTC_SRGBA8_6x5, "ASTC_SRGBA8_6x5"},
				{ScPixel::Type::ASTC_SRGBA8_6x6, "ASTC_SRGBA8_6x6"},
				{ScPixel::Type::ASTC_SRGBA8_8x5, "ASTC_SRGBA8_8x5"},
				{ScPixel::Type::ASTC_SRGBA8_8x6, "ASTC_SRGBA8_8x6"},
				{ScPixel::Type::ASTC_SRGBA8_8x8, "ASTC_SRGBA8_8x8"},
				{ScPixel::Type::ASTC_SRGBA8_10x5, "ASTC_SRGBA8_10x5"},
				{ScPixel::Type::ASTC_SRGBA8_10x6, "ASTC_SRGBA8_10x6"},
				{ScPixel::Type::ASTC_SRGBA8_10x8, "ASTC_SRGBA8_10x8"},
				{ScPixel::Type::ASTC_SRGBA8_10x10, "ASTC_SRGBA8_10x10"},
				{ScPixel::Type::ASTC_SRGBA8_12x10, "ASTC_SRGBA8_12x10"},
				{ScPixel::Type::ASTC_SRGBA8_12x12, "ASTC_SRGBA8_12x12"},

				{ScPixel::Type::ASTC_RGBA8_4x4, "ASTC_RGBA8_4x4"},
				{ScPixel::Type::ASTC_RGBA8_5x4, "ASTC_RGBA8_5x4"},
				{ScPixel::Type::ASTC_RGBA8_5x5, "ASTC_RGBA8_5x5"},
				{ScPixel::Type::ASTC_RGBA8_6x5, "ASTC_RGBA8_6x5"},
				{ScPixel::Type::ASTC_RGBA8_6x6, "ASTC_RGBA8_6x6"},
				{ScPixel::Type::ASTC_RGBA8_8x5, "ASTC_RGBA8_8x5"},
				{ScPixel::Type::ASTC_RGBA8_8x6, "ASTC_RGBA8_8x6"},
				{ScPixel::Type::ASTC_RGBA8_8x8, "ASTC_RGBA8_8x8"},
				{ScPixel::Type::ASTC_RGBA8_10x5, "ASTC_RGBA8_10x5"},
				{ScPixel::Type::ASTC_RGBA8_10x6, "ASTC_RGBA8_10x6"},
				{ScPixel::Type::ASTC_RGBA8_10x8, "ASTC_RGBA8_10x8"},
				{ScPixel::Type::ASTC_RGBA8_10x10, "ASTC_RGBA8_10x10"},
				{ScPixel::Type::ASTC_RGBA8_12x10, "ASTC_RGBA8_12x10"},
				{ScPixel::Type::ASTC_RGBA8_12x12, "ASTC_RGBA8_12x12"}
			};
	};
}
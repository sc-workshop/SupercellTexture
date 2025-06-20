#pragma once

#include "KhronosTexture.h"

namespace sc
{
	namespace texture
	{
		// TODO: mip maps ?
		// TODO: ETC compression
		class WORKSHOP_API KhronosTexture2 : public KhronosTexture
		{
		public:
			static const uint8_t FileIdentifier[12];

		public:
			/// <summary>
			/// Reads ktx1 file from stream
			/// </summary>
			/// <param name="buffer"></param>
			KhronosTexture2(wk::Stream& buffer);

		public:
			virtual Version version() const;
			virtual KhronosTextureCompression compression_type() const;

		public:
			virtual BasePixelType base_type() const { return KhronosTexture::format_type(m_format); };
			virtual ColorSpace colorspace() const { return KhronosTexture::format_colorspace(m_format); };
			virtual PixelDepth depth() const { return KhronosTexture::format_depth(m_format); };

		public:
			virtual void write(wk::Stream& buffer);

		public:
			virtual glInternalFormat internal_format() const;
			virtual glFormat format() const;
			virtual glType type() const;

		private:
			/// <summary>
			/// Reads KTX header
			/// </summary>
			/// <param name="buffer"></param>
			/// <returns> Image levels count </returns>
			uint32_t read_header(wk::Stream& buffer);

		private:
			VkFormat m_format = VkFormat::Unknown;
			uint32_t m_type_size = 0;
			uint32_t m_depth = 0;
		};
	}
}
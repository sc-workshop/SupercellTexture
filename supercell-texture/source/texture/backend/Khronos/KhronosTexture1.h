#pragma once

#include "KhronosTexture.h"

namespace sc
{
	namespace texture
	{
		// TODO: mip maps ?
		// TODO: ETC compression
		class WORKSHOP_API KhronosTexture1 : public KhronosTexture
		{
		public:
			static const uint8_t FileIdentifier[12];

		public:
			/// <summary>
			/// Reads ktx1 file from stream
			/// </summary>
			/// <param name="buffer"></param>
			KhronosTexture1(wk::Stream& buffer);

			/// <summary>
			/// Initializes a object with specified format from provided buffer. Buffer is accepted as is and will not be compressed.
			/// </summary>
			/// <param name="format"></param>
			/// <param name="buffer"></param>
			/// <param name="buffer_size"></param>
			KhronosTexture1(glInternalFormat format, uint16_t width, uint16_t height, uint8_t* buffer, size_t buffer_size);

			/// <summary>
			/// Initializes an object from provided Raw Image and compresses it if necessary.
			/// </summary>
			/// <param name="image"></param>
			/// <param name="format"></param>
			KhronosTexture1(wk::RawImage& image, glInternalFormat format);

			~KhronosTexture1();

		public:
			virtual KhronosTextureCompression compression_type();
			virtual uint32_t level_count() const;

		public:
			virtual BasePixelType base_type() const;

			virtual ColorSpace colorspace() const;

			virtual PixelDepth depth() const;

			virtual size_t data_length() const;
			virtual size_t data_length(uint32_t level_index) const;

			virtual uint8_t* data() const;
			virtual const wk::BufferStream* data(uint32_t level_index) const;

			virtual bool is_compressed() const;

			virtual size_t decompressed_data_length();
			size_t decompressed_data_length(uint32_t level_index);

		public:
			virtual void write(wk::Stream& buffer);
			virtual void decompress_data(wk::Stream& output);

		public:
			virtual void decompress_data(wk::Stream& output, uint32_t level_index);
			virtual void set_level_data(wk::Stream& data, Image::PixelDepth data_format, uint32_t level_index);
			virtual void reset_level_data(uint32_t level_index);

		public:
			virtual glInternalFormat internal_format() const
			{
				return m_internal_format;
			}

			virtual glFormat format() const
			{
				return m_format;
			}

			virtual glType type() const
			{
				return m_type;
			}

		private:
			/// <summary>
			/// Reads KTX header
			/// </summary>
			/// <param name="buffer"></param>
			/// <returns> Image levels count </returns>
			uint32_t read_header(wk::Stream& buffer);

		private:
			glType m_type;
			glFormat m_format;
			glInternalFormat m_internal_format;

			std::vector<wk::BufferStream*> m_levels;
		};
	}
}
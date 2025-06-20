#include "KhronosTexture1.h"

#include "core/stb/stb.h"
#include "core/io/file_stream.h"

using namespace wk;

namespace sc
{
	namespace texture {
		const uint8_t KhronosTexture1::FileIdentifier[12] = {
			0xAB, 'K', 'T', 'X', ' ', '1', '1', 0xBB, '\r', '\n', 0x1A, '\n'
		};

#pragma region Constructors
		KhronosTexture1::KhronosTexture1(Stream& buffer)
		{
			uint32_t levels_count = read_header(buffer);

			m_levels.resize(levels_count);
			for (uint32_t level_index = 0; levels_count > level_index; level_index++)
			{
				uint32_t level_length = buffer.read_unsigned_int();

				m_levels[level_index] = CreateRef<BufferStream>(level_length);
				buffer.read(m_levels[level_index]->data(), level_length);
			}
		}

		KhronosTexture1::KhronosTexture1(glInternalFormat format, uint16_t width, uint16_t height, uint8_t* buffer, size_t buffer_size) : m_internal_format(format)
		{
			m_width = width;
			m_height = height;
			m_format = KhronosTexture::type(format);

			if (!is_compressed())
			{
				m_type = glType::GL_UNSIGNED_BYTE;
			}
			else
			{
				m_type = glType::COMPRESSED;
			}

			auto stream = CreateRef<BufferStream>(buffer_size);
			Memory::copy(buffer, stream->data(), buffer_size);

			m_levels.push_back(stream);
		}

		KhronosTexture1::KhronosTexture1(RawImage& image, glInternalFormat format) : m_internal_format(format)
		{
			m_format = KhronosTexture::type(format);
			m_width = image.width();
			m_height = image.height();

			if (is_compressed())
			{
				m_type = glType::COMPRESSED;
			}
			else
			{
				m_type = glType::GL_UNSIGNED_BYTE;
			}

			SharedMemoryStream image_data(
				image.data(), image.data_length()
			);

			set_level_data(
				image_data,
				image.depth(),
				0
			);
		}

#pragma endregion

#pragma region Functions
		void KhronosTexture1::write(Stream& buffer)
		{
			bool is_compressed = m_type == glType::COMPRESSED;

			buffer.write(&KhronosTexture1::FileIdentifier, sizeof(KhronosTexture1::FileIdentifier));

			// endianess
			buffer.write_unsigned_int(0x04030201);

			// glType
			buffer.write_unsigned_int((uint32_t)m_type);

			// glTypeSize
			{
				uint32_t glTypeSize = 0;
				if (is_compressed)
				{
					glTypeSize = 1;
				}

				buffer.write_unsigned_int(glTypeSize);
			}

			// glFormat
			buffer.write_unsigned_int(is_compressed ? 0 : (uint32_t)m_format);

			// glInternalFormat
			buffer.write_unsigned_int((uint32_t)m_internal_format);

			// glBaseInternalType
			buffer.write_unsigned_int((uint32_t)m_format);

			// Width / height
			buffer.write_unsigned_int(m_width);
			buffer.write_unsigned_int(m_height);

			// pixelDepth
			buffer.write_unsigned_int(0);

			// numberOfArrayElements
			buffer.write_unsigned_int(0);

			// numberOfFaces
			buffer.write_unsigned_int(1);

			// numberOfMipmapLevels
			buffer.write_unsigned_int(static_cast<uint32_t>(m_levels.size()));

			// bytesOfKeyValueData
			buffer.write_unsigned_int(0);

			for (auto& level : m_levels)
			{
				uint32_t image_length = static_cast<uint32_t>(level->length());
				buffer.write_unsigned_int(image_length);
				buffer.write(level->data(), image_length);
			}
		}

#pragma endregion

#pragma region Getters/Setters
		Image::BasePixelType KhronosTexture1::base_type() const
		{
			return KhronosTexture1::format_type(m_format);
		}

		Image::ColorSpace KhronosTexture1::colorspace() const
		{
			return KhronosTexture1::format_colorspace(m_format);
		}

		Image::PixelDepth KhronosTexture1::depth() const
		{
			return KhronosTexture1::format_depth(m_internal_format);
		};

		KhronosTexture::Version KhronosTexture1::version() const
		{
			return Version::v1;
		}

		KhronosTexture::KhronosTextureCompression KhronosTexture1::compression_type() const
		{
			return KhronosTexture1::format_compression_type(m_internal_format);
		}

#pragma endregion

#pragma region Private Functions
		uint32_t KhronosTexture1::read_header(Stream& buffer)
		{
			for (uint8_t i = 0; sizeof(KhronosTexture1::FileIdentifier) > i; i++)
			{
				if (buffer.read_unsigned_byte() != KhronosTexture1::FileIdentifier[i])
				{
					throw Exception("Buffer is not KTX file or it is corrupted");
				}
			}

			// endianess
			buffer.read_unsigned_int();

			m_type = (glType)buffer.read_unsigned_int();

			// glTypeSize
			buffer.read_unsigned_int();

			glFormat format = (glFormat)buffer.read_unsigned_int();

			m_internal_format = (glInternalFormat)buffer.read_unsigned_int();
			glFormat internalBasetype = (glFormat)buffer.read_unsigned_int();

			if (format == glFormat::UNKNOWN)
			{
				m_format = internalBasetype;
			}
			else
			{
				m_format = format;
			}

			m_width = static_cast<uint16_t>(buffer.read_unsigned_int());
			m_height = static_cast<uint16_t>(buffer.read_unsigned_int());

			// pixelDepth | must be 0											//<-
			buffer.read_unsigned_int();											//
																				//
			// numberOfArrayElements | must be 0								// Some hardcoded/unsuported values
			buffer.read_unsigned_int();											//
																				//
			// numberOfFaces | must be 1										//
			buffer.read_unsigned_int();											//<-

			uint32_t levels_count = buffer.read_unsigned_int();

			uint32_t key_value_data_length = buffer.read_unsigned_int();

			// skip for now
			buffer.seek(key_value_data_length, Stream::SeekMode::Add);

			return levels_count;
		}
#pragma endregion
	}
}
#include "KhronosTexture1.h"

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

				m_levels[level_index] = new BufferStream(level_length);
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

			BufferStream* stream = new BufferStream(buffer_size);
			sc::Memory::copy(buffer, stream->data(), buffer_size);

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

			MemoryStream image_data(image.data(), image.data_length());

			set_level_data(
				image_data,
				image.depth(),
				0
			);
		}

		KhronosTexture1::~KhronosTexture1()
		{
			for (BufferStream* level : m_levels)
			{
				if (level != nullptr) delete level;
			}
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

			for (BufferStream* level : m_levels)
			{
				if (level == nullptr) continue;

				uint32_t image_length = static_cast<uint32_t>(level->length());
				buffer.write_unsigned_int(image_length);
				buffer.write(level->data(), image_length);
			}
		}

		void KhronosTexture1::decompress_data(Stream& output, uint32_t level_index)
		{
			if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;
			BufferStream* buffer = m_levels[level_index];
			if (buffer == nullptr) return;

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

		void KhronosTexture1::set_level_data(Stream& stream, Image::PixelDepth source_depth, uint32_t level_index)
		{
			// First, check if level index is ok
			// If index out of bound - create new buffer
			if (level_index >= m_levels.size())
			{
				level_index = static_cast<uint32_t>(m_levels.size());
				m_levels.resize(m_levels.size() + 1);
			};

			// Free level buffer before some changes if it exist
			if (m_levels[level_index] != nullptr)
			{
				delete m_levels[level_index];
			}

			// Level final data buffer
			BufferStream* buffer = new BufferStream();

			uint8_t* image_buffer = nullptr;
			size_t image_buffer_size = 0;

			// Second, we need to convert data base type to current texture type
			Image::PixelDepth destination_depth = depth();

			if (source_depth != destination_depth)
			{
				image_buffer_size = Image::calculate_image_length(m_width, m_height, destination_depth);
				image_buffer = sc::Memory::allocate(image_buffer_size);
				Image::remap(
					(uint8_t*)stream.data(), image_buffer,
					m_width, m_height,
					source_depth, destination_depth
				);
			}

			MemoryStream input_image(
				image_buffer ? image_buffer : (uint8_t*)stream.data(),
				image_buffer ? image_buffer_size : stream.length()
			);

			switch (compression_type())
			{
			case KhronosTextureCompression::ASTC:
				compress_astc(input_image, *buffer);
				break;

			default:
				buffer->resize(input_image.length());
				sc::Memory::copy(
					image_buffer ? image_buffer : (uint8_t*)stream.data(),
					buffer->data(),
					input_image.length()
				);
				break;
			}

			m_levels[level_index] = buffer;
		}

		void KhronosTexture1::reset_level_data(uint32_t level_index)
		{
			if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

			m_levels.erase(m_levels.begin(), m_levels.begin() + level_index);
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

		size_t KhronosTexture1::data_length() const
		{
			return data_length(0);
		}

		size_t KhronosTexture1::data_length(uint32_t level_index) const
		{
			if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

			return m_levels[level_index]->length();
		}

		uint8_t* KhronosTexture1::data() const
		{
			auto buffer = data(0);

			if (buffer != nullptr) return (uint8_t*)buffer->data();

			return nullptr;
		}

		const BufferStream* KhronosTexture1::data(uint32_t level_index) const
		{
			if (level_index >= m_levels.size()) level_index = static_cast<uint32_t>(m_levels.size()) - 1;

			return m_levels[level_index];
		}

		bool KhronosTexture1::is_compressed() const
		{
			return KhronosTexture1::format_compression(m_internal_format);
		}

		KhronosTexture::KhronosTextureCompression KhronosTexture1::compression_type()
		{
			return KhronosTexture1::format_compression_type(m_internal_format);
		}

		size_t KhronosTexture1::decompressed_data_length()
		{
			return decompressed_data_length(0);
		}

		size_t KhronosTexture1::decompressed_data_length(uint32_t level_index)
		{
			return Image::calculate_image_length(
				m_width / (uint16_t)(pow(2, level_index)),
				m_height / (uint16_t)(pow(2, level_index)),
				depth()
			);
		}

		void KhronosTexture1::decompress_data(Stream& output)
		{
			return decompress_data(output, 0);
		}

		uint32_t KhronosTexture1::level_count() const
		{
			return static_cast<uint32_t>(m_levels.size());
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
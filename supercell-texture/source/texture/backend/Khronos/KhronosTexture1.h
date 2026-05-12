#pragma once

#include "KhronosTexture.h"

namespace sc::texture {
    // TODO: mip maps ?
    // TODO: ETC compression
    class WORKSHOP_API KhronosTexture1 : public KhronosTexture {
    public:
        static const uint8_t FileIdentifier[12];

    public:
        /// @brief Reads ktx1 file from stream
        /// @param stream ktx1 file stream
        KhronosTexture1(wk::Stream& stream);

        /// @brief Initializes a object with specified format from provided buffer. Buffer is accepted as is and
        /// will not be compressed.
        /// @param format
        /// @param width
        /// @param height
        /// @param buffer
        /// @param buffer_size
        KhronosTexture1(glInternalFormat format, uint16_t width, uint16_t height, uint8_t* buffer, size_t buffer_size);

        /// @brief  Initializes a object with specified internal format, format and type from provided buffer.
        /// Buffer is accepted as is and will not be compressed.
        /// @param internal
        /// @param type
        /// @param format
        /// @param width
        /// @param height
        /// @param buffer
        /// @param buffer_size
        KhronosTexture1(glInternalFormat internal,
                        glType type,
                        glFormat format,
                        uint16_t width,
                        uint16_t height,
                        uint8_t* buffer,
                        size_t buffer_size);

        /// @brief  Initializes an object from provided Raw Image and compresses it if necessary.
        /// @param image
        /// @param format
        KhronosTexture1(wk::RawImage& image, glInternalFormat format);

    public:
        virtual Version version() const;
        virtual KhronosTextureCompression compression_type() const;

    public:
        virtual BasePixelType base_type() const;
        virtual ColorSpace colorspace() const;
        virtual PixelDepth depth() const;

    public:
        virtual void write(wk::Stream& buffer);

    public:
        virtual glInternalFormat internal_format() const { return m_internal_format; }

        virtual glFormat format() const { return m_format; }

        virtual glType type() const { return m_type; }

    private:
        /// @brief Reads KTX header
        /// @param buffer
        /// @return Image levels count
        uint32_t read_header(wk::Stream& buffer);

        void construct_buffer(uint8_t* buffer, size_t size);

    private:
        glType m_type;
        glFormat m_format;
        glInternalFormat m_internal_format;
    };
}
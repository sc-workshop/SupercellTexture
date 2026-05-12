#pragma once

#include "compression/backend/astc/astc.h"
#include "compression/compression.h"
#include "core/exception/exception.h"
#include "core/image/raw_image.h"
#include "core/io/buffer_stream.h"
#include "core/io/memory_stream.h"
#include "core/io/shared_memory_stream.h"
#include "core/memory/memory.h"
#include "core/memory/ref.h"
#include "core/preprocessor/api.h"
#include "glFormat.h"
#include "texture/interface/compressed_image.h"
#include "vkFormat.h"

namespace sc::texture {
    class WORKSHOP_API KhronosTexture : public CompressedImage {
        // Khronos shared definitions
    public:
        enum class Version {
            v1, // KTX 1.0
            v2  // KTX 2.0
        };

        enum class KhronosTextureCompression {
            None = 0,
            ASTC
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
        static Image::PixelDepth format_depth(vkFormat format);
        static Image::BasePixelType format_type(vkFormat format);
        static Image::ColorSpace format_colorspace(vkFormat format);

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

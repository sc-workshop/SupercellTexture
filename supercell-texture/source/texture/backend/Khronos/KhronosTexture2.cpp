#include "KhronosTexture2.h"

namespace sc::texture {
    const uint8_t KhronosTexture2::FileIdentifier[12] =
        {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

    KhronosTexture2::KhronosTexture2(wk::Stream& buffer) {
        uint32_t levels_count = read_header(buffer);

        m_levels.resize(levels_count);
        for (uint32_t level_index = 0; levels_count > level_index; level_index++) {
            uint64_t level_offset = buffer.read_unsigned_long();
            uint64_t level_length = buffer.read_unsigned_long();
            uint64_t level_uncompressed_length = buffer.read_unsigned_long();

            if (level_length != level_uncompressed_length) {
                throw wk::Exception("Current KTX2 implementation does not support data compressing");
            }

            size_t position = buffer.position();

            buffer.seek(level_offset);
            m_levels[level_index] = wk::CreateRef<wk::BufferStream>(level_length);
            buffer.read(m_levels[level_index]->data(), level_length);

            buffer.seek(position);
        }
    }

    KhronosTexture::Version KhronosTexture2::version() const {
        return Version::v2;
    }

    KhronosTexture::KhronosTextureCompression KhronosTexture2::compression_type() const {
        switch (m_format) {
            case vkFormat::R8_SRGB:
            case vkFormat::R8_UNORM:
            case vkFormat::R8G8_SRGB:
            case vkFormat::R8G8_UNORM:
            case vkFormat::R8G8B8_UNORM:
            case vkFormat::R8G8B8A8_UNORM:
            case vkFormat::R8G8B8_SRGB:
            case vkFormat::R8G8B8A8_SRGB:
                return KhronosTexture::KhronosTextureCompression::None;

            case vkFormat::ASTC_4x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x5_UNORM_BLOCK:
            case vkFormat::ASTC_8x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x5_UNORM_BLOCK:
            case vkFormat::ASTC_10x6_UNORM_BLOCK:
            case vkFormat::ASTC_10x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x12_UNORM_BLOCK:
            case vkFormat::ASTC_4x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x5_SRGB_BLOCK:
            case vkFormat::ASTC_8x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x5_SRGB_BLOCK:
            case vkFormat::ASTC_10x6_SRGB_BLOCK:
            case vkFormat::ASTC_10x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x12_SRGB_BLOCK:
                return KhronosTexture::KhronosTextureCompression::ASTC;

            default:
                return KhronosTexture::KhronosTextureCompression::None;
        }
    }

    void KhronosTexture2::write(wk::Stream&) {
        throw wk::Exception("KhronosTexture2::write is not implemented yet");
    }

    glInternalFormat KhronosTexture2::internal_format() const {
        switch (m_format) {
            case vkFormat::R8_UNORM:
            case vkFormat::R8_SRGB:
                return glInternalFormat::LUMINANCE8;

            case vkFormat::R8G8_UNORM:
            case vkFormat::R8G8_SRGB:
                return glInternalFormat::LUMINANCE8_ALPHA8;

            case vkFormat::R8G8B8_UNORM:
            case vkFormat::R8G8B8_SRGB:
                return glInternalFormat::RGB8;

            case vkFormat::R8G8B8A8_UNORM:
            case vkFormat::R8G8B8A8_SRGB:
                return glInternalFormat::RGBA8;

            case vkFormat::ASTC_4x4_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_4x4;

            case vkFormat::ASTC_4x4_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4;

            case vkFormat::ASTC_5x4_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_5x4;

            case vkFormat::ASTC_5x4_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4;

            case vkFormat::ASTC_5x5_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_5x5;

            case vkFormat::ASTC_5x5_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5;

            case vkFormat::ASTC_6x5_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_6x5;

            case vkFormat::ASTC_6x5_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5;

            case vkFormat::ASTC_6x6_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_6x6;

            case vkFormat::ASTC_6x6_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6;

            case vkFormat::ASTC_8x5_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_8x5;

            case vkFormat::ASTC_8x5_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5;

            case vkFormat::ASTC_8x6_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_8x6;

            case vkFormat::ASTC_8x6_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6;

            case vkFormat::ASTC_8x8_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_8x8;

            case vkFormat::ASTC_8x8_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8;

            case vkFormat::ASTC_10x5_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_10x5;

            case vkFormat::ASTC_10x5_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5;

            case vkFormat::ASTC_10x6_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_10x6;

            case vkFormat::ASTC_10x6_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6;

            case vkFormat::ASTC_10x8_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_10x8;

            case vkFormat::ASTC_10x8_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8;

            case vkFormat::ASTC_10x10_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_10x10;

            case vkFormat::ASTC_10x10_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10;

            case vkFormat::ASTC_12x10_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_12x10;

            case vkFormat::ASTC_12x10_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10;

            case vkFormat::ASTC_12x12_UNORM_BLOCK:
                return glInternalFormat::COMPRESSED_RGBA_ASTC_12x12;

            case vkFormat::ASTC_12x12_SRGB_BLOCK:
                return glInternalFormat::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12;

            default:
                return glInternalFormat::UNKNOWN;
        }
    }

    glFormat KhronosTexture2::format() const {
        switch (m_format) {
            case vkFormat::R8_SRGB:
            case vkFormat::R8_UNORM:
                return glFormat::RED;
            case vkFormat::R8G8_SRGB:
            case vkFormat::R8G8_UNORM:
                return glFormat::RG;
            case vkFormat::R8G8B8_UNORM:
                return glFormat::RGB;
            case vkFormat::R8G8B8A8_UNORM:
            case vkFormat::ASTC_4x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x5_UNORM_BLOCK:
            case vkFormat::ASTC_8x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x5_UNORM_BLOCK:
            case vkFormat::ASTC_10x6_UNORM_BLOCK:
            case vkFormat::ASTC_10x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x12_UNORM_BLOCK:
                return glFormat::RGBA;

            case vkFormat::R8G8B8_SRGB:
                return glFormat::SRGB;
            case vkFormat::R8G8B8A8_SRGB:
            case vkFormat::ASTC_4x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x5_SRGB_BLOCK:
            case vkFormat::ASTC_8x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x5_SRGB_BLOCK:
            case vkFormat::ASTC_10x6_SRGB_BLOCK:
            case vkFormat::ASTC_10x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x12_SRGB_BLOCK:
                return glFormat::SRGB_ALPHA;

            default:
                return glFormat::UNKNOWN;
        }
    }

    glType KhronosTexture2::type() const {
        switch (m_format) {
            case vkFormat::R8_SRGB:
            case vkFormat::R8_UNORM:
            case vkFormat::R8G8_SRGB:
            case vkFormat::R8G8_UNORM:
            case vkFormat::R8G8B8_UNORM:
            case vkFormat::R8G8B8A8_UNORM:
            case vkFormat::R8G8B8_SRGB:
            case vkFormat::R8G8B8A8_SRGB:
                return glType::UNSIGNED_BYTE;

            case vkFormat::ASTC_4x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x4_UNORM_BLOCK:
            case vkFormat::ASTC_5x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x5_UNORM_BLOCK:
            case vkFormat::ASTC_6x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x5_UNORM_BLOCK:
            case vkFormat::ASTC_8x6_UNORM_BLOCK:
            case vkFormat::ASTC_8x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x5_UNORM_BLOCK:
            case vkFormat::ASTC_10x6_UNORM_BLOCK:
            case vkFormat::ASTC_10x8_UNORM_BLOCK:
            case vkFormat::ASTC_10x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x10_UNORM_BLOCK:
            case vkFormat::ASTC_12x12_UNORM_BLOCK:
            case vkFormat::ASTC_4x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x4_SRGB_BLOCK:
            case vkFormat::ASTC_5x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x5_SRGB_BLOCK:
            case vkFormat::ASTC_6x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x5_SRGB_BLOCK:
            case vkFormat::ASTC_8x6_SRGB_BLOCK:
            case vkFormat::ASTC_8x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x5_SRGB_BLOCK:
            case vkFormat::ASTC_10x6_SRGB_BLOCK:
            case vkFormat::ASTC_10x8_SRGB_BLOCK:
            case vkFormat::ASTC_10x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x10_SRGB_BLOCK:
            case vkFormat::ASTC_12x12_SRGB_BLOCK:
                return glType::COMPRESSED;

            default:
                return glType::COMPRESSED;
        }
    }

    uint32_t KhronosTexture2::read_header(wk::Stream& stream) {
        for (uint8_t i = 0; sizeof(KhronosTexture2::FileIdentifier) > i; i++) {
            if (stream.read_unsigned_byte() != KhronosTexture2::FileIdentifier[i]) {
                throw wk::Exception("Buffer is not KTX2 file or it is corrupted");
            }
        }

        m_format = (vkFormat) stream.read_unsigned_int();
        m_type_size = stream.read_unsigned_int();
        m_width = (uint16_t) stream.read_unsigned_int();
        m_height = (uint16_t) stream.read_unsigned_int();
        m_depth = stream.read_unsigned_int();

        uint32_t layer_count = stream.read_unsigned_int();
        assert(layer_count == 0 && "Current KTX2 implementation does not support array texture");

        uint32_t face_count = stream.read_unsigned_int();
        assert(face_count == 1 && "Current KTX2 implementation does not support cube maps");

        uint32_t level_count = stream.read_unsigned_int();

        uint32_t supercompression_scheme = stream.read_unsigned_int();
        assert(supercompression_scheme == 0 && "Current KTX2 implementation does not supercompression");

        stream.read_unsigned_int(); // Data Format Descriptor offset
        stream.read_unsigned_int(); // Data Format Descriptor length

        stream.read_unsigned_int(); // Key Value Data offset
        stream.read_unsigned_int(); // Key Value Data length

        stream.read_unsigned_long(); // Supercompression Global Data offset
        stream.read_unsigned_long(); // Supercompression Global Data length

        return level_count;
    }
}

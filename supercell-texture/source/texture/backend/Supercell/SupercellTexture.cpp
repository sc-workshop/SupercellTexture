#include "SupercellTexture.h"

#include "ScPixel.hpp"
#include "compression/compression.h"
#include "core/hashing/ncrypto/fnv.h"
#include "texture/SCTX/Header_generated.h"
#include "texture/SCTX/MipMapData_generated.h"

using namespace wk;

namespace sc::texture {
    SupercellTexture::SupercellTexture(uint16_t width, uint16_t height, ScPixel::Type type, Ref<wk::Stream> buffer) :
        m_pixel_type(type),
        m_data(buffer) {
        m_width = width;
        m_height = height;
        std::vector<uint8_t> hash;
        m_levels.emplace_back(width, height, 0, hash);
    }

    SupercellTexture::SupercellTexture(wk::Stream& stream) {
        m_stream = &stream;
        read_streaming_data();
    }

    SupercellTexture::SupercellTexture(wk::RawImage& image, ScPixel::Type type, bool mip_maps) {
        // Writable dynamic buffer
        m_data = wk::CreateRef<wk::BufferStream>();

        m_pixel_type = type;
        m_width = image.width();
        m_height = image.height();

        // Level 0
        create_new_level(image);

        if (mip_maps) {
            generate_mip_maps(image);
        }
    }

    void SupercellTexture::read_streaming_data() {
        uint32_t texture_data_length = m_stream->read_unsigned_int();
        MemoryStream texture_data(texture_data_length);
        m_stream->read(texture_data.data(), texture_data_length);

        {
            flatbuffers::Verifier verifier((const uint8_t*) texture_data.data(), texture_data.length());
            bool is_valid_sctx = SCTX::VerifyHeaderBuffer(verifier);
            if (!is_valid_sctx) {
                throw wk::Exception("Failed to verify SCTX integrity");
            }
        }

        uint32_t levels_count;

        {
            const SCTX::Header* texture = SCTX::GetHeader(texture_data.data());
            m_pixel_type = (ScPixel::Type) texture->pixel_type();
            m_width = texture->width();
            m_height = texture->height();
            levels_count = texture->levels_count();
            m_texture_data_length = texture->texture_length();

            set_data_flags((uint32_t) texture->flags());

            auto* extensions = texture->extensions();
            if (extensions && extensions->extension()) {
                auto extensions_data = extensions->extension();
                auto extensions_types = extensions->extension_type();

                for (flatbuffers::Vector<SCTX::Extensions>::size_type i = 0; extensions_data->size() > i; i++) {
                    auto texture_extension_type = extensions_types->Get(i);
                    auto texture_extension = extensions_data->Get(i);

                    if (texture_extension_type == SCTX::Extension::ExtensionProxyTexture) {
                        auto proxy_texture = static_cast<const SCTX::ExtensionProxyTexture*>(texture_extension);
                        auto proxy_texture_data = proxy_texture->data();

                        wk::Ref<MemoryStream> proxy_texture_stream =
                            CreateRef<MemoryStream>(proxy_texture_data->size());
                        Memory::copy((uint8_t*) proxy_texture_data->data(),
                                     (uint8_t*) proxy_texture_stream->data(),
                                     proxy_texture_data->size());
                        proxy_textures.emplace_back(proxy_texture->width(),
                                                    proxy_texture->height(),
                                                    (ScPixel::Type) proxy_texture->pixel_type(),
                                                    proxy_texture_stream);
                    } else if (texture_extension_type == SCTX::Extension::ExtensionASTCEncodingParams) {
                        auto astc_params_extension =
                            static_cast<const SCTX::ExtensionASTCEncodingParams*>(texture_extension);
                        astc_encode_params = astc_params_extension->value()->str();
                    } else if (texture_extension_type == SCTX::Extension::ExtensionTags) {
                        auto tags_extension = static_cast<const SCTX::ExtensionTags*>(texture_extension);
                        auto tags_data = tags_extension->tags();
                        for (flatbuffers::Vector<SCTX::ExtensionTags>::size_type t = 0; tags_data->size() > t; t++) {
                            tags.emplace_back(tags_data->Get(t)->str());
                        }
                    }
                }
            }
        }

        uint32_t mip_maps_data_length = m_stream->read_unsigned_int();
        size_t texture_data_offset = m_stream->position() + mip_maps_data_length;

        {
            m_levels.reserve(levels_count);
            for (uint32_t i = 0; levels_count > i; i++) {
                uint32_t mip_map_data_length = m_stream->read_unsigned_int();
                wk::MemoryStream mip_map_buffer(mip_map_data_length);
                m_stream->read(mip_map_buffer.data(), mip_map_data_length);

                auto mip_map_data = SCTX::GetMipMap(mip_map_buffer.data());

                std::vector<uint8_t> hash(mip_map_data->hash()->begin(), mip_map_data->hash()->end());
                m_levels.emplace_back(mip_map_data->width(), mip_map_data->height(), mip_map_data->offset(), hash);
            }
        }

        m_stream->seek(texture_data_offset);
    }

    void SupercellTexture::generate_mip_maps(wk::RawImage& image) {
        float width = (float) image.width();
        float height = (float) image.height();
        while (true) {
            width = width / 2.0f;
            height = height / 2.0f;
            if (1.0f > width && 1.0f > height)
                break;

            wk::RawImage mip_map(1.0f > width ? 1 : (uint16_t) std::ceil(width),
                                 1.0f > height ? 1 : (uint16_t) std::ceil(height),
                                 image.depth(),
                                 image.colorspace());

            image.copy(mip_map);
            create_new_level(mip_map);
        }
    }

    void SupercellTexture::create_new_level(wk::RawImage& image) {
        if (!m_data)
            return;
        if (!m_data->is_writable())
            return;

        ScPixel::Compression compression = ScPixel::get_compression(m_pixel_type);
        auto transcode = [&compression, this](wk::RawImage& image) {
            wk::SharedMemoryStream input(image.data(), image.data_length());
            switch (compression) {
                case ScPixel::Compression::RAW:
                    m_data->write(image.data(), image.data_length());
                    break;
                case ScPixel::Compression::ASTC:
                    SupercellTexture::compress_astc(image.width(), image.height(), m_pixel_type, input, *m_data);
                    break;
                default:
                    throw wk::Exception("Unsupported compression format!");
                    break;
            }
        };

        size_t data_begin = m_data->position();

        if (ScPixel::get_depth(m_pixel_type) != image.depth()) {
            wk::RawImage temp(image.width(), image.height(), ScPixel::get_depth(m_pixel_type), image.colorspace());
            image.copy(temp);

            transcode(temp);
        } else {
            transcode(image);
        }

        size_t data_end = m_data->position();
        size_t level_index = m_levels.size();

        ScTextureLevel& level = m_levels.emplace_back();
        level.width = image.width();
        level.height = image.height();
        level.offset = (uint32_t) data_begin;

        uint64_t texture_hash = hash::fnv1a64(data(level_index), data_end - data_begin);

        level.hash.resize(sizeof(texture_hash));
        wk::Memory::copy(&texture_hash, level.hash.data(), level.hash.size());
    }

    bool SupercellTexture::read_data() const {
        if (!m_stream)
            return false;
        if (m_texture_data_length == 0)
            return false;

        if (use_padding) {
            m_stream->seek((m_stream->position() + 15) & ~15);
        }

        m_data = wk::CreateRef<wk::MemoryStream>(m_texture_data_length);
        bool is_compressed = ZstdDecompressor::validate(*m_stream);

        if (is_compressed) {
            ZstdDecompressor decompressor;
            decompressor.decompress(*m_stream, *m_data);
        } else {
            m_stream->read(m_data->data(), m_texture_data_length);
        }

        return true;
    }

    bool SupercellTexture::loaded() const {
        return bool(m_data);
    }

    Image::BasePixelType SupercellTexture::base_type() const {
        return ScPixel::get_base_pixel_type(m_pixel_type);
    }

    Image::ColorSpace SupercellTexture::colorspace() const {
        return ScPixel::get_colorspace(m_pixel_type);
    }

    Image::PixelDepth SupercellTexture::depth() const {
        return ScPixel::get_depth(m_pixel_type);
    }

    std::size_t SupercellTexture::data_length() const {
        return data_length(0);
    }

    std::uint8_t* SupercellTexture::data() const {
        return data(0);
    }

    std::size_t SupercellTexture::data_length(size_t level_index) const {
        if (!m_data)
            return 0;

        const ScTextureLevel& level = get_level(level_index);
        return m_data->length() - level.offset;
    }

    std::uint8_t* SupercellTexture::data(size_t level_index) const {
        if (!loaded())
            read_data();
        if (!m_data)
            return nullptr;

        const ScTextureLevel& level = get_level(level_index);
        return (std::uint8_t*) m_data->data() + level.offset;
    }

    void SupercellTexture::clear() {
        m_data.reset();
    }

    bool SupercellTexture::is_compressed() const {
        return ScPixel::get_compression(m_pixel_type) != ScPixel::Compression::RAW;
    }

    size_t SupercellTexture::level_count() const {
        return m_levels.size();
    }

    ScPixel::Type SupercellTexture::pixel_type() const {
        return m_pixel_type;
    }

    const ScTextureLevel& SupercellTexture::get_level(size_t index) const {
        return m_levels[index];
    }

    std::size_t SupercellTexture::decompressed_data_length(size_t level_idx) {
        if (!loaded())
            return 0;

        const ScTextureLevel& level = get_level(level_idx);

        return Image::calculate_image_length(level.width, level.height, depth());
    }

    void SupercellTexture::decompress_data(wk::Stream& buffer, size_t level_idx) {
        if (!loaded())
            read_data();

        const ScTextureLevel& level = get_level(level_idx);

        wk::SharedMemoryStream stream((uint8_t*) m_data->data() + level.offset, m_data->length() - level.offset);
        SupercellTexture::decompress_data(m_width, m_height, m_pixel_type, stream, buffer);
    }

    std::size_t SupercellTexture::decompressed_data_length() {
        return decompressed_data_length(0);
    }

    void SupercellTexture::decompress_data(wk::Stream& buffer) {
        decompress_data(buffer, 0);
    }

    void SupercellTexture::write(wk::Stream& buffer) {
        using namespace flatbuffers;
        FlatBufferBuilder builder(2048);

        // calling data getter just to load data in case if its not loaded and to make sure that texture is valid
        if (data() == nullptr) {
            throw wk::Exception("Texture data is incorrect or empty");
        }

        // Data chunk
        {
            std::vector<SCTX::Extension> extensions_types;
            std::vector<Offset<void>> extensions;

            if (!tags.empty()) {
                std::vector<Offset<String>> off_tags;
                off_tags.reserve(tags.size());
                for (const std::string& tag : tags) {
                    off_tags.push_back(builder.CreateString(tag));
                }
                Offset<SCTX::ExtensionTags> off_extension_tags = SCTX::CreateExtensionTagsDirect(builder, &off_tags);

                extensions.push_back(off_extension_tags.Union());
                extensions_types.push_back(SCTX::Extension::ExtensionTags);
            }

            if (!astc_encode_params.empty()) {
                Offset<String> off_astc_encode_params = builder.CreateString(astc_encode_params);
                Offset<SCTX::ExtensionASTCEncodingParams> off_extension_astc_params =
                    SCTX::CreateExtensionASTCEncodingParams(builder, off_astc_encode_params);

                extensions.push_back(off_extension_astc_params.Union());
                extensions_types.push_back(SCTX::Extension::ExtensionASTCEncodingParams);
            }

            if (!proxy_textures.empty()) {
                std::vector<Offset<SCTX::ExtensionProxyTexture>> off_proxy_textures;
                off_proxy_textures.reserve(proxy_textures.size());
                for (const SupercellTexture& proxy_texture : proxy_textures) {
                    Offset<SCTX::ExtensionProxyTexture> off_proxy_texture =
                        SCTX::CreateExtensionProxyTexture(builder,
                                                          proxy_texture.width(),
                                                          proxy_texture.height(),
                                                          (uint16_t) proxy_texture.pixel_type(),
                                                          builder.CreateVector(proxy_texture.data(),
                                                                               proxy_texture.data_length()));
                    off_proxy_textures.push_back(off_proxy_texture);
                }
                Offset<Vector<Offset<SCTX::ExtensionProxyTexture>>> off_proxy_textures_vector =
                    builder.CreateVector(off_proxy_textures);

                extensions.push_back(off_proxy_textures_vector.Union());
                extensions_types.push_back(SCTX::Extension::ExtensionProxyTexture);
            }

            Offset<SCTX::Extensions> off_extensions;
            if (!extensions.empty()) {
                Offset<Vector<SCTX::Extension>> off_extension_types_vector = builder.CreateVector(extensions_types);
                Offset<Vector<Offset<void>>> off_extensions_vector = builder.CreateVector(extensions);
                off_extensions = SCTX::CreateExtensions(builder, off_extension_types_vector, off_extensions_vector);
            }

            Offset<SCTX::Header> off_texture_data = SCTX::CreateHeader(builder,
                                                                       0,
                                                                       (uint32_t) m_pixel_type,
                                                                       m_width,
                                                                       m_height,
                                                                       (uint8_t) m_levels.size(),
                                                                       0,
                                                                       (SCTX::TextureFlags) get_data_flags(),
                                                                       (uint32_t) m_data->length(),
                                                                       0,
                                                                       0,
                                                                       off_extensions);

            builder.FinishSizePrefixed(off_texture_data, SCTX::HeaderIdentifier());

            auto texture_data = builder.GetBufferSpan();
            buffer.write(texture_data.data(), (uint32_t) texture_data.size_bytes());
            builder.Clear();
        }

        // Mip maps chunk
        {
            wk::BufferStream mip_maps;
            for (const ScTextureLevel& level : m_levels) {
                Offset<SCTX::MipMap> off_mip_map =
                    SCTX::CreateMipMapDirect(builder, level.width, level.height, level.offset, &level.hash);
                builder.FinishSizePrefixed(off_mip_map);

                auto mip_map_data = builder.GetBufferSpan();
                mip_maps.write(mip_map_data.data(), mip_map_data.size_bytes());
                builder.Clear();
            }

            buffer.write_unsigned_int((uint32_t) mip_maps.length());
            buffer.write(mip_maps.data(), mip_maps.length());
        }

        if (use_padding) {
            size_t pad_bytes = ((buffer.position() + 15) & ~15) - buffer.position();
            for (size_t i = 0; pad_bytes > i; i++) {
                buffer.write_unsigned_byte(0);
            }
        }

        // Data itself
        if (use_compression) {
            ZstdCompressor::Props props;
            props.compression_level = 16;
            props.checksum_flag = false;
            props.content_size_flag = true;

            ZstdCompressor compressor(props);
            m_data->seek(0);
            compressor.compress(*m_data, buffer);
        } else {
            buffer.write(m_data->data(), m_data->length());
        }
    }

    void SupercellTexture::decompress_data(
        uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output) {
        ScPixel::Compression compression = ScPixel::get_compression(type);

        input.seek(0);
        switch (compression) {
            case ScPixel::Compression::RAW:
                output.write(input.data(), input.length());
                break;
            case ScPixel::Compression::ASTC:
                decompress_astc(width, height, type, input, output);
                break;
            default:
                throw wk::Exception("Unsupported compression!");
        }
    }

    void SupercellTexture::set_data_flags(uint32_t flags) {
        using SCTX::TextureFlags;

        use_compression = (flags & (uint32_t) TextureFlags::use_compression) > 0;
        unknown_flag1 = (flags & (uint32_t) TextureFlags::unknown_flag2) > 0;
        unknown_flag2 = (flags & (uint32_t) TextureFlags::unknown_flag3) > 0;
        use_padding = (flags & (uint32_t) TextureFlags::use_padding) > 0;
    }

    uint32_t SupercellTexture::get_data_flags() const {
        using SCTX::TextureFlags;

        uint8_t result = 0;

        if (use_compression)
            result |= (uint32_t) TextureFlags::use_compression;
        if (unknown_flag1)
            result |= (uint32_t) TextureFlags::unknown_flag2;
        if (unknown_flag2)
            result |= (uint32_t) TextureFlags::unknown_flag3;
        if (use_padding)
            result |= (uint32_t) TextureFlags::use_padding;

        return result;
    }

    void SupercellTexture::decompress_astc(
        uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output) {
        auto [x, y, z] = ScPixel::get_astc_blocks(type);

        ASTCDecompressor::Props props;
        props.blocks_x = x;
        props.blocks_y = y;
        props.profile =
            ScPixel::get_colorspace(type) == ColorSpace::Linear ? astc::Profile::PRF_LDR : astc::Profile::PRF_LDR_SRGB;

        ASTCDecompressor context(props);
        context.decompress(width, height, Image::BasePixelType::RGBA, input, output);
    }

    void SupercellTexture::compress_astc(
        uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output) {
        auto [x, y, z] = ScPixel::get_astc_blocks(type);

        ASTCCompressor::Props props;
        props.blocks_x = x;
        props.blocks_y = y;

        ASTCCompressor context(props);
        context.compress(width, height, ScPixel::get_base_pixel_type(type), input, output);
    }
}
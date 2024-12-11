#pragma once

#include "ScPixel.hpp"
#include "ScMipMap.h"

#include "texture/interface/compressed_image.h"
#include "core/io/memory_stream.h"
#include "core/io/file_stream.h"

#include <filesystem>
#include <optional>

namespace sc::texture
{
	class SupercellTexture final : public CompressedImage
	{
	public:
		using IdArray = std::vector<uint32_t>;
		using StreamingTextureArray = std::vector<SupercellTexture>;
		using MipMapsArray = std::vector<ScMipMap>;

	private:
		using StreamT = wk::InputFileStream;

	public:
		SupercellTexture(uint16_t width, uint16_t height, ScPixel::Type type, wk::Ref<wk::MemoryStream> buffer);
		SupercellTexture(std::filesystem::path path);

	public:
		virtual void write(wk::Stream& buffer);

	public:
		virtual BasePixelType base_type() const;
		virtual ColorSpace colorspace() const;
		virtual PixelDepth depth() const;

		virtual std::size_t data_length() const;
		virtual std::uint8_t* data() const;

		virtual bool is_compressed() const;

		void clear();

		bool read_data();

	public:
		virtual std::size_t decompressed_data_length()
		{
			return Image::calculate_image_length(m_width, m_height, depth());
		}

		virtual void decompress_data(wk::Stream& buffer);

	public:
		static void decompress_data(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		static void decompress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		void read_streaming_data();

	public:
		std::optional<StreamingTextureArray> streaming_textures;
		std::optional<IdArray> streaming_ids;
		std::optional<MipMapsArray> mip_maps;

		uint32_t unknown_integer = 0;

	private:
		ScPixel::Type m_pixel_type = ScPixel::Type::UNKNOWN;
		size_t m_texture_data_length = 0;

		wk::Ref<wk::MemoryStream> m_data;
		wk::Ref<StreamT> m_stream;
	};
}
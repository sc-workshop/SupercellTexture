#pragma once

#include "ScPixel.hpp"
#include "ScTextureLevel.h"

#include "texture/interface/compressed_image.h"
#include "core/io/memory_stream.h"
#include "core/io/shared_memory_stream.h"
#include "core/io/buffer_stream.h"
#include "core/io/file_stream.h"

#include <filesystem>
#include <optional>

namespace sc::texture
{
	class SupercellTexture final : public CompressedImage
	{
	public:
		using IdArray = std::vector<uint32_t>;
		using VariantsArray = std::vector<SupercellTexture>;

	private:
		using StreamT = wk::InputFileStream;

	public:
		SupercellTexture(uint16_t width, uint16_t height, ScPixel::Type type, wk::Ref<wk::MemoryStream> buffer);
		SupercellTexture(std::filesystem::path path);

	public:
		virtual void write(wk::Stream& buffer);
		void write(wk::Stream& buffer, bool compress_data);

	public:
		virtual BasePixelType base_type() const;
		virtual ColorSpace colorspace() const;
		virtual PixelDepth depth() const;

		virtual std::size_t data_length() const;
		virtual std::uint8_t* data() const;

		virtual std::size_t data_length(size_t level_index) const;
		virtual std::uint8_t* data(size_t level_index) const;

		virtual bool is_compressed() const;

		void clear();

		bool read_data();

		size_t level_count() const;
		const ScPixel::Type pixel_type() const;

		const ScTextureLevel& get_level(size_t index) const;

	public:
		virtual std::size_t decompressed_data_length();
		virtual void decompress_data(wk::Stream& buffer);

		virtual std::size_t decompressed_data_length(size_t level);
		virtual void decompress_data(wk::Stream& buffer, size_t level);

	public:
		static void decompress_data(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		static void decompress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		void read_streaming_data();

	public:
		std::optional<VariantsArray> streaming_variants;
		std::optional<IdArray> streaming_ids;

		uint32_t unknown_integer = 0;

	private:
		ScPixel::Type m_pixel_type = ScPixel::Type::UNKNOWN;
		size_t m_texture_data_length = 0;

		wk::Ref<wk::MemoryStream> m_data;
		wk::Ref<StreamT> m_stream;
		std::vector<ScTextureLevel> m_levels;
	};
}
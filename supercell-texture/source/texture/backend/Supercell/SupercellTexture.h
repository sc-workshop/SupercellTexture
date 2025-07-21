#pragma once

#include "ScPixel.hpp"
#include "ScTextureLevel.h"

#include "texture/interface/compressed_image.h"
#include "core/io/memory_stream.h"
#include "core/io/shared_memory_stream.h"
#include "core/io/buffer_stream.h"
#include "core/io/file_stream.h"
#include "core/image/raw_image.h"

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
		// Texture variant constructor
		SupercellTexture(uint16_t width, uint16_t height, ScPixel::Type type, wk::Ref<wk::MemoryStream> buffer);

		// File constructor
		SupercellTexture(std::filesystem::path path);

		// New SCTX texture constructor
		SupercellTexture(wk::RawImage& image, ScPixel::Type type, bool generate_mip_maps = false);

	public:
		virtual void write(wk::Stream& buffer);

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

		bool read_data() const;
		bool loaded() const;

		size_t level_count() const;
		const ScPixel::Type pixel_type() const;

		const ScTextureLevel& get_level(size_t index) const;

	public:
		virtual std::size_t decompressed_data_length();
		virtual void decompress_data(wk::Stream& buffer);

		virtual std::size_t decompressed_data_length(size_t level);
		virtual void decompress_data(wk::Stream& buffer, size_t level);

		void set_data_flags(uint32_t flags);
		uint32_t get_data_flags() const;

	public:
		static void decompress_data(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		static void decompress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);
		static void compress_astc(uint16_t width, uint16_t height, ScPixel::Type type, wk::Stream& input, wk::Stream& output);

	private:
		void read_streaming_data();
		void generate_mip_maps(wk::RawImage& image);
		void create_new_level(wk::RawImage& image);

	public:
		std::optional<VariantsArray> streaming_variants;
		std::optional<IdArray> streaming_ids;

		bool use_compression = false;
		bool unknown_flag1 = false;
		bool unknown_flag2 = false;
		bool use_padding = false;

	private:
		ScPixel::Type m_pixel_type = ScPixel::Type::UNKNOWN;
		size_t m_texture_data_length = 0;

		mutable wk::Ref<wk::Stream> m_data;
		mutable wk::Ref<StreamT> m_stream;

		std::vector<ScTextureLevel> m_levels;
	};
}
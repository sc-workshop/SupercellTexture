#pragma once

#include <core/image/image.h>
#include <core/preprocessor/api.h>

namespace sc
{
	class SUPERCELL_API CompressedImage : public Image
	{
	public:
		CompressedImage() = default;
		virtual ~CompressedImage() = default;

	public:
		virtual std::size_t decompressed_data_length() = 0;
		virtual void decompress_data(Stream& buffer) = 0;
	};
}
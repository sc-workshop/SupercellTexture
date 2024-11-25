#pragma once

#include "core/image/image.h"
#include "core/preprocessor/api.h"

namespace sc
{
	class WORKSHOP_API CompressedImage : public wk::Image
	{
	public:
		CompressedImage() = default;
		virtual ~CompressedImage() = default;

	public:
		virtual std::size_t decompressed_data_length() = 0;
		virtual void decompress_data(wk::Stream& buffer) = 0;
	};
}
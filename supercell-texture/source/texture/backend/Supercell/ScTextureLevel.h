#pragma once

#include "core/io/memory_stream.h"

namespace sc
{
	class ScTextureLevel
	{
	public:
		ScTextureLevel(uint16_t _width, uint16_t _height, uint32_t _offset, wk::MemoryStream& _hash) : width(_width), height(_height), offset(_offset), hash(_hash)
		{

		}
	public:
		uint16_t width = 0;
		uint16_t height = 0;
		uint32_t offset = 0;
		wk::MemoryStream hash;
	};
}
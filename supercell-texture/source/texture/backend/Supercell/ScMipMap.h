#pragma once

#include "core/io/memory_stream.h"

namespace sc
{
	class ScMipMap
	{
	public:
		ScMipMap(uint16_t _width, uint16_t _height, uint32_t _unknown_length, wk::MemoryStream& _hash) : width(_width), height(_height), unknown_length(_unknown_length), hash(_hash)
		{

		}
	public:
		uint16_t width = 0;
		uint16_t height = 0;
		uint32_t unknown_length = 0;
		wk::MemoryStream hash;
	};
}
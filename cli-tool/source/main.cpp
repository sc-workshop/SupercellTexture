#include <filesystem>
#include <core/console/console.h>
#include <core/io/file_stream.h>

#include "texture/texture.h"

int main(int argc, const char** argv)
{
	std::filesystem::path executable = argv[0];
	std::filesystem::path executable_name = executable.stem();

	// Arguments
	sc::ArgumentParser parser(executable_name.string(), "Tool for compress and decompress textures");

	parser.add_argument("mode")
		.help("Possible values: compress, decompress")
		.choices("compress", "decompress");

	// TODO: console app
	return -1;
}
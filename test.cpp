#define XML_SERIALIZER_IMPLEMENTATION
#include "xml_serializer.hpp"
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include <rapidXML.hpp>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fs = std::filesystem;

std::vector<char> load_file(fs::path path) {
	std::fstream f(path, std::ios::in | std::ios::binary | std::ios::ate);
	if(!f.good()) return {0};
	size_t fsize = f.tellg();
	f.seekg(f.beg);
	std::vector<char> ret(fsize + 1);
	f.read(ret.data(), fsize);
	ret[fsize] = 0;
	return ret;
}

int main(int argc, const char *argv[]) {
	std::cout << fs::current_path() << "\n";
	SetConsoleOutputCP(65001);

	rapidxml::xml_document jmdict_doc;

	{
		auto jmdict_file = load_file("xml_samples/jmdict.xml");
		try {
			jmdict_doc.parse<0>(jmdict_file.data());
		} catch(rapidxml::parse_error e) {
			std::cout << e.what();
			return 1;
		}
	}

	parse_node(&jmdict_doc);

	return 0;
}

#define XML_SERIALIZER_IMPLEMENTATION
#define XML_SERIALIZER_DEBUG
#include "xml_serializer.hpp"
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include <rapidXML.hpp>
#include <rapidXML_print.hpp>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

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

/* *INDENT-OFF* */
/* clang-format off */
XMLSerializationLevel test_serialization = {{
	{"JMdict", {{
		{"entry", {{
			{"ent_seq", {{}, [](auto v) {
				std::cout << std::stoll(v->value()) << "\n";
			}}},
			{"k_ele", {{
				{"keb", {{}, [](auto v){
					std::cout << "kanji element: " << v->value() << "\n";
				}}}
			}}},
			{"r_ele", {{
				{"reb", {{}, [](auto v){
					std::cout << "reading: " << v->value() << "\n";
				}}}
			}}}
		}}}
	}}}
}};
/* clang-format on */
/* *INDENT-ON* */

int main(int argc, const char *argv[]) {
	std::cout << fs::current_path() << "\n";

#ifdef _WIN32
	SetConsoleOutputCP(65001);
#endif

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

	xml_serialize_node(&jmdict_doc, &test_serialization);

	return 0;
}

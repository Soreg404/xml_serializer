#pragma once
#include <cstring>
#include <iostream>
#include <map>
#include <rapidXML.hpp>
#include <rapidXML_print.hpp>
#include <string>
#define LOG(x, ...) printf(x "\n", ##__VA_ARGS__)

struct XMLSerializationLevel {
	using xml_def = std::pair<std::string, XMLSerializationLevel>;
	using xml_map = std::map<std::string, XMLSerializationLevel>;
	xml_map nested_defs;

	using serialization_func = void (*)(rapidxml::xml_node<> *);
	serialization_func serialize = nullptr;
};

using xs_level = XMLSerializationLevel;
using xs_map = XMLSerializationLevel::xml_map;
using xs_def = XMLSerializationLevel::xml_def;
using xs_func = XMLSerializationLevel::serialization_func;

/* *INDENT-OFF* */
/* clang-format off */
xs_level test = {{
	{"JMdict", {{
		{"entry", {{
			{"ent_seq", {{}, [](auto v) {
				std::cout << std::stoll(v->value()) << "\n";
			}}},
			{"k_ele", {{}, [](auto v) {
				std::cout << *v << "\n";
			}}}
		}}}
	}}}
}};
/* clang-format on */
/* *INDENT-ON* */

void parse_node(rapidxml::xml_node<> *element);

#ifdef XML_SERIALIZER_IMPLEMENTATION

void element_dfs(rapidxml::xml_node<> *current_element, XMLSerializationLevel *current_rules) {
	LOG("== begin %s ==", current_element->name());

	if(current_rules->serialize) {
		LOG("serialization");
		current_rules->serialize(current_element);
	}

	auto nested_element = current_element->first_node();
	if(!nested_element || std::strcmp(nested_element->name(), "") == 0) return;

	LOG("> nest [%s]", current_element->name());
	std::string cname;
	do {
		cname = nested_element->name();

		auto serialize_frag_found = current_rules->nested_defs.find(cname);
		if(serialize_frag_found == current_rules->nested_defs.end()) {
			LOG("! no serialization rule [%s]", nested_element->name());
			continue;
		} else {
			element_dfs(nested_element, &(serialize_frag_found->second));
		}

	} while(nested_element = nested_element->next_sibling());

	LOG("== end %s ==", current_element->name());
}

void parse_node(rapidxml::xml_node<> *element) {
	element_dfs(element, &test);
}

#endif
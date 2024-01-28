#pragma once
#include <cstring>
#include <iostream>
#include <map>
#include <rapidXML.hpp>
#include <rapidXML_print.hpp>
#include <string>

#ifdef XML_SERIALIZER_DEBUG
#define XML_SERIALIZER_LOG(x, ...) printf(x "\n", ##__VA_ARGS__)
#else
#define XML_SERIALIZER_LOG(...)
#endif

struct XMLSerializationLevel {
	using xml_def = std::pair<std::string, XMLSerializationLevel>;
	using xml_map = std::map<std::string, XMLSerializationLevel>;
	xml_map nested_defs;

	using serialization_func = void (*)(rapidxml::xml_node<> *);
	serialization_func serialize = nullptr;
};

void xml_serialize_node(rapidxml::xml_node<> *element, XMLSerializationLevel *serialization_rules);

#ifdef XML_SERIALIZER_IMPLEMENTATION

void element_dfs(rapidxml::xml_node<> *current_element, XMLSerializationLevel *current_rules) {
	XML_SERIALIZER_LOG("== begin %s ==", current_element->name());

	if(current_rules->serialize) {
		XML_SERIALIZER_LOG("serialization");
		current_rules->serialize(current_element);
	}

	auto nested_element = current_element->first_node();
	if(!nested_element || std::strcmp(nested_element->name(), "") == 0) return;

	XML_SERIALIZER_LOG("> nest [%s]", current_element->name());
	std::string cname;
	do {
		cname = nested_element->name();

		auto serialize_frag_found = current_rules->nested_defs.find(cname);
		if(serialize_frag_found == current_rules->nested_defs.end()) {
			XML_SERIALIZER_LOG("! no serialization rule [%s]", nested_element->name());
			continue;
		} else {
			element_dfs(nested_element, &(serialize_frag_found->second));
		}

	} while(nested_element = nested_element->next_sibling());

	XML_SERIALIZER_LOG("== end %s ==", current_element->name());
}

void xml_serialize_node(rapidxml::xml_node<> *element, XMLSerializationLevel *serialization_rules) {
	element_dfs(element, serialization_rules);
}

#endif
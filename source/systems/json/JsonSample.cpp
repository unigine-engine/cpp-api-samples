#include "JsonSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;


// here is an example of how to create JSON file
JsonPtr JsonSample::json_create()
{
	JsonPtr json = Json::create("node");
	JsonPtr json_1 = json->addChild();
	json_1->setObject();
	json_1->setName("object");
	json_1->addChild("child", "hello from json");
	JsonPtr array = json->addChild("array");
	array->setArray();
	JsonPtr array_1 = array->addChild("array_child_0");
	array_1->setObject();
	array_1->addChild("object_child_number", 12);

	JsonPtr array_2 = array->addChild("array_child_1");
	array_2->addChild("object_child_string", "some string");
	JsonPtr array_3 = array->addChild("array_child_2");
	JsonPtr bool_child = array_3->addChild("object_child_bool");
	bool_child->setBool(true);
	JsonPtr array_4 = array->addChild("array_child_3");
	JsonPtr null_child = array_4->addChild("object_child_null", false);
	null_child->setNull();

	json->setAPIInterfaceOwner(false);
	return json;
}

void JsonSample::json_print(const JsonPtr &json)
{
	json_print_node(json, 0);
}


// here is an example of how you can access JSON data
void JsonSample::json_print_node(const JsonPtr &json, int offset, int index, bool print_name)
{
	bool is_comma_needed = false;
	JsonPtr parent = json->getParent();
	if (parent)
	{
		is_comma_needed = parent->getNumChildren() != index;
	}

	if (json->isObject())
	{
		if (print_name)
		{
			print_string_offset(String::format("%s: {", json->getName()).get(), offset);
		}
		else
		{
			print_string_offset("{", offset);
		}
		const int num_children = json->getNumChildren();
		for (int i = 0; i < num_children; i++)
		{
			json_print_node(json->getChild(i), offset + 1, i + 1);
		}
		print_string_offset("}", offset, is_comma_needed);
	}

	if (json->isArray())
	{
		if (print_name)
		{
			print_string_offset(String::format("%s: [", json->getName()).get(), offset);
		}
		else
		{
			print_string_offset("[", offset);
		}
		const int num_children = json->getNumChildren();
		for (int i = 0; i < num_children; i++)
		{
			JsonPtr child = json->getChild(i);
			json_print_node(child, offset + 1, i + 1, false);
		}
		print_string_offset("]", offset, is_comma_needed);
	}

	if (json->isNumber())
	{
		if (print_name)
		{
			print_string_offset(
				String::format("%s: %.2f", json->getName(), json->getNumber()).get(), offset,
				is_comma_needed);
		}
		else
		{
			print_string_offset(String::format("%.2f", json->getNumber()).get(), is_comma_needed);
		}
	}
	if (json->isString())
	{
		if (print_name)
		{
			print_string_offset(
				String::format("%s: \"%s\"", json->getName(), json->getString().get()).get(),
				offset, is_comma_needed);
		}
		else
		{
			print_string_offset(String::format("\"%s\"", json->getString().get()).get(), offset,
				is_comma_needed);
		}
	}
	if (json->isBool())
	{
		String value = json->getBool() ? "true" : "false";
		if (print_name)
		{
			print_string_offset(String::format("%s: %s", json->getName(), value.get()).get(),
				offset, is_comma_needed);
		}
		else
		{
			print_string_offset(String::format("%s", value.get()).get(), offset, is_comma_needed);
		}
	}
	if (json->isNull())
	{
		if (print_name)
		{
			print_string_offset(String::format("%s: null", json->getName()).get(), offset,
				is_comma_needed);
		}
		else
		{
			print_string_offset("null", offset, is_comma_needed);
		}
	}
}

void JsonSample::print_string_offset(const Unigine::String &value, int offset, bool print_comma)
{
	for (int i = 0; i < offset; ++i)
	{
		Log::message("\t");
	}
	Log::message("%s", value.get());
	if (print_comma)
	{
		Log::message(",");
	}
	Log::message("\n");
}

//------------------------Sample Logic-----------------------

REGISTER_COMPONENT(JsonSample)

void JsonSample::init()
{
	is_console_onscreen = Console::isOnscreen();
	Console::setOnscreen(true);

	{
		// create the JSON
		const JsonPtr json = json_create();

		// print JSON tree
		json_print(json);
	}
}

void JsonSample::shutdown()
{
	Console::setOnscreen(is_console_onscreen);
}

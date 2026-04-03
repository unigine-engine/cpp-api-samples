// Demonstrates Unigine::Json API for creating and traversing JSON structures.
// Shows building a tree with objects, arrays, strings, numbers, booleans, and nulls.
// Recursive print function formats output with proper indentation and commas.

#include "JsonSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;


// Example of how to create a JSON object
JsonPtr JsonSample::json_create()
{
	// Root JSON node
	JsonPtr json = Json::create("node");

	// Add an object with a child string
	JsonPtr json_1 = json->addChild();
	json_1->setObject();
	json_1->setName("object");
	json_1->addChild("child", "hello from json");

	// Create an array and add multiple children to it
	JsonPtr array = json->addChild("array");
	array->setArray();

	// First array element: object with a number
	JsonPtr array_1 = array->addChild("array_child_0");
	array_1->setObject();
	array_1->addChild("object_child_number", 12);

	// Second array element: string
	JsonPtr array_2 = array->addChild("array_child_1");
	array_2->addChild("object_child_string", "some string");

	// Third array element: boolean
	JsonPtr array_3 = array->addChild("array_child_2");
	JsonPtr bool_child = array_3->addChild("object_child_bool");
	bool_child->setBool(true);

	// Fourth array element: null value
	JsonPtr array_4 = array->addChild("array_child_3");
	JsonPtr null_child = array_4->addChild("object_child_null", false);
	null_child->setNull();

	return json;
}

// Prints entire JSON tree to console with formatting.
void JsonSample::json_print(const JsonPtr &json)
{
	// Print the JSON tree starting from the root
	json_print_node(json, 0);
}


// Recursive JSON printing function
// Goes through objects, arrays, and values, then formats them nicely
void JsonSample::json_print_node(const JsonPtr &json, int offset, int index, bool print_name)
{
	bool is_comma_needed = false;
	JsonPtr parent = json->getParent();
	if (parent)
	{
		is_comma_needed = parent->getNumChildren() != index;
	}

	// Handle object type
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

	// Handle array type
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

	// Handle number type
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

	// Handle string type
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

	// Handle boolean type
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

	// Handle null type
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

// Utility to print strings with indentation and optional comma
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

// Sample Logic
REGISTER_COMPONENT(JsonSample)

// Console is enabled and JSON tree is created and printed.
void JsonSample::init()
{
	is_console_onscreen = Console::isOnscreen();
	Console::setOnscreen(true);

	{
		// Create a JSON tree
		const JsonPtr json = json_create();

		// Print it to console
		json_print(json);
	}
}

// Console state is restored to original value.
void JsonSample::shutdown()
{
	Console::setOnscreen(is_console_onscreen);
}

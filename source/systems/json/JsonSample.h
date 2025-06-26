#pragma once
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class JsonSample : public Unigine::ComponentBase
{

	COMPONENT_DEFINE(JsonSample, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void shutdown();

	Unigine::JsonPtr json_create();
	void json_print(const Unigine::JsonPtr &json);
	void json_print_node(const Unigine::JsonPtr &json, int offset = 0, int index = 0,
		bool print_name = true);
	void print_string_offset(const Unigine::String &value, int offset, bool print_comma = false);

private:
	bool is_console_onscreen{false};
};

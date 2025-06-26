#pragma once

#include <UnigineComponentSystem.h>

class FilesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FilesSample, Unigine::ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(String, filepath, "cpp_samples/Systems/Files/generated_file")

private:
	void init();
	void shutdown();

	void create_writer();
	void create_reader();

private:
	Unigine::WidgetWindowPtr reader;
	Unigine::WidgetWindowPtr writer;

	int widget_padding = 30;
};

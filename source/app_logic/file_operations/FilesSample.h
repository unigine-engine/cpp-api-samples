#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates file I/O using the File class.
// Provides a simple notepad-style UI with separate reader/writer windows.
// Files are stored in the project's data directory.
class FilesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FilesSample, Unigine::ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates basic file I/O by providing a GUI with two windows: "
							"a writer to save text into a file and a reader to display the file contents.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

	// File path used for reading and writing
	PROP_PARAM(String, filepath, "cpp_samples/app_logic/files/generated_file")

private:
	void init();
	void shutdown();

	void create_writer(); // Creates editable text window with save button
	void create_reader(); // Creates read-only display window with refresh button

private:
	Unigine::WidgetWindowPtr reader; // Window displaying file contents
	Unigine::WidgetWindowPtr writer; // Window for editing and saving text

	int widget_padding = 30; // Margin from screen edges
};

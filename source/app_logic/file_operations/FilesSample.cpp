// Demonstrates basic file I/O using Unigine::File. Writer window saves editable
// text to file, Reader window displays file contents. Uses "wb"/"rb" modes for
// binary-safe string operations via writeString/readString.

#include "FilesSample.h"

using namespace Unigine;
using namespace Math;


// Text content is written to file using binary mode for length-prefixed string storage.
void writeTextToFile(const String &content, const String &filepath)
{
	const FilePtr file = File::create();

	// Open file in binary write mode
	if (!file->open(filepath, "wb"))
	{
		Log::error("writeTextToFile: Failed to open file %s\n", filepath.get());
		return;
	}

	// Write string with length prefix (writeString format)
	file->writeString(content.get());

	if (!file->close())
	{
		Log::error("writeTextToFile: Failed to close file %s\n", filepath.get());
	}
}

// Text content is read from file using binary mode for length-prefixed string retrieval.
String readTextFromFile(const String &filepath)
{
	const FilePtr file = File::create();
	// Open file in binary read mode
	if (!file->open(filepath, "rb"))
	{
		Log::error("readTextFromFile: Failed to open file %s\n", filepath.get());
		return "";
	}
	// Read length-prefixed string (readString format)
	String content = file->readString();

	if (!file->close())
	{
		Log::error("readTextFromFile: Failed to close file %s\n", filepath.get());
	}
	return content;
}


REGISTER_COMPONENT(FilesSample)

// Reader and writer windows are created for file I/O demonstration.
void FilesSample::init()
{
	create_reader();
	create_writer();
}

// UI windows are released.
void FilesSample::shutdown()
{
	writer.deleteLater();
	reader.deleteLater();
}

// Writer window with editable text field and save button is created.
void FilesSample::create_writer()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
	GuiPtr gui = main_window->getGui();

	// Create writer window
	writer = WidgetWindow::create(gui, "Writer");
	writer->setWidth(300);
	writer->setHeight(150);
	gui->addChild(writer, Gui::ALIGN_OVERLAP);
	writer->setPosition(widget_padding, main_window->getSize().y / 2);

	// Editable text field is pre-filled with current file contents
	WidgetEditLinePtr edit_line = WidgetEditLine::create(gui, readTextFromFile(filepath.get()));
	edit_line->setCapacity(1000);
	writer->addChild(edit_line, Gui::ALIGN_TOP | Gui::ALIGN_EXPAND);
	edit_line->setText(readTextFromFile(filepath.get()));

	// "Write" button saves current text to file on click
	WidgetButtonPtr write_button = WidgetButton::create(gui, "Write");
	write_button->getEventClicked().connect(*this, [edit_line, this] {
		const char *content = edit_line->getText();
		writeTextToFile(content, filepath.get());
	});
	writer->addChild(write_button, Gui::ALIGN_CENTER | Gui::ALIGN_BOTTOM);
}

// Reader window with read-only label and refresh button is created.
void FilesSample::create_reader()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
	GuiPtr gui = main_window->getGui();

	// Create reader window
	reader = WidgetWindow::create(gui, "Reader");
	reader->setWidth(300);
	reader->setHeight(150);
	gui->addChild(reader, Gui::ALIGN_OVERLAP);
	reader->setPosition(main_window->getSize().x - reader->getWidth() - widget_padding,
		main_window->getSize().y / 2);

	// Label displays file content (read-only display)
	WidgetLabelPtr label = WidgetLabel::create(gui, "");
	label->setEnabled(false);
	reader->addChild(label, Gui::ALIGN_TOP | Gui::ALIGN_EXPAND);

	// "Read" button reloads text from file on click
	WidgetButtonPtr read_button = WidgetButton::create(gui, "Read");

	// Lambda captures label and reloads file content
	const auto read_file = [label, this] {
		label->setText(readTextFromFile(filepath.get()));
	};

	read_button->getEventClicked().connect(*this, read_file);
	// Initial load of file content
	read_file();
	reader->addChild(read_button, Gui::ALIGN_CENTER | Gui::ALIGN_BOTTOM);
}

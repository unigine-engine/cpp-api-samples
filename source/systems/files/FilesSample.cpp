#include "FilesSample.h"

using namespace Unigine;
using namespace Math;

// Here you can see how to perform basic file I/O

//-----------------------------------------------------------
void writeTextToFile(const String &content, const String &filepath)
{
	// create a file
	const FilePtr file = File::create();
	// open file with specified filepath and mode
	if (!file->open(filepath, "wb"))
	{
		Log::error("writeTextToFile: Failed to open file %s\n", filepath.get());
		return;
	}
	file->writeString(content.get());

	if (!file->close())
	{
		Log::error("writeTextToFile: Failed to close file %s\n", filepath.get());
	}
}

String readTextFromFile(const String &filepath)
{
	const FilePtr file = File::create();
	if (!file->open(filepath, "rb"))
	{
		Log::error("readTextFromFile: Failed to open file %s", filepath.get());
		return "";
	}
	String content = file->readString();

	if (!file->close())
	{
		Log::error("readTextFromFile: Failed to close file %s\n", filepath.get());
	}
	return content;
}


// Sample Logic
//-----------------------------------------------------------

REGISTER_COMPONENT(FilesSample)

void FilesSample::init()
{
	create_reader();
	create_writer();
}

void FilesSample::shutdown()
{
	writer.deleteLater();
	reader.deleteLater();
}

void FilesSample::create_writer()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
	GuiPtr gui = main_window->getGui();

	writer = WidgetWindow::create(gui, "Writer");
	writer->setWidth(300);
	writer->setHeight(150);
	gui->addChild(writer, Gui::ALIGN_OVERLAP);
	writer->setPosition(widget_padding, main_window->getSize().y / 2);

	WidgetEditLinePtr edit_line = WidgetEditLine::create(gui, readTextFromFile(filepath.get()));
	edit_line->setCapacity(1000);
	writer->addChild(edit_line, Gui::ALIGN_TOP | Gui::ALIGN_EXPAND);
	edit_line->setText(readTextFromFile(filepath.get()));

	WidgetButtonPtr write_button = WidgetButton::create(gui, "Write");
	write_button->getEventClicked().connect(*this, [edit_line, this] {
		const char *content = edit_line->getText();
		writeTextToFile(content, filepath.get());
	});
	writer->addChild(write_button, Gui::ALIGN_CENTER | Gui::ALIGN_BOTTOM);
}

void FilesSample::create_reader()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();
	GuiPtr gui = main_window->getGui();

	reader = WidgetWindow::create(gui, "Reader");
	reader->setWidth(300);
	reader->setHeight(150);
	gui->addChild(reader, Gui::ALIGN_OVERLAP);
	reader->setPosition(main_window->getSize().x - reader->getWidth() - widget_padding,
		main_window->getSize().y / 2);
	WidgetLabelPtr label = WidgetLabel::create(gui, "");
	label->setEnabled(false);
	reader->addChild(label, Gui::ALIGN_TOP | Gui::ALIGN_EXPAND);
	WidgetButtonPtr read_button = WidgetButton::create(gui, "Read");

	const auto read_file = [label, this] {
		label->setText(readTextFromFile(filepath.get()));
	};

	read_button->getEventClicked().connect(*this, read_file);
	read_file();
	reader->addChild(read_button, Gui::ALIGN_CENTER | Gui::ALIGN_BOTTOM);
}

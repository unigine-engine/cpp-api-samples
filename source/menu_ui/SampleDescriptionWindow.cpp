#include "SampleDescriptionWindow.h"
#include "SampleWidgets.h"
#include <string>

using namespace Unigine;
using namespace Math;


void SampleDescriptionWindow::createWindow(int align, int width)
{
	auto world_path = World::getPath();
	auto world_name = String::filename(world_path);

	String cpp_component_samples_xml_path = FileSystem::getAbsolutePath(String::joinPaths(Engine::get()->getDataPath(), "../cpp_component_samples.sample"));

	XmlPtr cpp_component_samples_xml = Xml::create();
	if (!cpp_component_samples_xml->load(cpp_component_samples_xml_path))
	{
		Unigine::Log::warning("SampleDescriptionWindow::createWindow(): cannot open %s file\n", cpp_component_samples_xml_path.get());
		return;
	}

	XmlPtr cpp_component_samples_samples_pack = cpp_component_samples_xml->getChild("samples_pack");
	XmlPtr samples_xml = cpp_component_samples_samples_pack->getChild("samples");

	String title;
	String description;
	String controls;

	for (int i = 0; i < samples_xml->getNumChildren(); ++i)
	{
		XmlPtr sample_xml = samples_xml->getChild(i);

		if (String::compare(sample_xml->getArg("id"), world_name) == 0)
		{
			title = sample_xml->getArg("title");
			description = sample_xml->getChild("desc")->getChildData("brief");
			if (sample_xml->isChild("controls"))
				controls = sample_xml->getChildData("controls");
			break;
		}
	}

	w_main_window = WidgetWindow::create(title.get());
	w_main_window->setWidth(width);
	w_main_window->arrange();
	WindowManager::getMainWindow()->addChild(w_main_window, Gui::ALIGN_OVERLAP | align);

	init_source_box();

	if (!description.empty())
	{
		w_about_group = WidgetGroupBox::create("About", 8, 8);
		w_main_window->addChild(w_about_group, Gui::ALIGN_LEFT);

		w_about_lbl = WidgetLabel::create(description.get());
		w_about_lbl->setFontWrap(1);
		w_about_lbl->setFontRich(1);
		//		w_about_lbl->setWidth(width);
		w_about_group->addChild(w_about_lbl, Gui::ALIGN_EXPAND);
	}
	if (!controls.empty())
	{
		w_controls_group = WidgetGroupBox::create("Controls", 8, 8);
		w_main_window->addChild(w_controls_group, Gui::ALIGN_LEFT);

		w_controls_lbl = WidgetLabel::create(controls.get());
		w_controls_lbl->setFontWrap(1);
		w_controls_lbl->setFontRich(1);
		//		w_controls_lbl->setWidth(width);
		w_controls_group->addChild(w_controls_lbl, Gui::ALIGN_EXPAND);
	}
}

void SampleDescriptionWindow::shutdown()
{
	w_main_window.deleteLater();
}

Unigine::WidgetLabelPtr SampleDescriptionWindow::addLabel(
	const char *label_text)
{
	if (!w_parameters_grid)
		init_parameter_box();

	auto label = WidgetLabel::create(label_text);
	w_parameters_grid->addChild(WidgetLabel::create());
	w_parameters_grid->addChild(label);
	w_parameters_grid->addChild(WidgetLabel::create());
	return label;
}

WidgetSliderPtr SampleDescriptionWindow::addFloatParameter(const char *name, const char *tooltip,
	float default_value, float min_value, float max_value, std::function<void(float)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	return add_float_parameter(*this, w_parameters_grid, name, tooltip, default_value, min_value, max_value, std::move(on_change));
}

WidgetSliderPtr SampleDescriptionWindow::addIntParameter(const char *name, const char *tooltip,
	int default_value, int min_value, int max_value, std::function<void(int)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	return add_int_parameter(*this, w_parameters_grid, name, tooltip, default_value, min_value, max_value, std::move(on_change));
}

Unigine::WidgetCheckBoxPtr SampleDescriptionWindow::addBoolParameter(
	const char *name, const char *tooltip, bool default_value,
	std::function<void(bool)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	return add_bool_parameter(*this, w_parameters_grid, name, tooltip, default_value, std::move(on_change));
}

WidgetComboBoxPtr SampleDescriptionWindow::addSwitchParameter(const char *name, const char *tooltip, int default_value,
	const Unigine::Vector<const char *> &values, std::function<void(int)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	return add_switch_parameter(*this, w_parameters_grid, name, tooltip, default_value, values, std::move(on_change));
}

WidgetComboBoxPtr SampleDescriptionWindow::addSwitchParameter(const char *name, const char *tooltip, int default_value,
	const char *const *values, int num_values, std::function<void(int)> on_change)
{
	Vector<const char *> value_list;
	for (int i = 0; i < num_values; i += 1)
		value_list.append(values[i]);
	return addSwitchParameter(name, tooltip, default_value, value_list, std::move(on_change));
}

void SampleDescriptionWindow::setStatus(const char *status)
{
	if (!w_status_lbl)
		init_status_box();

	w_status_lbl->setText(status);
	w_status_lbl->arrange();
}

// Normalize and quote path for shell commands (platform-specific)
static std::string quote_for_shell(std::string path)
{
#ifdef _WIN32
	for (char &c : path)
		if (c == '/') c = '\\';
	while (path.size() > 3 && path.back() == '\\')
		path.pop_back();
	std::string out;
	out.reserve(path.size() + 2);
	out.push_back('"');
	for (char c : path)
	{
		if (c == '"') out += "\\\"";
		else out.push_back(c);
	}
	out.push_back('"');
	return out;
#else
	std::string out;
	out.reserve(path.size() + 2);
	out.push_back('\'');
	for (char c : path)
	{
		if (c == '\'') out += "'\\''";
		else out.push_back(c);
	}
	out.push_back('\'');
	return out;
#endif
}

static void open_in_file_manager(const std::string &path)
{
	const std::string p = quote_for_shell(path);
#ifdef _WIN32
	std::system(("explorer " + p).c_str());
#else
	std::system(("sh -c \""
		"if command -v xdg-open >/dev/null 2>&1; then xdg-open " + p + " >/dev/null 2>&1 & "
		"elif command -v gio >/dev/null 2>&1; then gio open " + p + " >/dev/null 2>&1 & "
		"elif command -v kde-open5 >/dev/null 2>&1; then kde-open5 " + p + " >/dev/null 2>&1 & "
		"else exit 127; fi\"").c_str());
#endif
}

static void open_in_editor(const std::string &world)
{
	const std::string p = quote_for_shell(world);
#ifdef UNIGINE_DOUBLE
	const char *editor = "Editor_double_x64";
#else
	const char *editor = "Editor_x64";
#endif
#ifdef _WIN32
	std::system(String::format("start \"\" %s.exe -starting_world %s", editor, p.c_str()));
#else
	std::system(String::format("./%s -starting_world %s &", editor, p.c_str()));
#endif
}

void SampleDescriptionWindow::init_source_box()
{
	String world_path = World::getPath();

	String sample_path = String::pathname(world_path);
	String sample_data_path = String::joinPaths(Engine::get()->getDataPath(), sample_path);
	sample_path = sample_path.replace("cpp_component_samples/","", true);
	String sample_source_path = String::joinPaths(Engine::get()->getDataPath(), "../source/");
	sample_source_path = String::joinPaths(sample_source_path, sample_path);

	WidgetGroupBoxPtr source_group = WidgetGroupBox::create("Browse");
	w_main_window->addChild(source_group, Gui::ALIGN_EXPAND);

	WidgetHBoxPtr hbox = WidgetHBox::create(8,8);
	source_group->addChild(hbox, Gui::ALIGN_EXPAND);

	WidgetButtonPtr source_button = WidgetButton::create("Code in Explorer");
	hbox->addChild(source_button, Gui::ALIGN_EXPAND);
	source_button->getEventClicked().connect(*this, [sample_source_path](){
		Log::message("Source Code Path: '%s'\n", sample_source_path.get());
		open_in_file_manager(sample_source_path.get());
	});

	WidgetButtonPtr editor_button = WidgetButton::create("Content in Editor");
	hbox->addChild(editor_button, Gui::ALIGN_EXPAND);
	editor_button->getEventClicked().connect(*this, [world_path](){
		Log::message("Open in Editor: '%s'\n", world_path.get());
		open_in_editor(world_path.get());
	});
}

const WidgetGroupBoxPtr &SampleDescriptionWindow::getParameterGroupBox()
{
	if (!w_parameters_grid)
		init_parameter_box();
	return w_parameters_group;
}

const WidgetGridBoxPtr &SampleDescriptionWindow::getParameterGridBox()
{
	if (!w_parameters_grid)
		init_parameter_box();
	return w_parameters_grid;
}

void SampleDescriptionWindow::init_parameter_box()
{
	w_parameters_group = WidgetGroupBox::create("Parameters", 8, 8);
	w_main_window->addChild(w_parameters_group, Gui::ALIGN_LEFT);
	w_parameters_grid = WidgetGridBox::create(3);
	w_parameters_group->addChild(w_parameters_grid, Gui::ALIGN_EXPAND);
}

void SampleDescriptionWindow::init_status_box()
{
	w_status_group = WidgetGroupBox::create("Status", 8, 8);
	w_main_window->addChild(w_status_group, Gui::ALIGN_LEFT);
	w_status_lbl = WidgetLabel::create();
	w_status_lbl->setFontWrap(1);
	w_status_lbl->setFontRich(1);
	w_status_lbl->setWidth(300);
	w_status_group->addChild(w_status_lbl, Gui::ALIGN_EXPAND);
}

void SampleDescriptionWindow::addParameterSpacer()
{
	for (int i = 0; i < 3; ++i)
	{
		auto spacer = WidgetSpacer::create();
		w_parameters_grid->addChild(spacer);
	}
}

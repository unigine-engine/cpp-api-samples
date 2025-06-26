#include "SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;


void SampleDescriptionWindow::createWindow(int align, int width)
{
	auto world_path = World::getPath();
	auto world_name = String::filename(world_path);

	String cpp_samples_xml_path = FileSystem::getAbsolutePath(String::joinPaths(Engine::get()->getDataPath(), "../cpp_samples.sample"));

	XmlPtr cpp_samples_xml = Xml::create();
	if (!cpp_samples_xml->load(cpp_samples_xml_path))
	{
		Unigine::Log::warning("SampleDescriptionWindow::createWindow(): cannot open %s file\n", cpp_samples_xml_path.get());
		return;
	}

	XmlPtr cpp_samples_samples_pack = cpp_samples_xml->getChild("samples_pack");
	XmlPtr samples_xml = cpp_samples_samples_pack->getChild("samples");

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
	disconnectAll();
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

	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	w_parameters_grid->addChild(label, Gui::ALIGN_LEFT);
	label->setToolTip(tooltip);

	auto slider = WidgetSlider::create();
	slider->setMinValue((int)(min_value * 100));
	slider->setMaxValue((int)(max_value * 100));
	slider->setValue((int)(default_value * 100));

	slider->setWidth(200);
	slider->setButtonWidth(20);
	slider->setButtonHeight(20);
	slider->setToolTip(tooltip);
	w_parameters_grid->addChild(slider, Gui::ALIGN_LEFT);

	label = WidgetLabel::create(String::ftoa(default_value, 2));
	label->setWidth(20);
	label->setToolTip(tooltip);
	w_parameters_grid->addChild(label);

	slider->getEventChanged().connect(*this, [this, label, slider, on_change]() {
		float v = slider->getValue() / 100.0f;
		label->setText(String::ftoa(v, 2));
		on_change(v);
	});

	return slider;
}

WidgetSliderPtr SampleDescriptionWindow::addIntParameter(const char *name, const char *tooltip,
	int default_value, int min_value, int max_value, std::function<void(int)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	w_parameters_grid->addChild(label, Gui::ALIGN_LEFT);

	auto slider = WidgetSlider::create();
	slider->setMinValue(min_value);
	slider->setMaxValue(max_value);
	slider->setValue(default_value);

	slider->setWidth(200);
	slider->setButtonWidth(20);
	slider->setButtonHeight(20);
	slider->setToolTip(tooltip);
	w_parameters_grid->addChild(slider, Gui::ALIGN_LEFT);

	label = WidgetLabel::create(String::itoa(default_value));
	label->setWidth(20);
	label->setToolTip(tooltip);
	w_parameters_grid->addChild(label);

	slider->getEventChanged().connect(*this, [this, label, slider, on_change]() {
		int v = slider->getValue();
		label->setText(String::itoa(v));
		on_change(v);
	});

	return slider;
}

Unigine::WidgetCheckBoxPtr SampleDescriptionWindow::addBoolParameter(
	const char *name, const char *tooltip, bool default_value,
	std::function<void(bool)> on_change)
{
	if (!w_parameters_grid)
		init_parameter_box();

	auto label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	auto checkbox = WidgetCheckBox::create();
	checkbox->setToolTip(tooltip);
	checkbox->setChecked(default_value);

	checkbox->getEventChanged().connect(*this, [this,
		checkbox,
		callback = std::move(on_change)]
		(const WidgetPtr &widget) {
		if (checkbox->isChecked())
			callback(true);
		else
			callback(false);
	});

	w_parameters_grid->addChild(label, Gui::ALIGN_LEFT);
	w_parameters_grid->addChild(checkbox, Gui::ALIGN_CENTER);
	w_parameters_grid->addChild(WidgetLabel::create(), Gui::ALIGN_LEFT);
	return checkbox;
}

void SampleDescriptionWindow::setStatus(const char *status)
{
	if (!w_status_lbl)
		init_status_box();

	w_status_lbl->setText(status);
	w_status_lbl->arrange();
}

const WidgetGroupBoxPtr &SampleDescriptionWindow::getParameterGroupBox()
{
	if (!w_parameters_grid)
		init_parameter_box();
	return w_parameters_group;
}

void SampleDescriptionWindow::init_parameter_box()
{
	w_parameters_group = WidgetGroupBox::create("Parameters", 8, 8);
	w_main_window->addChild(w_parameters_group, Gui::ALIGN_LEFT);
	w_parameters_grid = WidgetGridBox::create(3);
	w_parameters_group->addChild(w_parameters_grid);
}

void SampleDescriptionWindow::init_status_box()
{
	w_status_group = WidgetGroupBox::create("Status", 8, 8);
	w_main_window->addChild(w_status_group, Gui::ALIGN_LEFT);
	w_status_lbl = WidgetLabel::create();
	// w_status_lbl->setFontWrap(1);
	// w_status_lbl->setFontRich(1);
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

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineString.h>
#include <UnigineWidgets.h>

class SimpleInformationBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleInformationBox, Unigine::ComponentBase);

	COMPONENT_INIT(init, -1);
	COMPONENT_SHUTDOWN(shutdown);

	enum class INFO_ALIGN
	{
		LEFT = 0,
		CENTER = 1,
		RIGHT = 2,
		JUSTIFY = 3
	};

	Unigine::WidgetPtr getColumn(int index);

	void setWindowTitle(const char* title) { window->setText(title); }
	const char* getTitle() const { return window->getText(); }
	void setColumnsCount(int count);
	int getColumnsCount() { return main_gridbox->getNumColumns(); }
	void setAboutInfo(const char* str);
	const char* getAboutInfo() const { return about_label->getText(); }
	void setParametersInfo(int index, const char* str);
	const char* getParametersInfo(int index) const { if (index < parameters_labels.size()) return parameters_labels[index]->getText(); else return nullptr; }
	void setWidth(int width);
	int getWidth() const { return width_in_pixels; }

	Unigine::WidgetComboBoxPtr addCombobox(int index, const char* name, const char* tooltip = nullptr);
	Unigine::WidgetSliderPtr addSlider(int index, const char* name, float multiplier = 1.0f, const char* tooltip = nullptr);

	void clearAboutInfo() { about_label->setText(""); }
	void clearParametersInfo(int index) { if (index < parameters_labels.size())parameters_labels[index]->setText(""); }

	void pushBackAboutInfo(const char* str, INFO_ALIGN align = INFO_ALIGN::LEFT);
	void pushBackAboutInfo(const char* str1, const char* str2, INFO_ALIGN align = INFO_ALIGN::LEFT);
	void pushBackParametersInfo(int index, const char* str, INFO_ALIGN align = INFO_ALIGN::LEFT);
	void pushBackParametersInfo(int index, const char* str1, const char* str2, INFO_ALIGN align = INFO_ALIGN::LEFT);

	void pushBackWhiteSpaceLineAboutInfo();
	void pushBackWhiteSpaceLineParametersInfo(int index);

	void showAdditionalWidgets(int index, bool value);

	const Unigine::WidgetGroupBoxPtr &getParametersGroupBox(int index) const { return parameters_groupboxes[index]; }

private:
	void init();
	void shutdown();

	Unigine::WidgetWindowPtr window;
	Unigine::WidgetVBoxPtr main_background;
	Unigine::WidgetGridBoxPtr main_gridbox;
	Unigine::WidgetGroupBoxPtr about_groupbox;
	Unigine::Vector<Unigine::WidgetGroupBoxPtr> parameters_groupboxes;
	Unigine::WidgetLabelPtr about_label;
	Unigine::Vector<Unigine::WidgetLabelPtr> parameters_labels;
	Unigine::Vector<Unigine::WidgetPtr> additional_widgets;

	Unigine::Input::MOUSE_HANDLE previous_handle;

	Unigine::EventConnections widget_connections;

	int width_in_pixels = 1;
};

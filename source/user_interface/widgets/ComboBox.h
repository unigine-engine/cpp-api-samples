#pragma once
#include <UnigineComponentSystem.h>

class ComboBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ComboBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(600, 50));
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetComboBoxPtr widget_combobox;
};

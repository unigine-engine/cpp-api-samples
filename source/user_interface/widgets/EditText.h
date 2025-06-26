#pragma once
#include <UnigineComponentSystem.h>

class EditText : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EditText, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(250, 150));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(150, 100));
	PROP_PARAM(String, text, "Enter text...");
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetEditTextPtr widget_edittext;
};

#pragma once
#include <UnigineComponentSystem.h>

class EditLine : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EditLine, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(750, 50));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(150, 30));
	PROP_PARAM(String, text, "Enter text...");
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetEditLinePtr widget_editline;
};

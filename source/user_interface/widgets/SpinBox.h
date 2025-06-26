#pragma once
#include <UnigineComponentSystem.h>

class SpinBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SpinBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(625, 300));

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetSpinBoxPtr widget_spinbox;
	Unigine::WidgetEditLinePtr widget_editline;
};

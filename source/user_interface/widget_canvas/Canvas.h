#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class Canvas : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Canvas, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	int create_line(int order, float x, float y, float radius, int num, float angle);
	int create_polygon(int order, float x, float y, float radius, int num, float angle);
	int create_text(int order, float x, float y, Unigine::String string);

	Unigine::WidgetCanvasPtr canvas;
};
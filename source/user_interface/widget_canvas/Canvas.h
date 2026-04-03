// WidgetCanvas sample demonstrating 2D vector graphics rendering.
// Lines, filled polygons, and text are drawn procedurally using polar coordinates.
// A 3D perspective transform creates animated rotation effect each frame.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Renders animated 2D vector graphics using WidgetCanvas primitives.
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

	// Creates line strip from polar coordinates with specified vertex count
	int create_line(int order, float x, float y, float radius, int num, float angle);
	// Creates filled polygon from polar coordinates with specified vertex count
	int create_polygon(int order, float x, float y, float radius, int num, float angle);
	// Creates text element at specified screen position
	int create_text(int order, float x, float y, Unigine::String string);

	// Canvas widget for rendering 2D primitives
	Unigine::WidgetCanvasPtr canvas;
};
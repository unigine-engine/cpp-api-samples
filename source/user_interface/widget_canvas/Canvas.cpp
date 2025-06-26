#include "Canvas.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(Canvas);

using namespace Unigine;
using namespace Math;

void Canvas::init()
{
	// get main window
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	// get main window gui
	GuiPtr gui = main_window->getGui();

	// create canvas
	canvas = WidgetCanvas::create(gui);

	canvas->setLineColor(create_line(0, 200.0f, 200.0f, 100.0f, 3, 360.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));
	canvas->setLineColor(create_line(0, 200.0f, 200.0f, 100.0f, 4, 360.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f));
	canvas->setLineColor(create_line(0, 200.0f, 200.0f, 100.0f, 5, 360.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f));

	canvas->setLineColor(create_line(0, 800.0f, 400.0f, 100.0f, 16, 360.0f * 9.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));

	canvas->setPolygonColor(create_polygon(0, 600.0f, 200.0f, 100.0f, 6, 360.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f));
	canvas->setPolygonColor(create_polygon(1, 600.0f, 200.0f, 100.0f, 3, 360.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));

	canvas->setPolygonColor(create_polygon(0, 400.0f, 400.0f, 100.0f, 8, 360.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f));
	canvas->setPolygonColor(create_polygon(1, 400.0f, 400.0f, 100.0f, 4, 360.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f));

	create_text(0, 200.0f - 64.0f, 200.0f - 30.0f, "This is C++ canvas text");

	main_window->addChild(canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
}

void Canvas::update()
{
	GuiPtr gui = WindowManager::getMainWindow()->getGui();

	float fov = 2.0f;
	float time = Game::getTime();
	float x = gui->getWidth() / 2.0f;
	float y = gui->getHeight() / 2.0f;
	canvas->setTransform(translate(vec3(x, y, 0.0f)) * perspective(fov, 1.0f, 0.01f, 100.0f) * rotateY(sinf(time)) * rotateX(cosf(time * 0.5f)) * translate(vec3(-x, -y, -1.0f / tanf(fov * Consts::DEG2RAD * 0.5f))));
}

void Canvas::shutdown()
{
	canvas.deleteLater();
}

int Canvas::create_line(int order, float x, float y, float radius, int num, float angle)
{
	int line = canvas->addLine(order);
	for (int i = 0; i <= num; i++)
	{
		float s = Unigine::Math::sin(angle / num * Consts::DEG2RAD * i) * radius + x;
		float c = Unigine::Math::cos(angle / num * Consts::DEG2RAD * i) * radius + y;
		canvas->addLinePoint(line, vec3(s, c, 0.0f));
	}
	return line;
}

int Canvas::create_polygon(int order, float x, float y, float radius, int num, float angle)
{
	int polygon = canvas->addPolygon(order);
	for (int i = 0; i < num; i++)
	{
		float s = Unigine::Math::sin(angle / num * Consts::DEG2RAD * i) * radius + x;
		float c = Unigine::Math::cos(angle / num * Consts::DEG2RAD * i) * radius + y;
		canvas->addPolygonPoint(polygon, vec3(s, c, 0.0f));
	}
	return polygon;
}

int Canvas::create_text(int order, float x, float y, Unigine::String string)
{
	int text = canvas->addText(order);
	canvas->setTextPosition(text, vec2(x, y));
	canvas->setTextText(text, string.getRaw());
	return text;
}

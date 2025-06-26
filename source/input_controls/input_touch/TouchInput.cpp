#include "TouchInput.h"
#include "../../utils/SimpleInformationBox.h"
#include <UnigineWindowManager.h>
#include <UnigineInput.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(TouchInput);

using namespace Unigine;
using namespace Math;

void TouchInput::init()
{
	info = getComponent<SimpleInformationBox>(node);

	info->setWindowTitle("Touch Input Sample");
	info->setColumnsCount(1);
	info->setWidth(300);
	info->pushBackAboutInfo("This sample demostrates the simple usage of Touch input.");

	canvas = new CanvasWithCircles();

	Game::getPlayer()->setControlled(false);
}

void TouchInput::update()
{
	canvas->clear();

	info->clearParametersInfo(0);

	int cnt = 0;
	for (int i = 0; i < Input::NUM_TOUCHES; i++)
	{
		if (Input::isTouchPressed(i))
		{
			ivec2 positionOfTouch = Input::getTouchPosition(i) - WindowManager::getMainWindow()->getPosition();

			canvas->addCircle(positionOfTouch.x, positionOfTouch.y, 32, i, "Touch " + String::itoa(i));

			info->pushBackParametersInfo(0, "Touch " + String::itoa(i), SimpleInformationBox::INFO_ALIGN::CENTER);
			info->pushBackParametersInfo(0, "X", String::itoa(positionOfTouch.x));
			info->pushBackParametersInfo(0, "Y", String::itoa(positionOfTouch.y));
			cnt++;
		}
	}

	if (cnt == 0)
		info->pushBackParametersInfo(0, "Here will be displayed information about the positions of the touches", SimpleInformationBox::INFO_ALIGN::CENTER);
}

void TouchInput::shutdown()
{
	if (canvas)
		delete canvas;

	Game::getPlayer()->setControlled(true);
}

TouchInput::CanvasWithCircles::CanvasWithCircles()
{
	canvas = WidgetCanvas::create();
	WindowManager::getMainWindow()->addChild(canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	radius = 50;
}

TouchInput::CanvasWithCircles::~CanvasWithCircles()
{
	canvas.deleteLater();
}

void TouchInput::CanvasWithCircles::addCircle(int x, int y, int num, int color_num, String str)
{
	int polygon = canvas->addPolygon();
	canvas->setPolygonColor(polygon, colors[color_num % number_of_colors]);
	for (int i = 0; i < num; i++)
	{
		float s = Math::sin(Consts::PI2 * i / num) * radius + x;
		float c = Math::cos(Consts::PI2 * i / num) * radius + y;
		canvas->addPolygonPoint(polygon, vec3(s, c, 0.0f));
	}

	int text = canvas->addText();
	canvas->setTextText(text, str);
	canvas->setTextSize(text, 16);
	canvas->setTextColor(text, colors[color_num % number_of_colors]);
	canvas->setTextPosition(text, vec2(x - str.size()*4.0f, y - radius*1.5f));
}

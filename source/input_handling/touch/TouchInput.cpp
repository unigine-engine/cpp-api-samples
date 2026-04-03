// Displays active touch points on screen using WidgetCanvas. Each touch is rendered
// as a colored circle at its position. Supports multi-touch with up to NUM_TOUCHES
// simultaneous inputs. Touch positions are adjusted relative to window coordinates.

#include "TouchInput.h"
#include "../../utils/SimpleInformationBox.h"
#include <UnigineWindowManager.h>
#include <UnigineInput.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(TouchInput);

using namespace Unigine;
using namespace Math;

// Info panel and canvas overlay are initialized; player control is disabled.
void TouchInput::init()
{
	info = getComponent<SimpleInformationBox>(node);

	info->setWindowTitle("Touch Input Sample");
	info->setColumnsCount(1);
	info->setWidth(300);
	info->pushBackAboutInfo("This sample demostrates the simple usage of Touch input.");

	// Canvas overlay is created for rendering touch circles
	canvas = new CanvasWithCircles();

	// Player control is disabled to prevent camera rotation from touch input
	Game::getPlayer()->setControlled(false);
}

// Active touch points are polled and visualized as circles on the canvas.
void TouchInput::update()
{
	canvas->clear();

	info->clearParametersInfo(0);

	// All possible touch slots are checked for active touches
	int cnt = 0;
	for (int i = 0; i < Input::NUM_TOUCHES; i++)
	{
		if (Input::isTouchPressed(i))
		{
			// Touch position is converted to window-relative coordinates
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

// Canvas is deleted and player control is restored.
void TouchInput::shutdown()
{
	if (canvas)
		delete canvas;

	Game::getPlayer()->setControlled(true);
}

// Canvas widget is created and attached to the main window as background overlay.
TouchInput::CanvasWithCircles::CanvasWithCircles()
{
	canvas = WidgetCanvas::create();
	WindowManager::getMainWindow()->addChild(canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	radius = 50;
}

// Canvas widget is scheduled for deletion.
TouchInput::CanvasWithCircles::~CanvasWithCircles()
{
	canvas.deleteLater();
}

// Colored circle polygon with label is added at the specified position.
void TouchInput::CanvasWithCircles::addCircle(int x, int y, int num, int color_num, String str)
{
	// Circle is drawn as a polygon with 'num' vertices
	int polygon = canvas->addPolygon();
	canvas->setPolygonColor(polygon, colors[color_num % number_of_colors]);
	for (int i = 0; i < num; i++)
	{
		float s = Math::sin(Consts::PI2 * i / num) * radius + x;
		float c = Math::cos(Consts::PI2 * i / num) * radius + y;
		canvas->addPolygonPoint(polygon, vec3(s, c, 0.0f));
	}

	// Text label is positioned above the circle
	int text = canvas->addText();
	canvas->setTextText(text, str);
	canvas->setTextSize(text, 16);
	canvas->setTextColor(text, colors[color_num % number_of_colors]);
	canvas->setTextPosition(text, vec2(x - str.size()*4.0f, y - radius*1.5f));
}

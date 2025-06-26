#include "VisualizerSample.h"
#include <UnigineVisualizer.h>

using namespace Unigine;

REGISTER_COMPONENT(VisualizerSample)

void VisualizerSample::init()
{
	visualizer_usage = getComponent<VisualizerUsage>(visualizer_usage_node);
	if (!visualizer_usage)
	{
		Log::error("VisualizerSample::init() can't find VisualizerUsage on visualizer usage node: \n");
	}

	window.createWindow();

	WidgetGroupBoxPtr params = window.getParameterGroupBox();
	WidgetPtr grid = params->getChild(0);

	//========== Enable visualizer checkbox =========//
	WidgetCheckBoxPtr visualizer_check_box = WidgetCheckBox::create("Enable visualizer");
	params->addChild(visualizer_check_box, Gui::ALIGN_LEFT);
	visualizer_check_box->getEventChanged().connect(*this, [this, visualizer_check_box]() {
		Visualizer::setEnabled(visualizer_check_box->isChecked());
		});
	visualizer_check_box->setChecked(true);

	//========== Enable visualizer checkbox =========//
	WidgetCheckBoxPtr depth_test_check_box = WidgetCheckBox::create("Enable depth test");
	params->addChild(depth_test_check_box, Gui::ALIGN_LEFT);
	depth_test_check_box->getEventChanged().connect(*this, [this, depth_test_check_box]() {
		if (depth_test_check_box->isChecked())
		{
			Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
		}
		else {
			Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
		}
		});
	depth_test_check_box->setChecked(true);

	//========== Enable point2D checkbox =========//
	WidgetCheckBoxPtr point2D_check_box = WidgetCheckBox::create("Point2D");
	params->addChild(point2D_check_box, Gui::ALIGN_LEFT);
	point2D_check_box->getEventChanged().connect(*this, [this, point2D_check_box]() {
		visualizer_usage->renderPoint2D = point2D_check_box->isChecked();
		});
	point2D_check_box->setChecked(visualizer_usage->renderPoint2D);

	//========== Enable line2D checkbox =========//
	WidgetCheckBoxPtr line2D_check_box = WidgetCheckBox::create("Line2D");
	params->addChild(line2D_check_box, Gui::ALIGN_LEFT);
	line2D_check_box->getEventChanged().connect(*this, [this, line2D_check_box]() {
		visualizer_usage->renderLine2D = line2D_check_box->isChecked();
		});
	line2D_check_box->setChecked(visualizer_usage->renderPoint2D);

	//========== Enable triangle2D checkbox =========//
	WidgetCheckBoxPtr triangle2D_check_box = WidgetCheckBox::create("Triangle2D");
	params->addChild(triangle2D_check_box, Gui::ALIGN_LEFT);
	triangle2D_check_box->getEventChanged().connect(*this, [this, triangle2D_check_box]() {
		visualizer_usage->renderTriangle2D = triangle2D_check_box->isChecked();
		});
	triangle2D_check_box->setChecked(visualizer_usage->renderTriangle2D);

	//========== Enable quad2D checkbox =========//
	WidgetCheckBoxPtr quad2D_check_box = WidgetCheckBox::create("Quad2D");
	params->addChild(quad2D_check_box, Gui::ALIGN_LEFT);
	quad2D_check_box->getEventChanged().connect(*this, [this, quad2D_check_box]() {
		visualizer_usage->renderQuad2D = quad2D_check_box->isChecked();
		});
	quad2D_check_box->setChecked(visualizer_usage->renderQuad2D);

	//========== Enable rectangle checkbox =========//
	WidgetCheckBoxPtr rectangle_check_box = WidgetCheckBox::create("Rectangle");
	params->addChild(rectangle_check_box, Gui::ALIGN_LEFT);
	rectangle_check_box->getEventChanged().connect(*this, [this, rectangle_check_box]() {
		visualizer_usage->renderRectangle = rectangle_check_box->isChecked();
		});
	rectangle_check_box->setChecked(visualizer_usage->renderRectangle);

	//========== Enable message2D checkbox =========//
	WidgetCheckBoxPtr message2D_check_box = WidgetCheckBox::create("Message2D");
	params->addChild(message2D_check_box, Gui::ALIGN_LEFT);
	message2D_check_box->getEventChanged().connect(*this, [this, message2D_check_box]() {
		visualizer_usage->renderMessage2D = message2D_check_box->isChecked();
		});
	message2D_check_box->setChecked(visualizer_usage->renderMessage2D);
}

void VisualizerSample::shutdown()
{
	Visualizer::setEnabled(false);
	window.shutdown();
}

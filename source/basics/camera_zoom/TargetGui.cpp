#include "TargetGui.h"
#include <UnigineGame.h>


REGISTER_COMPONENT(TargetGui);

using namespace Unigine;
using namespace Math;

void TargetGui::update()
{
	float distance = (node->getWorldPosition() - Game::getPlayer()->getWorldPosition()).length();
	distance_label->setText("Distance to label : " + String::ftoa(distance, 2) + " units");
}

void TargetGui::init()
{
	object_gui = checked_ptr_cast<ObjectGui>(node);
	if (!object_gui)
	{
		Log::error("TargetGui::init cannot find ObjectGui for node!\n");
	}
	gui = object_gui->getGui();

	WidgetWindowPtr window = WidgetWindow::create();
	gui->addChild(window, Gui::ALIGN_CENTER);

	WidgetVBoxPtr vbox = WidgetVBox::create();
	vbox->setSpace(1, 3);
	window->addChild(vbox, Gui::ALIGN_CENTER);
	window->setWidth(gui->getWidth());
	window->setHeight(gui->getHeight());


	WidgetLabelPtr target_label = WidgetLabel::create(node->getName());
	target_label->setFontSize(50);
	vbox->addChild(target_label);

	float distance = (node->getWorldPosition() - Game::getPlayer()->getWorldPosition()).length();
	distance_label = WidgetLabel::create("Distance to label : " + String::ftoa(distance, 2) + " units");
	distance_label->setFontSize(50);
	vbox->addChild(distance_label);

	WidgetLabelPtr size_label = WidgetLabel::create("Size of label : " +
		String::ftoa(object_gui->getPhysicalWidth()) + "x" + String::ftoa(object_gui->getPhysicalHeight(), 2));
	size_label->setFontSize(50);
	vbox->addChild(size_label);
}

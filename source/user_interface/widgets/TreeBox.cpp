// Demonstrates WidgetTreeBox for hierarchical item display. Parent-child
// relationships are established via addItemChild, creating expandable tree
// nodes. Selection changes trigger EventChanged callback for console output.

#include "TreeBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(TreeBox);

using namespace Unigine;
using namespace Math;

// Tree box widget is created with hierarchical parent-child items and selection callback.
void TreeBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_treebox = WidgetTreeBox::create(gui);

	gui->addChild(widget_treebox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_treebox->setPosition(position.get().x, position.get().y);
	widget_treebox->setFontSize(font_size.get());
	widget_treebox->setFontOutline(1);

	// Add first parent node and its children
	widget_treebox->addItem("parent 0");
	widget_treebox->addItem("child 0");
	widget_treebox->addItem("child 1");
	widget_treebox->addItem("child 2");
	// Establish parent-child relationships by index
	widget_treebox->addItemChild(0, 1);
	widget_treebox->addItemChild(0, 2);
	widget_treebox->addItemChild(0, 3);

	// Add second parent node and its children
	widget_treebox->addItem("parent 1");
	widget_treebox->addItem("child 0");
	widget_treebox->addItem("child 1");
	widget_treebox->addItem("child 2");
	widget_treebox->addItemChild(4, 5);
	widget_treebox->addItemChild(4, 6);
	widget_treebox->addItemChild(4, 7);

	// Lambda callback displays selected item text on change
	widget_treebox->getEventChanged().connect(*this, [this]() {
		String msg = String("TreeBox: ") + widget_treebox->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void TreeBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_treebox);

		widget_treebox.deleteLater();
	}

	Console::setOnscreen(false);
}

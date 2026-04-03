// Demo controller that manages multiple easing algorithms for cat movement.
// Each child node contains a different MotionMode component (Linear, EaseIn,
// EaseOut, Spring variants). User can switch between modes via dropdown.

#include "CatDemo.h"

#include "MotionMode.h"
#include "SpringRegular.h"

#include <UnigineWidgets.h>

REGISTER_COMPONENT(CatDemo);

using namespace Unigine;
using namespace Math;

// Demo mode GUI elements are created and added to the sample window.
void CatDemo::initGUI(const WidgetWindowPtr &window)
{
	demo_box = WidgetGroupBox::create("Demo", 8, 4);
	auto demo_label = WidgetLabel::create("Choose animation type for the cat:");
	demo_box->addChild(demo_label, Gui::ALIGN_LEFT);

	// Cat movement modes are added to the demo combobox
	for (const auto &c : cat_components)
		demo_combo->addItem(c->getNode()->getName());
	demo_combo->setCurrentItem(mode);
	demo_combo->getEventChanged().connect(this, &CatDemo::update_cat_mode);
	demo_combo->setEnabled(0);
	demo_combo->arrange();
	demo_box->addChild(demo_combo, Gui::ALIGN_LEFT);

	window->addChild(demo_box, Gui::ALIGN_LEFT);
}

// MotionMode components are collected from child nodes and wired up with target references.
void CatDemo::init()
{
	if (!laserNode)
		Log::error("CatGame::init(): cannot find laserNode!\n");
	if (!catNode)
		Log::error("CatGame::init(): cannot find catNode!\n");

	if (!demo_combo)
		demo_combo = WidgetComboBox::create();

	// Collect all MotionMode components from child nodes.
	// Each child node represents a different easing algorithm
	// (Linear, EaseIn, EaseOut, Spring variants, etc.)
	int num_demo_modes = node->getNumChildren();
	cat_components.allocate(num_demo_modes);

	for (int i = 0; i < num_demo_modes; i++)
	{
		NodePtr mode = node->getChild(i);
		auto comp = ComponentSystem::get()->getComponent<MotionMode>(mode);
		if (!comp)
			continue;
		// Wire up target and persecutor references for each motion mode
		comp->targetNode = laserNode;
		comp->persecutorNode = catNode;
		comp->setEnabled(false);
		cat_components.emplace_back(comp);
	}
}

// Called when the demo mode is activated. GUI is enabled and current motion mode is applied.
void CatDemo::on_enable()
{
	demo_combo->setEnabled(1);
	update_cat_mode();
	demo_box->setHidden(false);
}

// Called when the demo mode is deactivated. Current motion mode and GUI are disabled.
void CatDemo::on_disable()
{
	cat_components[mode]->setEnabled(0);
	demo_combo->setEnabled(0);
	demo_box->setHidden(true);
}

// Reads the selected mode from the combobox and switches to it.
void CatDemo::update_cat_mode()
{
	auto new_mode = demo_combo->getCurrentItem();
	switch_cat_mode(new_mode);
}

// Disables the current motion mode and enables the new one.
void CatDemo::switch_cat_mode(int new_mode)
{
	cat_components[mode]->setEnabled(false);
	mode = new_mode;
	cat_components[mode]->setEnabled(true);
}

void CatDemo::shutdown()
{
	shutdown_gui();
}

// GUI widgets are released via deleteLater for safe deferred destruction.
void CatDemo::shutdown_gui()
{
	demo_box.deleteLater();
	demo_combo.deleteLater();
}

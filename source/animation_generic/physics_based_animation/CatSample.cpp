// Main controller for the cat animation sample. Manages two modes: Demo mode
// (showcases different easing algorithms) and Game mode (interactive catch game).
// Toggle buttons allow switching between modes via the sample description window.

#include "CatSample.h"

#include "CatDemo.h"
#include "CatGame.h"

#include <UnigineWidgets.h>
#include <UnigineWindowManager.h>

#include "../../utils/Utils.h"

REGISTER_COMPONENT(CatSample);

using namespace Unigine;
using namespace Math;

// Sample is initialized: mouse handle is configured and mode controllers are obtained.
void CatSample::init()
{
	// Keep mouse cursor visible (SOFT mode shows cursor, USER mode hides it)
	mouse_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// Get references to the demo and game mode controllers
	game_manager = ComponentSystem::get()->getComponent<CatGame>(gameNode.get());
	if (!game_manager)
		Log::error("CatSample::init(): cannot find CatGame component!\n");

	demo_manager = ComponentSystem::get()->getComponent<CatDemo>(demoNode.get());
	if (!demo_manager)
		Log::error("CatSample::init(): cannot find CatDemo component!\n");

	init_gui();
	// Start with both modes disabled, then activate demo mode
	demo_manager->setEnabled(0);
	game_manager->setEnabled(0);

	demo_button->setToggled(true);
}

// GUI is created: description window, mode toggle buttons, and child GUIs are initialized.
void CatSample::init_gui()
{
	title_label = WidgetLabel::create("Move the pointer away from the cat");
	title_label->setTextAlign(Gui::ALIGN_CENTER);
	title_label->setFontSize(40);
	title_label->setFontOutline(1);
	Gui::getCurrent()->addChild(title_label, Gui::ALIGN_EXPAND);

	sample_description_window.createWindow();

	auto vbox = sample_description_window.getParameterGroupBox();

	auto hbox = WidgetHBox::create(5);
	vbox->addChild(hbox, Gui::ALIGN_BOTTOM);
	game_button = WidgetButton::create("Start Game");
	game_button->setToggleable(1);
	game_button->getEventClicked().connect(this, &CatSample::switch_to_game);
	hbox->addChild(game_button, Gui::ALIGN_LEFT);
	game_manager->initGUI(sample_description_window.getWindow());

	demo_button = WidgetButton::create("Animations Demo");
	demo_button->getEventClicked().connect(this, &CatSample::switch_to_demo);
	demo_button->setToggleable(1);
	hbox->addChild(demo_button, Gui::ALIGN_LEFT);
	demo_manager->initGUI(sample_description_window.getWindow());
}

// Demo mode is disabled and game mode is activated.
void CatSample::switch_to_game()
{
	demo_manager->setEnabled(0);
	game_manager->setEnabled(1);

	// MUTE_EVENT temporarily disconnects the event to prevent recursive callback
	// when programmatically setting the toggle state
	MUTE_EVENT(demo_button->getEventClicked());
	demo_button->setToggled(0);
}

// Game mode is disabled and demo mode is activated.
void CatSample::switch_to_demo()
{
	game_manager->setEnabled(0);
	demo_manager->setEnabled(1);

	// Mute event to avoid triggering switch_to_game when untoggling
	MUTE_EVENT(game_button->getEventClicked());
	game_button->setToggled(0);
}

// Mouse handle is restored and GUI resources are released.
void CatSample::shutdown()
{
	Input::setMouseHandle(mouse_handle);
	shutdown_gui();
}

// GUI widgets are released via deleteLater for safe deferred destruction.
void CatSample::shutdown_gui()
{
	title_label.deleteLater();
	demo_button.deleteLater();
	game_button.deleteLater();
	sample_description_window.shutdown();
}

#include "CatSample.h"

#include "CatDemo.h"
#include "CatGame.h"

#include <UnigineWidgets.h>
#include <UnigineWindowManager.h>

#include "../../utils/Utils.h"

REGISTER_COMPONENT(CatSample);

using namespace Unigine;
using namespace Math;

void CatSample::init()
{
	// don't hide mouse cursor when we click on the screen
	mouse_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	game_manager = ComponentSystem::get()->getComponent<CatGame>(gameNode.get());
	if (!game_manager)
		Log::error("CatSample::init(): cannot find CatGame component!\n");

	demo_manager = ComponentSystem::get()->getComponent<CatDemo>(demoNode.get());
	if (!demo_manager)
		Log::error("CatSample::init(): cannot find CatDemo component!\n");

	init_gui();
	demo_manager->setEnabled(0);
	game_manager->setEnabled(0);
}

void CatSample::init_gui()
{
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

void CatSample::switch_to_game()
{
	demo_manager->setEnabled(0);
	game_manager->setEnabled(1);

	// disable demo_button callback while setting its 'toggled' state
	MUTE_EVENT(demo_button->getEventClicked());
	demo_button->setToggled(0);
}

void CatSample::switch_to_demo()
{
	game_manager->setEnabled(0);
	demo_manager->setEnabled(1);

	// disable game_button callback while setting its 'toggled' state
	MUTE_EVENT(game_button->getEventClicked());
	game_button->setToggled(0);
}

void CatSample::shutdown()
{
	Input::setMouseHandle(mouse_handle);
	shutdown_gui();
}

void CatSample::shutdown_gui()
{
	demo_button.deleteLater();
	game_button.deleteLater();
	sample_description_window.shutdown();
}

// Implements a simple game where player must keep laser pointer away from the cat.
// Difficulty increases over time by ramping up spring stiffness and damping, making
// the cat faster and more responsive. Game ends when cat catches the laser.

#include "CatGame.h"

#include "Laser.h"
#include "SpringRegular.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(CatGame);

using namespace Unigine;
using namespace Math;

// Game mode GUI elements are created and added to the sample window.
void CatGame::initGUI(const Unigine::WidgetWindowPtr &window)
{
	game_box = WidgetGroupBox::create("Game", 8, 4);
	game_label = WidgetLabel::create(
		String::format("Don't let the cat catch the laser pointer!\nTime: %.2f", timer));
	game_label->setFontVSpacing(4);
	game_box->addChild(game_label, Gui::ALIGN_LEFT);
	window->addChild(game_box, Gui::ALIGN_LEFT);
}

// References to Laser and SpringRegular components are obtained from property nodes.
void CatGame::init()
{
	laser_component = ComponentSystem::get()->getComponent<Laser>(laserNode.get());
	if (!laser_component)
		Log::error("CatGame::init(): cannot find Laser component!\n");
	cat_mode = ComponentSystem::get()->getComponent<SpringRegular>(catMode.get());
	if (!cat_mode)
		Log::error("CatGame::init(): cannot find SpringRegular component!\n");
}

// Called when game mode is activated. Game state is reset and cat motion is enabled.
void CatGame::on_enable()
{
	catched = false;
	cat_mode->setEnabled(1);
	timer = 0;
	game_box->setHidden(false);
}

// Called when game mode is deactivated. Cat motion is disabled and laser control is restored.
void CatGame::on_disable()
{
	cat_mode->setEnabled(0);
	laser_component->setEnabled(1);
	game_box->setHidden(true);
}

// Game state is updated: either active gameplay or game over screen is processed.
void CatGame::update()
{
	catched ? update_gameover() : update_game();
}

// Active gameplay is processed: difficulty increases and catch condition is checked.
void CatGame::update_game()
{
	timer += Game::getIFps();

	// Increase difficulty over time: cat becomes faster and more responsive.
	// Stiffness controls how quickly the spring snaps to target.
	// Damping controls oscillation reduction (higher = less bouncy).
	cat_mode->setStiffness(cat_mode->getStiffness() + Game::getIFps() * 2.0f);
	cat_mode->setDamping(cat_mode->getDamping() + Game::getIFps() * 0.25f);

	// Check if cat has caught the laser (spring finished oscillating at target)
	catched = cat_mode->getFinished();
	if (catched)
		laser_component->setEnabled(0);

	game_label->setText(
		String::format("Don't let the cat catch the laser pointer!\nTime: %.2f", timer));
}

// Game over state is processed. Restart is available via Enter key.
void CatGame::update_gameover()
{
	// Game is restarted when Enter key is pressed
	if (Input::isKeyDown(Input::KEY_ENTER))
	{
		catched = false;
		timer = 0;
		cat_mode->RefreshSpring();
		laser_component->setEnabled(1);
	}

	game_label->setText(String::format("GAME OVER! Press Enter to restart\nTime: %.2f", timer));
}

void CatGame::shutdown()
{
	shutdown_gui();
}

// GUI widgets are released via deleteLater for safe deferred destruction.
void CatGame::shutdown_gui()
{
	game_label.deleteLater();
	game_box.deleteLater();
}
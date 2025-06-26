#include "CatGame.h"

#include "Laser.h"
#include "SpringRegular.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(CatGame);

using namespace Unigine;
using namespace Math;

void CatGame::initGUI(const Unigine::WidgetWindowPtr &window)
{
	game_box = WidgetGroupBox::create("Game", 8, 4);
	game_label = WidgetLabel::create(
		String::format("Don't let the cat catch the laser pointer!\nTime: %.2f", timer));
	game_label->setFontVSpacing(4);
	game_box->addChild(game_label, Gui::ALIGN_LEFT);
	window->addChild(game_box, Gui::ALIGN_LEFT);
}

void CatGame::init()
{
	laser_component = ComponentSystem::get()->getComponent<Laser>(laserNode.get());
	if (!laser_component)
		Log::error("CatGame::init(): cannot find Laser component!\n");
	cat_mode = ComponentSystem::get()->getComponent<SpringRegular>(catMode.get());
	if (!cat_mode)
		Log::error("CatGame::init(): cannot find SpringRegular component!\n");
}

void CatGame::on_enable()
{
	catched = false;
	cat_mode->setEnabled(1);
	timer = 0;
	game_box->setHidden(false);
}

void CatGame::on_disable()
{
	cat_mode->setEnabled(0);
	laser_component->setEnabled(1);
	game_box->setHidden(true);
}

void CatGame::update()
{
	catched ? update_gameover() : update_game();
}

void CatGame::update_game()
{
	timer += Game::getIFps();

	// change cat's movement parametrs with time
	cat_mode->setStiffness(cat_mode->getStiffness() + Game::getIFps() * 2.0f);
	cat_mode->setDamping(cat_mode->getDamping() + Game::getIFps() * 0.25f);

	catched = cat_mode->getFinished();
	if (catched)
		laser_component->setEnabled(0);

	game_label->setText(
		String::format("Don't let the cat catch the laser pointer!\nTime: %.2f", timer));
}

void CatGame::update_gameover()
{
	// restart game
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

void CatGame::shutdown_gui()
{
	game_label.deleteLater();
	game_box.deleteLater();
}
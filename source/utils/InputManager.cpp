#include "InputManager.h"
#include <UnigineGame.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(InputManager)

void InputManager::update()
{
	if (Console::isActive())
		return;

	int positive = 0;
	int negative = 0;
	
	positive = Input::isKeyPressed(static_cast<Input::KEY>(key_forward));
	negative = Input::isKeyPressed(static_cast<Input::KEY>(key_backward));
	trottle = static_cast<float>(positive - negative);

	positive = Input::isKeyPressed(static_cast<Input::KEY>(key_right));
	negative = Input::isKeyPressed(static_cast<Input::KEY>(key_left));
	turn = static_cast<float>(positive - negative);
}

#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>


class InputManager : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(InputManager, ComponentBase)
	COMPONENT_UPDATE(update)

	UNIGINE_INLINE float getThrottle() const { return trottle; };
	UNIGINE_INLINE float getTurn() const { return turn; }

private:
	Unigine::Input::KEY key_forward = Unigine::Input::KEY_W;
	Unigine::Input::KEY key_backward = Unigine::Input::KEY_S;
	Unigine::Input::KEY key_right = Unigine::Input::KEY_D;
	Unigine::Input::KEY key_left = Unigine::Input::KEY_A;

	float trottle = 0.f;
	float turn = 0.f;

private:
	void update();
};

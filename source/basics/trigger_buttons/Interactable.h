#pragma once
#include <UnigineGame.h>

class Interactable
{
public:
	static const int interactable_update_priority;


	virtual void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) = 0;
	virtual float getValue() = 0;
};
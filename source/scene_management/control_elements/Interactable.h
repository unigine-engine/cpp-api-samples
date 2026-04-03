// Base interface for interactive control elements (buttons, levers, valves).
// Provides common interaction and value retrieval methods.
// Derived classes implement specific control behavior.

#pragma once
#include <UnigineGame.h>

// Abstract interface for player-interactable objects in the scene.
class Interactable
{
public:
	// Update priority ensures interactables update before reactors
	static const int interactable_update_priority;

	// Called each frame while player holds interaction key
	virtual void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) = 0;
	// Returns normalized state value (0-1) for reactors to read
	virtual float getValue() = 0;
};
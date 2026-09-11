#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>

// Smoothly transitions metalness between two values in a ping-pong pattern.
class MaterialsMetalness : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialsMetalness, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Starting metalness value (0 = non-metallic)
	PROP_PARAM(Float, beginMetalness, 0.0f);
	// Target metalness value (1 = fully metallic)
	PROP_PARAM(Float, endMetalness, 1.0f);
	// Duration of one transition cycle in seconds
	PROP_PARAM(Float, time, 5.0f);

private:
	void init();
	void update();

private:
	// Cached reference to the object's material
	Unigine::MaterialPtr material;
	// Current position in the animation timeline
	float current_time = 0.0f;
	// Animation direction (1 = toward end, -1 = toward begin)
	float time_sign = 1.0f;
};

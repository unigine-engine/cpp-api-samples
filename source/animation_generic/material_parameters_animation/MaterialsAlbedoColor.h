#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>

// Animates albedo color smoothly between two colors in a ping-pong pattern.
class MaterialsAlbedoColor : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialsAlbedoColor, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Starting color (shown at time = 0)
	PROP_PARAM(Color, beginColor, Unigine::Math::vec4_zero);
	// Target color (reached at time = duration)
	PROP_PARAM(Color, endColor, Unigine::Math::vec4_one);
	// Duration of one color transition in seconds
	PROP_PARAM(Float, time, 5.0f);

private:
	void init();
	void update();

private:
	// Cached reference to the object's material
	Unigine::MaterialPtr material;
	// Current position in the animation timeline
	float current_time = 0.0f;
	// Animation direction (1 = toward endColor, -1 = toward beginColor)
	float time_sign = 1.0f;
};

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>

// Periodically toggles shadow casting on/off for the material.
class MaterialsCastWorldShadow : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialsCastWorldShadow, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Interval between shadow toggle in seconds
	PROP_PARAM(Float, time, 5.0f);

private:
	void init();
	void update();

private:
	// Cached reference to the object's material
	Unigine::MaterialPtr material;
	// Tracks elapsed time for shadow state switching
	float current_time = 0.0f;
	// Direction of time accumulation for ping-pong timing
	float time_sign = 1.0f;
};

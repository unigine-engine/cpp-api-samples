#pragma once

#include "SpringMotion.h"

#include <UnigineComponentSystem.h>

// Spring motion with direct control over physics parameters.
// Exposes stiffness and damping as editable properties for fine-tuning.
// Provides runtime setters for dynamic difficulty adjustment (used in game mode).
class SpringRegular : public SpringMotion

{
public:
	COMPONENT_DEFINE(SpringRegular, SpringMotion);

	PROP_PARAM(Float, defaultStiffness, 3.0f, "Stiffness");
	PROP_PARAM(Float, defaultDamping, 0.75f, "Damping");

	void setStiffness(float value) { stiffness = value; }
	float getStiffness() const { return stiffness; }
	void setDamping(float value) { damping = value; }
	float getDamping() const { return damping; }
	bool getFinished() const { return finished; }
	void RefreshSpring();

private:
	void on_disable() override;
	void refresh_spring_settings() override;
};

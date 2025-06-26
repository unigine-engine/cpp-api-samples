#pragma once

#include "SpringMotion.h"

#include <UnigineComponentSystem.h>

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

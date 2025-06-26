#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
class Waves : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Waves, ComponentBase);
	COMPONENT_INIT(init);

	void setBeaufort(float input_beaufort) { water->setBeaufort(input_beaufort); }
	float getBeaufort() const { return water->getBeaufort(); }

private:
	void init();


private:
	Unigine::ObjectWaterGlobalPtr water;
};

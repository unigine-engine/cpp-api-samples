#include "Waves.h"

#include <UnigineInput.h>
#include <UnigineWorld.h>
REGISTER_COMPONENT(Waves);
void Waves::init()
{
	water = Unigine::checked_ptr_cast<Unigine::ObjectWaterGlobal>(
		Unigine::World::getNodeByType(Unigine::Node::TYPE::OBJECT_WATER_GLOBAL));

	water->setBeaufort(static_cast<float>(0.f));
}

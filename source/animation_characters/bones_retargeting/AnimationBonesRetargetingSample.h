#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates animation retargeting between skeletons with different proportions.
// Retargeting now works automatically through shared skeletons assigned on MeshSkinned
// and MeshSkinnedAnimation. To prevent stretching when skeleton proportions differ,
// rotation-only frame masks are applied to most bones. Hip and ground-contact bones
// receive full transforms to maintain correct body height and ground placement.
class AnimationBonesRetargetingSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesRetargetingSample, ComponentBase);
	COMPONENT_INIT(init);

private:
	void init();
};

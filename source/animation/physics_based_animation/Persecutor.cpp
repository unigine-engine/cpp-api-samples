#include "Persecutor.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(PersecutorSkinned);

using namespace Unigine;
using namespace Math;

void PersecutorSkinned::init()
{
	persecutor = static_ptr_cast<ObjectMeshSkinned>(node);
	if (!persecutor)
		Log::error("Persecutor::init(): node is not ObjectMeshSkinned!\n");

	// set persecutor's animation
	persecutor->setNumLayers(2);
	persecutor->setLayerAnimationFilePath(0, persecutorIdleAnim);
	persecutor->setLayerAnimationFilePath(1, persecutorRunAnim);
}

Unigine::Math::Vec3 PersecutorSkinned::getPosition() const
{
	return persecutor->getWorldPosition();
};

Unigine::Math::quat PersecutorSkinned::getRotation() const
{
	return persecutor->getWorldRotation();
};

void PersecutorSkinned::setPosition(const Unigine::Math::Vec3 &new_pos)
{
	persecutor->setWorldPosition(new_pos);
};

void PersecutorSkinned::setRotation(const Unigine::Math::Vec3 &target_dir)
{
	// align forward direction to negative X-axis
	quat target_rot = rotationFromDir(vec3(target_dir), vec3_up) * quat(0, 0, 1, -90.0f);

	// use exponential turning average to set direction
	persecutor->setWorldRotation(slerp(persecutor->getWorldRotation(), target_rot,
		1.0f - Math::exp(-turnRate * Game::getIFps())));
};

void PersecutorSkinned::setAnimation(float persecutor_speed, bool reached_target)
{
	// calculate animation weight for smooth transition
	// animation using simple version of the exponential movement average without "exp" function
	idle_anim_weight = lerp(idle_anim_weight, reached_target ? 1.0f : 0.0f,
		saturate(moveRate * Game::getIFps()));
	persecutor->setLayer(0, 1, idle_anim_weight);
	persecutor->setLayer(1, 1, 1.0f - idle_anim_weight);

	run_anim_time += persecutor_speed * animationSpeed * Game::getIFps();
	persecutor->setLayerFrame(0, 0);
	persecutor->setLayerFrame(1, run_anim_time);
};

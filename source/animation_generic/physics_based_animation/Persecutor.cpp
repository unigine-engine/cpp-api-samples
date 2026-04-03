// Controls the animated cat character (ObjectMeshSkinned) with two-layer animation
// blending between idle and run states. Rotation uses exponential smoothing for
// natural turning, and run animation speed matches actual movement speed.

#include "Persecutor.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(PersecutorSkinned);

using namespace Unigine;
using namespace Math;

// Node is cast to ObjectMeshSkinned and animation layers are configured.
void PersecutorSkinned::init()
{
	persecutor = static_ptr_cast<ObjectMeshSkinnedLegacy>(node);
	if (!persecutor)
		Log::error("Persecutor::init(): node is not ObjectMeshSkinned!\n");

	// Set up two animation layers for blending: idle (layer 0) and run (layer 1).
	// Layers allow smooth transitions between animation states.
	persecutor->setNumLayers(2);
	persecutor->setLayerAnimationFilePath(0, persecutorIdleAnim);
	persecutor->setLayerAnimationFilePath(1, persecutorRunAnim);
}

// World position of the skinned mesh is returned.
Unigine::Math::Vec3 PersecutorSkinned::getPosition() const
{
	return persecutor->getWorldPosition();
}

// World rotation of the skinned mesh is returned.
Unigine::Math::quat PersecutorSkinned::getRotation() const
{
	return persecutor->getWorldRotation();
}

// World position of the skinned mesh is set directly.
void PersecutorSkinned::setPosition(const Unigine::Math::Vec3 &new_pos)
{
	persecutor->setWorldPosition(new_pos);
}

// Rotation is smoothly interpolated toward the target direction.
void PersecutorSkinned::setRotation(const Unigine::Math::Vec3 &target_dir)
{
	// Build rotation from direction vector. Model faces -X, so apply 90 degree correction.
	quat target_rot = rotationFromDir(vec3(target_dir), vec3_up) * quat(0, 0, 1, -90.0f);

	// Exponential smoothing for rotation: creates natural turning motion
	// Higher turnRate = snappier turns, lower = more gradual
	persecutor->setWorldRotation(slerp(persecutor->getWorldRotation(), target_rot,
		1.0f - Math::exp(-turnRate * Game::getIFps())));
}

// Animation layers are blended based on movement state and speed.
void PersecutorSkinned::setAnimation(float persecutor_speed, bool reached_target)
{
	// Blend between idle and run animations based on movement state.
	// Uses exponential moving average for smooth weight transitions.
	idle_anim_weight = lerp(idle_anim_weight, reached_target ? 1.0f : 0.0f,
		saturate(moveRate * Game::getIFps()));
	persecutor->setLayer(0, 1, idle_anim_weight);		// Idle layer weight
	persecutor->setLayer(1, 1, 1.0f - idle_anim_weight);	// Run layer weight (inverse)

	// Advance run animation based on movement speed (speed-matched footsteps)
	run_anim_time += persecutor_speed * animationSpeed * Game::getIFps();
	persecutor->setLayerFrame(0, 0);		// Idle stays at frame 0
	persecutor->setLayerFrame(1, run_anim_time);	// Run advances with movement
}

#pragma once

#include <UnigineComponentSystem.h>

// Abstract interface for the chasing entity (the cat).
// Decouples motion logic from the actual character representation,
// allowing different character types (skinned mesh, simple mesh, etc.).
class PersecutorBase
{
public:
	virtual Unigine::Math::Vec3 getPosition() const = 0;
	virtual Unigine::Math::quat getRotation() const = 0;
	virtual void setPosition(const Unigine::Math::Vec3 &new_pos) = 0;
	virtual void setRotation(const Unigine::Math::Vec3 &target_dir) = 0;
	virtual void setAnimation(float persecutor_speed, bool catched) = 0;
};

// Skinned mesh implementation of the persecutor (animated cat character).
// Handles skeletal animation blending between idle and run states.
class PersecutorSkinned
	: public Unigine::ComponentBase
	, public PersecutorBase
{
public:
	COMPONENT_DEFINE(PersecutorSkinned, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(File, persecutorRunAnim);	// Running animation file
	PROP_PARAM(File, persecutorIdleAnim);	// Idle animation file
	PROP_PARAM(Float, moveRate, 10.f);		// Position interpolation speed
	PROP_PARAM(Float, turnRate, 15.f);		// Rotation interpolation speed
	PROP_PARAM(Float, animationSpeed, 15.0f);

	Unigine::Math::Vec3 getPosition() const override;
	Unigine::Math::quat getRotation() const override;
	void setPosition(const Unigine::Math::Vec3 &new_pos) override;
	void setRotation(const Unigine::Math::Vec3 &target_dir) override;
	void setAnimation(float persecutor_speed, bool catched) override;

private:
	Unigine::ObjectMeshSkinnedLegacyPtr persecutor = nullptr;

	// Animation settings
	float idle_anim_weight = 0.0f;
	float run_anim_time = 0.0f;

private:
	void init();
};

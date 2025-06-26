#pragma once

#include <UnigineComponentSystem.h>

class PersecutorBase
{
public:
	virtual Unigine::Math::Vec3 getPosition() const = 0;
	virtual Unigine::Math::quat getRotation() const = 0;
	virtual void setPosition(const Unigine::Math::Vec3 &new_pos) = 0;
	virtual void setRotation(const Unigine::Math::Vec3 &target_dir) = 0;
	virtual void setAnimation(float persecutor_speed, bool catched) = 0;
};

class PersecutorSkinned
	: public Unigine::ComponentBase
	, public PersecutorBase
{
public:
	COMPONENT_DEFINE(PersecutorSkinned, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(File, persecutorRunAnim);
	PROP_PARAM(File, persecutorIdleAnim);
	PROP_PARAM(Float, moveRate, 10.f);
	PROP_PARAM(Float, turnRate, 15.f);
	PROP_PARAM(Float, animationSpeed, 15.0f);

	Unigine::Math::Vec3 getPosition() const override;
	Unigine::Math::quat getRotation() const override;
	void setPosition(const Unigine::Math::Vec3 &new_pos) override;
	void setRotation(const Unigine::Math::Vec3 &target_dir) override;
	void setAnimation(float persecutor_speed, bool catched) override;

private:
	Unigine::ObjectMeshSkinnedPtr persecutor = nullptr;

	// animation settings
	float idle_anim_weight = 0.0f;
	float run_anim_time = 0.0f;

private:
	void init();
};

#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>

class BodyFractureUnit
	: public Unigine::ComponentBase
{
public:
	enum FRACTURING_MODE
	{
		MODE_SLICING,
		MODE_CRACKING,
		MODE_SHATTERING
	};

	COMPONENT_DEFINE(BodyFractureUnit, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Toggle, debug, true);

	PROP_PARAM(Float, max_impulse, 3.0f);
	PROP_PARAM(Float, threshold, 0.01f);
	PROP_PARAM(Material, material);
	PROP_PARAM(Switch, mode, 0, "Slicing,Cracking,Shattering");

	PROP_PARAM(Int, cracking_num_cuts, 10, "Num cuts", "Cracking num cuts", "Cracking", "mode=1");
	PROP_PARAM(Int, cracking_num_rings, 1, "Num rings", "Cracking num rings", "Cracking", "mode=1");
	PROP_PARAM(Float, cracking_step, 3.0f, "Step size", "Cracking step size", "Cracking", "mode=1");

	PROP_PARAM(Int, shattering_num_pieces, 10, "Num pieces", "Shattering num pieces", "Shattering", "mode=2");

	Unigine::BodyFracturePtr getBody() const;
	bool isBroken();
	void crack(float impulse, const Unigine::Math::Vec3 &point, const Unigine::Math::vec3 &normal);

private:
	void init();
	void update();
	void shutdown();

	void onContactEnter(const Unigine::BodyPtr &body, int num);

	void crack(const Unigine::BodyFracturePtr &fracture, const Unigine::Math::Vec3 &point, const Unigine::Math::vec3 &normal);

private:
	Unigine::BodyFracturePtr own_body;
	bool visualizer_enabled;
};

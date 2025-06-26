#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>
#include <UnigineVector.h>

#include <memory>

class BuoyancyPhysics : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BuoyancyPhysics, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, center_of_mass_node);
	PROP_PARAM(IVec2, volume_grid_size);
	PROP_PARAM(Float, buoyancy);
	PROP_PARAM(Float, water_linear_damping);
	PROP_PARAM(Float, water_angular_damping);
	PROP_PARAM(Toggle, debug);

private:
	struct VolumePart
	{
		Unigine::Math::Vec3 center;
		float water_height;
	};

private:
	void init();
	void update();
	void update_physics();
	void shutdown();

private:
	Unigine::Vector<VolumePart> volume_parts;
	Unigine::ObjectWaterGlobalPtr water;
	Unigine::BodyRigidPtr body_rigid;
	Unigine::ShapeBoxPtr volume_shape_box;

private:
	// volume calculations:
	Unigine::Math::Vec3 size;
	Unigine::Math::mat3 rotation;

	Unigine::Math::Scalar getWidth();
	Unigine::Math::Scalar getDepth();
	Unigine::Math::Scalar getHeight();

	Unigine::Math::Scalar getHalfWidth();
	Unigine::Math::Scalar getHalfDepth();
	Unigine::Math::Scalar getHalfHeight();

	Unigine::Math::Vec3 getAnchorPoint(Unigine::Math::Vec3 center);
	bool isUnderWater(float water_height);
	Unigine::Math::Scalar getWaterVolume(float water_height);
};

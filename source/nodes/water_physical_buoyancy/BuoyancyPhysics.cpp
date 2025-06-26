#include "BuoyancyPhysics.h"

#include <UnigineNode.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

REGISTER_COMPONENT(BuoyancyPhysics);

using namespace Unigine;
using namespace Math;

void BuoyancyPhysics::init()
{
	water = checked_ptr_cast<ObjectWaterGlobal>(
		World::getNodeByType(Node::TYPE::OBJECT_WATER_GLOBAL));
	if (!water)
	{
		Log::error("BuoyComponent::init: can't find ObjectWaterGlobal in world");
	}

	body_rigid = node->getObjectBodyRigid();
	if (!body_rigid)
	{
		Log::error("BuoyComponent::init: body_rigid is null");
	}

	for (int i = 0; i < body_rigid->getNumShapes(); ++i)
	{
		ShapePtr s = body_rigid->getShape(i);

		if (s->getType() == Shape::TYPE::SHAPE_BOX && String::equal(s->getName(), "volume"))
		{
			auto tmp_shape_box = checked_ptr_cast<ShapeBox>(s);
			volume_shape_box = tmp_shape_box;
			break;
		}
	}

	if (!volume_shape_box)
	{
		Log::error("BuoyComponent::init: volume_shape_box is null");
	}

	volume_parts.resize(volume_grid_size.get().x * volume_grid_size.get().y);

	if (center_of_mass_node.get() != nullptr)
	{
		body_rigid->setShapeBased(false);
		body_rigid->setCenterOfMass(static_cast<vec3>(center_of_mass_node->getPosition()));
	}
	else
	{
		Log::warning("BuoyPhysics::init : center of mass node is null");
	}

	if (debug)
	{
		Visualizer::setEnabled(true);
		Visualizer::setMode(Visualizer::MODE::MODE_ENABLED_DEPTH_TEST_DISABLED);
	}
	else
	{
		Visualizer::setEnabled(false);
	}
};

void BuoyancyPhysics::update()
{

	if (water == nullptr || body_rigid == nullptr || volume_shape_box == nullptr)
	{
		return;
	}

	size = Vec3(volume_shape_box->getSize().x / static_cast<float>(volume_grid_size.get().x),
		volume_shape_box->getSize().y / static_cast<float>(volume_grid_size.get().y),
		volume_shape_box->getSize().z);

	rotation = mat3(volume_shape_box->getTransform());

	Mat4 t = volume_shape_box->getTransform();
	Vec3 shape_size = Vec3(volume_shape_box->getSize());

	Vec3 start = t.getTranslate() - t.getAxisX() * shape_size.x * 0.5f
		- t.getAxisY() * shape_size.y * 0.5f;

	for (int i = 0; i < volume_parts.size(); ++i)
	{
		int y = i / volume_grid_size.get().x;
		int x = i % volume_grid_size.get().x;

		volume_parts[i].center = start
			+ static_cast<Vec3>(rotation.getAxisX())
				* (static_cast<float>(x) * size.x + getHalfWidth())
			+ static_cast<Vec3>(rotation.getAxisY())
				* (static_cast<float>(y) * size.y + getHalfDepth());
	}

	for (int i = 0; i < volume_parts.size(); ++i)
	{
		float h = water->fetchHeight(Vec3(getAnchorPoint(volume_parts[i].center).x,
			getAnchorPoint(volume_parts[i].center).y, 0.0f));

		if (getAnchorPoint(volume_parts[i].center).z < h)
		{
			volume_parts[i].water_height
				= Math::clamp(h - static_cast<float>(getAnchorPoint(volume_parts[i].center).z),
					0.0f, static_cast<float>(size.z));
		}
		else
		{
			volume_parts[i].water_height = 0.0f;
		}
	}
	if (debug)
	{
		for (int i = 0; i < volume_parts.size(); ++i)
		{
			Mat4 transform;
			transform.set(rotation, volume_parts[i].center);
			Visualizer::renderPoint3D(getAnchorPoint(volume_parts[i].center), 0.3f, vec4_white);
			Vec3 volume_size = Vec3(size.x, size.y, volume_parts[i].water_height);
			Mat4 volume_transform = Mat4(rotation,
				getAnchorPoint(volume_parts[i].center)
					+ static_cast<Vec3>(rotation.getAxisZ()) * volume_size.z * 0.5f);
			if (isUnderWater(volume_parts[i].water_height))
			{
				Visualizer::renderVector(getAnchorPoint(volume_parts[i].center),
					static_cast<Vec3>(getAnchorPoint(volume_parts[i].center)
						+ static_cast<Vec3>(rotation.getAxisZ()) * volume_size.z),
					vec4_red);
				Visualizer::renderBox(static_cast<vec3>(volume_size), volume_transform,
					vec4(0.0f, 0.0f, 1.f, 1.f));
			}
		}
	}
}

void BuoyancyPhysics::update_physics()
{
	if (!water || !body_rigid || !volume_shape_box)
	{
		return;
	}

	float water_volume = 0.0f;
	float all_volume = volume_shape_box->getSize().x * volume_shape_box->getSize().y
		* volume_shape_box->getSize().z;

	for (int i = 0; i < volume_parts.size(); ++i)
	{
		if (!isUnderWater(volume_parts[i].water_height))
		{
			continue;
		}

		Vec3 force = -static_cast<Vec3>(Unigine::Physics::getGravity())
			* getWaterVolume(volume_parts[i].water_height) * buoyancy;

		force /= static_cast<float>(volume_parts.size());

		body_rigid->addForce(static_cast<vec3>(force));

		Vec3 radius = Vec3(getAnchorPoint(volume_parts[i].center)
			+ static_cast<Vec3>(rotation.getAxisZ()) * volume_parts[i].water_height
			- body_rigid->getWorldCenterOfMass());

		Vec3 torque = cross(radius, force);
		body_rigid->addTorque(static_cast<vec3>(torque));

		water_volume += static_cast<float>(getWaterVolume(volume_parts[i].water_height));
	}

	float coeff = 0.0f;
	if (all_volume > 0.0f)
	{
		coeff = water_volume / all_volume;
	}

	body_rigid->addLinearImpulse(
		(-body_rigid->getLinearVelocity() * water_linear_damping * coeff * body_rigid->getMass()));

	body_rigid->addAngularImpulse(
		(-body_rigid->getAngularVelocity() * water_angular_damping * coeff)
		* inverse(body_rigid->getIWorldInertia()));
}

void BuoyancyPhysics::shutdown()
{
	Visualizer::setEnabled(false);
}

Scalar BuoyancyPhysics::getHalfWidth()
{
	return size.x * static_cast<Scalar>(0.5);
}

Scalar BuoyancyPhysics::getHalfDepth()
{
	return size.y * static_cast<Scalar>(0.5);
}

Scalar BuoyancyPhysics::getHalfHeight()
{
	return size.z * static_cast<Scalar>(0.5);
}

Unigine::Math::Vec3 BuoyancyPhysics::getAnchorPoint(Unigine::Math::Vec3 center)
{
	return center - static_cast<Vec3>(rotation.getAxisZ()) * getHalfHeight();
}

bool BuoyancyPhysics::isUnderWater(float water_height)
{
	return water_height != 0.0f;
}

Scalar BuoyancyPhysics::getWaterVolume(float water_height)
{
	return size.x * size.y * water_height;
}

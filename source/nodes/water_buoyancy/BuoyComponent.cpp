#include "BuoyComponent.h"
#include "BuoySample.h"

#include <UnigineGame.h>
#include <UnigineConsole.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(BuoyComponent);

using namespace Unigine;
using namespace Math;

void BuoyComponent::init()
{
	water = checked_ptr_cast<ObjectWaterGlobal>(World::getNodeByType(Node::OBJECT_WATER_GLOBAL));
	if (!water)
		Log::error("BuoyComponent::init(): can't find ObjectWaterGlobal on scene!\n");
}

void BuoyComponent::update()
{
	float mass_lerp_c = mass * 0.01f;
	if (mass == 0.0f)
		mass_lerp_c = 0.00001;

	if (water)
	{
		auto node_transform = node->getWorldTransform();

		// get water height in entity position
		Vec3 point0 = point_front_center->getWorldPosition();
		Vec3 point1 = point_back_left->getWorldPosition();
		Vec3 point2 = point_back_right->getWorldPosition();

		//create basis by 3 point
		Vec3 tmp_z = normalize(cross(normalize(point1 - point0), normalize(point2 - point0)));
		if (getAngle(vec3(tmp_z), vec3_up) > 90)
			tmp_z = -tmp_z;
		Vec3 tmp_y = normalize(point1 - point0);
		Vec3 tmp_x = tmp_y;
		cross(tmp_x, tmp_y, tmp_z).normalize();
		cross(tmp_y, tmp_z, tmp_x).normalize();
		Mat4 old_basis;
		old_basis.setTranslate(point0);
		old_basis.setColumn3(0, tmp_x);
		old_basis.setColumn3(1, tmp_y);
		old_basis.setColumn3(2, tmp_z);


		// find height of each point
		Scalar h0 = water->fetchHeight(Vec3(point0.x, point0.y, 0.0f));
		Scalar h1 = water->fetchHeight(Vec3(point1.x, point1.y, 0.0f));
		Scalar h2 = water->fetchHeight(Vec3(point2.x, point2.y, 0.0f));

		Scalar lerp_k = Game::getIFps() * Console::getFloat("global_buoyancy") / mass_lerp_c;
		Scalar diff = Math::max(Math::abs((h0 - point0.z) + (h1 - point1.z) + (h2 - point2.z)), Scalar(1.0f));
		lerp_k = clamp(lerp_k * diff, Scalar(0.0f), Scalar(1.0f));

		point0.z = lerp(point0.z, h0, lerp_k);
		point1.z = lerp(point1.z, h1, lerp_k);
		point2.z = lerp(point2.z, h2, lerp_k);

		// calculate new basis for changed point
		tmp_z = normalize(cross(normalize(point1 - point0), normalize(point2 - point0)));
		tmp_y = normalize(point1 - point0);
		cross(tmp_x, tmp_y, tmp_z).normalize();
		cross(tmp_y, tmp_z, tmp_x).normalize();
		Mat4 new_basis;
		new_basis.setTranslate(point0);
		new_basis.setColumn3(0, tmp_x);
		new_basis.setColumn3(1, tmp_y);
		new_basis.setColumn3(2, tmp_z);

		// calculate translation from old_basis to new_basis
		Mat4 translation_basis, new_transform;
		mul(translation_basis, new_basis, inverse(old_basis));

		// apply transformation to node_transform
		mul(new_transform, translation_basis, node_transform);

		node->setWorldTransform(new_transform);
	}
}

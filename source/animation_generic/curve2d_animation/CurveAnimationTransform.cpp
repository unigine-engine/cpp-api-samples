#include "CurveAnimationTransform.h"

#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(CurveAnimationTransform);
REGISTER_COMPONENT(CurveAnimationMaterialParamFloat);

void CurveAnimationTransform::update()
{
	time += Game::getIFps() * speed;

	// Evaluate each position curve at current time
	float x = position->x->evaluate(time);
	float y = position->y->evaluate(time);
	float z = position->z->evaluate(time);

	// Evaluate rotation curves (Euler angles)
	float ang_x = rotation->x->evaluate(time);
	float ang_y = rotation->y->evaluate(time);
	float ang_z = rotation->z->evaluate(time);

	// Scale curves are optional - use 1.0 if no animation is defined
	float scale_x = scale_prop->x->getNumKeys() > 1 ? scale_prop->x->evaluate(time) : 1.0f;
	float scale_y = scale_prop->y->getNumKeys() > 1 ? scale_prop->y->evaluate(time) : 1.0f;
	float scale_z = scale_prop->z->getNumKeys() > 1 ? scale_prop->z->evaluate(time) : 1.0f;

	// Compose final transform matrix: T * R * S
	node->setTransform(Mat4(translate(vec3(x, y, z)) * rotate(quat(ang_x, ang_y, ang_z)) * scale(vec3(scale_x, scale_y, scale_z))));
}

void CurveAnimationMaterialParamFloat::update()
{
	time += Game::getIFps() * speed;

	ObjectPtr obj = checked_ptr_cast<Object>(node);
	if (!obj)
		return;

	// Evaluate each parameter's curve and apply to material
	for (int i = 0; i < parameters.size(); i++)
	{
		auto& param = parameters[i];
		obj->setMaterialParameterFloat(param->name, param->curve->evaluate(time), param->surface);
	}
}

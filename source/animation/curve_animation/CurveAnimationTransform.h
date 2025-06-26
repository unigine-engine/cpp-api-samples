#pragma once

#include "UnigineComponentSystem.h"

class CurveAnimationTransform :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CurveAnimationTransform, ComponentBase);
	COMPONENT_UPDATE(update);

	struct Curves : Unigine::ComponentStruct
	{
		PROP_PARAM(Curve2d, x);
		PROP_PARAM(Curve2d, y);
		PROP_PARAM(Curve2d, z);
	};

	PROP_STRUCT(Curves, position);
	PROP_STRUCT(Curves, rotation);
	PROP_STRUCT(Curves, scale_prop);

	PROP_PARAM(Float, speed, 1.0f);

private:
	void update();
	float time = 0;
};

class CurveAnimationMaterialParamFloat :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CurveAnimationMaterialParamFloat, ComponentBase);
	COMPONENT_UPDATE(update);

	struct Curves : Unigine::ComponentStruct
	{
		PROP_PARAM(Curve2d, curve);
		PROP_PARAM(String, name);
		PROP_PARAM(Int, surface, 0);
	};

	PROP_ARRAY_STRUCT(Curves, parameters);
	PROP_PARAM(Float, speed, 1.0f);

private:
	void update();
	float time = 0;
};

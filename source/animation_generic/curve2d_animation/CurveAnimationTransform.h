#pragma once

#include "UnigineComponentSystem.h"

// Animates node transform using Curve2D for each axis of position, rotation, and scale.
// Curve2D is a lightweight alternative to the Animation system - useful for simple
// procedural animations where full track/playback infrastructure is not needed.
// Each curve is evaluated at the current time to build the final transform matrix.
class CurveAnimationTransform :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CurveAnimationTransform, ComponentBase);
	COMPONENT_UPDATE(update);

	// Curve triplet for X, Y, Z components
	struct Curves : Unigine::ComponentStruct
	{
		PROP_PARAM(Curve2d, x);
		PROP_PARAM(Curve2d, y);
		PROP_PARAM(Curve2d, z);
	};

	// Transform components - each axis has its own curve for full control
	PROP_STRUCT(Curves, position);
	PROP_STRUCT(Curves, rotation);	// Euler angles in degrees
	PROP_STRUCT(Curves, scale_prop);

	PROP_PARAM(Float, speed, 1.0f);	// Playback speed multiplier

private:
	void update();
	float time = 0;
};

// Animates material float parameters using Curve2D.
// Each entry in the parameters array maps a curve to a material parameter by name.
// Useful for animating emission, opacity, or other shader parameters over time.
class CurveAnimationMaterialParamFloat :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CurveAnimationMaterialParamFloat, ComponentBase);
	COMPONENT_UPDATE(update);

	// Definition of a single animated parameter
	struct Curves : Unigine::ComponentStruct
	{
		PROP_PARAM(Curve2d, curve);		// Animation curve
		PROP_PARAM(String, name);		// Material parameter name
		PROP_PARAM(Int, surface, 0);	// Surface index to apply to
	};

	PROP_ARRAY_STRUCT(Curves, parameters);	// List of animated parameters
	PROP_PARAM(Float, speed, 1.0f);

private:
	void update();
	float time = 0;
};

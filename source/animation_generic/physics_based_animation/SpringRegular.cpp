// Spring variant using direct physics parameters (stiffness, damping) from component
// properties. Provides RefreshSpring() to reset values, useful when gameplay logic
// modifies spring parameters dynamically (e.g., difficulty ramping).

#include "SpringRegular.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(SpringRegular);

using namespace Unigine;
using namespace Math;

// Spring settings are reset to property-defined defaults.
void SpringRegular::RefreshSpring()
{
	refresh_spring_settings();
}

// Called when component is deactivated. Spring parameters are reset to defaults.
void SpringRegular::on_disable()
{
	// Reset to default values when component is disabled
	refresh_spring_settings();
}

// Stiffness and damping are set from property-defined defaults.
void SpringRegular::refresh_spring_settings()
{
	// Use property-defined defaults (allows reset after game mode difficulty ramping)
	stiffness = defaultStiffness;
	damping = defaultDamping;
}
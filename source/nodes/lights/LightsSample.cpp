// Demonstrates the three main light types: World, Omni, and Projected.
// World light simulates the sun with atmospheric scattering.
// Omni light radiates in all directions from a point with optional IES profiles.
// Projected light casts a cone with configurable FOV and penumbra.

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineLights.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class LightSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(LightSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Create one instance of each light type
		create_world_light();
		create_omni_light();
		create_projected_light();

		// Enable visualizer to show light shapes and ranges
		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);

		gui.init(this);
	}

	void update()
	{
		// Render light volume visualizations (world light has no local volume)
		omni_light->renderVisualizer();
		proj_light->renderVisualizer();
	}

	void shutdown()
	{
		gui.shutdown();
		Visualizer::setEnabled(visualizer_enabled);
	}

	void create_world_light()
	{
		// LightWorld: directional light simulating sun/moon with infinite range
		world_light = LightWorld::create(vec4_white);

		// Temperature mode calculates color from Kelvin value (6500K = daylight)
		world_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		world_light->setColorTemperature(6500.f);
		world_light->setIntensity(1.f);

		// Enable atmospheric scattering for realistic sky rendering
		world_light->setScattering(LightWorld::SCATTERING_SUN);

		// Rotation controls light direction (sun angle in the sky)
		world_light->setWorldRotation(quat(273.f, 330.f, 120.f));
	}

	void create_omni_light()
	{
		// LightOmni: point light radiating equally in all directions
		// Parameters: color, attenuation distance, name
		omni_light = LightOmni::create(vec4_white, 20.f, "omni");

		// Sphere shape creates soft area light shadows (more realistic)
		omni_light->setShapeType(Light::SHAPE_SPHERE);
		omni_light->setShapeRadius(0.17f);

		// Distance at which light intensity reaches zero
		omni_light->setAttenuationDistance(20.f);

		// IES mode uses photometric profiles for realistic light distribution
		omni_light->setShadowColorTextureMode(Light::SHADOW_COLOR_MODE_IES);

		// Texture can be loaded by path or assigned directly
		omni_light->setTextureFilePath(FileSystem::resolvePartialVirtualPath("white.texture"));

		// Warm color temperature (3165K = tungsten bulb)
		omni_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		omni_light->setColorTemperature(3165.f);
		omni_light->setIntensity(1.3f);

		omni_light->setWorldPosition(Vec3(1.1f, -1.3f, 2.2f));
	}

	void create_projected_light()
	{
		// LightProj: spotlight casting a cone of light
		// Parameters: color, attenuation distance, FOV angle, name
		proj_light = LightProj::create(vec4_white, 20.f, 70.f, "proj");

		// Warm-neutral color temperature (4000K = fluorescent)
		proj_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		proj_light->setColorTemperature(4000.f);
		proj_light->setIntensity(1.3f);

		// Point shape creates sharp shadows (ideal for spotlights)
		proj_light->setShapeType(Light::SHAPE_POINT);

		// Cone angle from center to edge
		proj_light->setFov(70.f);

		proj_light->setAttenuationDistance(20.f);
		// Penumbra: soft transition from lit to shadow region (0-1)
		proj_light->setPenumbra(0.15f);

		// Set the shadow color texture mode to SHADOW_COLOR_MODE_SIMPLE to be able to use any 2D texture
		proj_light->setShadowColorTextureMode(Light::SHADOW_COLOR_MODE_SIMPLE);

		proj_light->setTextureFilePath(FileSystem::resolvePartialVirtualPath("white.texture"));

		proj_light->setWorldPosition(Vec3(-0.8f, -1.1f, 2.3f));
		// Rotation determines light direction
		proj_light->setWorldRotation(quat(30.f, 343.f, 331.f));
	}

	// ========================================================================================

	struct SampleGui
	{
		void init(LightSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

			// World light rotation (simulates sun movement)
			sample_description_window.addFloatParameter(
				"world light angle",
				"Change the rotation of the LightWorld.",
				273.f,
				273.f,
				360.f,
				[sample](float value) {
					sample->world_light->setWorldRotation(quat(value, 330.f, 120.f));
				}
			);

			sample_description_window.addParameterSpacer();

			// Omni light brightness control
			sample_description_window.addFloatParameter(
				"omni light intensity",
				"Change the light intensity of the LightOmni.",
				1.3f,
				0.f,
				2.f,
				[sample](float value) {
					sample->omni_light->setIntensity(value);
				}
			);

			// Omni light color warmth/coolness
			sample_description_window.addFloatParameter(
				"omni light color temperature",
				"Change the color temperature of the LightOmni.",
				3165.f,
				1000.f,
				10'000.f,
				[sample](float value) {
					sample->omni_light->setColorTemperature(value);
				}
			);

			sample_description_window.addParameterSpacer();

			// Projected light edge softness
			sample_description_window.addFloatParameter(
				"projected light penumbra",
				"Change the penumbra parameter of the LightProj.",
				.15f,
				0.f,
				1.f,
				[sample](float value) {
					sample->proj_light->setPenumbra(value);
				}
			);

			// Projected light cone angle
			sample_description_window.addFloatParameter(
				"projected light fov",
				"Change the FOV of the LightProj.",
				75.f,
				0.f,
				120.f,
				[sample](float value) {
					sample->proj_light->setFov(value);
				}
			);
		}

		void shutdown() { sample_description_window.shutdown(); }

		SampleDescriptionWindow sample_description_window;
		LightSample *sample = nullptr;
	};

	// ========================================================================================

	// Global directional light (sun/moon)
	LightWorldPtr world_light;
	// Point light radiating in all directions
	LightOmniPtr omni_light;
	// Spotlight with cone-shaped illumination
	LightProjPtr proj_light;

	// Saved visualizer state for restoration
	bool visualizer_enabled = false;

	SampleGui gui;
};

REGISTER_COMPONENT(LightSample);

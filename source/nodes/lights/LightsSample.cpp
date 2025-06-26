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
		create_world_light();
		create_omni_light();
		create_projected_light();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);

		gui.init(this);
	}

	void update()
	{
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
		world_light = LightWorld::create(vec4_white);

		world_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		world_light->setColorTemperature(6500.f);
		world_light->setIntensity(1.f);

		world_light->setScattering(LightWorld::SCATTERING_SUN);

		world_light->setWorldRotation(quat(273.f, 330.f, 120.f));
	}

	void create_omni_light()
	{
		omni_light = LightOmni::create(vec4_white, 20.f, "omni");

		omni_light->setShapeType(Light::SHAPE_SPHERE);
		omni_light->setShapeRadius(0.17f);

		omni_light->setAttenuationDistance(20.f);

		// set the shadow color texture mode to ies for the omni light
		omni_light->setShadowColorTextureMode(Light::SHADOW_COLOR_MODE_IES);

		// set the projected texture by its path, alternatively you can use LightOmni::setTexture and LightOmni::setTextureImage
		omni_light->setTextureFilePath(FileSystem::resolvePartialVirtualPath("white.texture"));

		omni_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		omni_light->setColorTemperature(3165.f);
		omni_light->setIntensity(1.3f);

		omni_light->setWorldPosition(Vec3(1.1f, -1.3f, 2.2f));
	}

	void create_projected_light()
	{
		proj_light = LightProj::create(vec4_white, 20.f, 70.f, "proj");

		proj_light->setColorMode(Light::COLOR_MODE_TEMPERATURE);
		proj_light->setColorTemperature(4000.f);
		proj_light->setIntensity(1.3f);

		proj_light->setShapeType(Light::SHAPE_POINT);

		proj_light->setFov(70.f);

		proj_light->setAttenuationDistance(20.f);
		proj_light->setPenumbra(0.15f);

		// set the shadow color texture mode to SHADOW_COLOR_MODE_SIMPLE to be able to use any 2D texture
		proj_light->setShadowColorTextureMode(Light::SHADOW_COLOR_MODE_SIMPLE);

		proj_light->setTextureFilePath(FileSystem::resolvePartialVirtualPath("white.texture"));

		proj_light->setWorldPosition(Vec3(-0.8f, -1.1f, 2.3f));
		proj_light->setWorldRotation(quat(30.f, 343.f, 331.f));
	}

	// ========================================================================================

	struct SampleGui
	{
		void init(LightSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

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

	LightWorldPtr world_light;
	LightOmniPtr omni_light;
	LightProjPtr proj_light;

	bool visualizer_enabled = false;

	SampleGui gui;
};

REGISTER_COMPONENT(LightSample);

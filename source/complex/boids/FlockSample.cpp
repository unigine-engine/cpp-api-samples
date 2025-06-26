#include "FlockSample.h"

#include <UnigineVisualizer.h>

#include "FlockController.h"

REGISTER_COMPONENT(FlockSample);

void FlockSample::init()
{
	fish_controller = Unigine::ComponentSystem::get()->getComponent<FlockController>(
		fish_controller_parameter);
	if (!fish_controller)
	{
		Unigine::Log::error("FlockSample::init(): No FlockController on fish node was found\n");
		return;
	}
	bird_controller = Unigine::ComponentSystem::get()->getComponent<FlockController>(
		bird_controller_parameter);
	if (!bird_controller)
	{
		Unigine::Log::message("FlockSample::init(): No FlockController on bird node was found\n");
		return;
	}

	is_visualizer_enabled_at_sample_start = Unigine::Visualizer::isEnabled();
	Unigine::Visualizer::setEnabled(true);

	sample_description_window.createWindow();
	init_ui();
}

void FlockSample::init_ui()
{
	const auto create_flock_ui = [this](const Unigine::String &controller_name,
									 FlockController *controller) {
		sample_description_window.addLabel(controller_name);
		sample_description_window.addFloatParameter("Cohesion", "Cohesion coefficient",
			controller->cohesion_coefficient, 0.f, 10.f,
			[this, controller](float v) { controller->getFlock()->setCohesionCoefficient(v); });
		sample_description_window.addFloatParameter("Spot Radius", "Spot radius", controller->cohesion_spot_radius,
			0.f, 1000.f, [this, controller](float v) {
				for (auto &unit : controller->getFlock()->getUnits())
					unit.setSpotRadius(v);
			});
		sample_description_window.addFloatParameter("Alignment", "Alignment Coefficient",
			controller->alignment_coefficient, 0.f, 10.f,
			[this, controller](float v) { controller->getFlock()->setAlignmentCoefficient(v); });
		sample_description_window.addFloatParameter("Separation", "Separation coefficient",
			controller->separation_coefficient, 0.f, 10.f,
			[this, controller](float v) { controller->getFlock()->setSeparationCoefficient(v); });
		sample_description_window.addFloatParameter("Separation Desired Range", "separation desired range",
			controller->separation_desired_range, 1.f, 10.f, [this, controller](float v) {
				for (auto &unit : controller->getFlock()->getUnits())
					unit.setDesiredSeparationRange(v);
			});

		sample_description_window.addFloatParameter("Target", "Target chase coefficient",
			controller->target_coefficient, 0.f, 10.f,
			[this, controller](float v) { controller->getFlock()->setTargetChaseValue(v); });

		sample_description_window.addFloatParameter("Unit Max Speed", "Unit speed", controller->max_speed, 0.f, 100.f,
			[this, controller](float v) {
				for (auto &unit : controller->getFlock()->getUnits())
				{
					Unigine::Math::Random rand;
					auto offset = rand.getFloat(-5.f, 5.f);
					unit.setMaxSpeed(v + offset);
				}
			});
		sample_description_window.addFloatParameter("Unit Max Force", "unit max force", controller->max_force, 0.f,
			10.f, [this, controller](float v) {
				for (auto &unit : controller->getFlock()->getUnits())
				{
					Unigine::Math::Random rand;
					const auto offset = rand.getFloat(-5.f, 5.f);
					unit.setMaxForce(v + offset);
				}
			});

		sample_description_window.addFloatParameter("Unit Max Turn Speed", "unit max turn speed", controller->rotation_speed, 0.1f, 300.f, [this, controller](float v) {
			for (auto &unit : controller->getFlock()->getUnits())
			{
				unit.setRotationSpeed(v);
			}
		});
		sample_description_window.addParameterSpacer();
	};

	create_flock_ui("Birds controller", bird_controller);
	create_flock_ui("Fish controller", fish_controller);
	sample_description_window.addBoolParameter("Enable Debug", "Enable debugging", is_debug_enabled, [this](bool v) {
		is_debug_enabled = v;
	});
}

void FlockSample::update()
{
	if (is_debug_enabled)
		visualize_flock();
}

void FlockSample::shutdown()
{
	sample_description_window.shutdown();
	Unigine::Visualizer::setEnabled(is_visualizer_enabled_at_sample_start);
}

void FlockSample::visualize_flock()
{
	using namespace Unigine::Math;
	const auto visualize_flock = [this](FlockController* flock, const vec4 &color) {
		if (flock->getFlock()->getUnits().empty())
			return;

		BoundBox bb;
		bb.setTransform(flock->getFlock()->getUnits().first().getNode()->getWorldTransform());

		for (auto& unit : flock->getFlock()->getUnits())
			bb.expand(unit.getNode()->getWorldBoundBox());

		Unigine::Visualizer::renderBoundBox(bb, Mat4_identity, color, 0, false);
	};
	visualize_flock(bird_controller, vec4_red);
	visualize_flock(fish_controller, vec4_green);
}

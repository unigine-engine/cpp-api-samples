// Interactive demo of an animated character navigating an experimental
// navigation mesh with root-motion locomotion. The character is driven by
// ExperimentalSeekerCharacter: the route is computed and followed by the
// inherited ExperimentalSeeker logic, while the movement itself comes from
// the root motion of the animation graph (a straight walk with the course
// corrected by rotating the node, animated turns in place, start and stop
// animations).
// The target node is moved with the widget manipulator, the locomotion
// settings can be tweaked in the sample window.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/navigation/ExperimentalNavigationUtils.h"
#include "../../utils/navigation/ExperimentalSeekerCharacter.h"

class ExperimentalNavigationMeshCharacterLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshCharacterLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void update();
	void shutdown();

	void create_ui();
	void update_status();

	// sample visualization
	bool show_navigation_mesh{false};

	Unigine::ExperimentalNavigationMeshPtr navigation_mesh;
	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator{nullptr};
	ExperimentalSeekerCharacter *character{nullptr};
};

REGISTER_COMPONENT(ExperimentalNavigationMeshCharacterLogic);

using namespace Unigine;
using namespace Unigine::Math;

void ExperimentalNavigationMeshCharacterLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// the manipulator moves the target node; rotation and scaling make no
	// sense for a point target and are disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setAxesRotation(false);
		widget_manipulator->setAxesScale(false);
	}

	Unigine::Vector<ExperimentalSeekerCharacter *> characters;
	getComponentsInWorld<ExperimentalSeekerCharacter>(characters);
	if (characters.size() > 0)
		character = characters[0];
	else
		Log::warning("ExperimentalNavigationMeshCharacterLogic: no ExperimentalSeekerCharacter component in the world\n");

	// the navigation mesh the character walks on, for the visualization toggle
	if (character)
		navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(character->navigation.get());

	sample_description_window.createWindow();
	create_ui();
}

void ExperimentalNavigationMeshCharacterLogic::update()
{
	// disable player controls while dragging the manipulator
	if (widget_manipulator)
		Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	if (show_navigation_mesh && navigation_mesh)
		navigation_mesh->renderVisualizer();

	update_status();
}

void ExperimentalNavigationMeshCharacterLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

void ExperimentalNavigationMeshCharacterLogic::create_ui()
{
	if (!character)
		return;

	sample_description_window.addFloatParameter(
		"Movement speed",
		"The walking speed (in m/s): the locomotion animations are played at the rate movement speed / walk animation speed, so the root motion matches it (the rate never drops below the min rate of the character).",
		character->movement_speed, 0.7f, 6.f,
		[this](float value) {
			character->movement_speed = value;
		});

	sample_description_window.addFloatParameter(
		"Rotation speed",
		"The maximum yaw rate (in degrees per second) of the course correction applied by rotating the node.",
		character->rotation_speed, 30.f, 360.f,
		[this](float value) {
			character->rotation_speed = value;
		});

	sample_description_window.addFloatParameter(
		"Turn angle",
		"The angular error (in degrees) above which the character stops (when walking) and turns in place with the turn animation; below it the course is corrected by rotating the node.",
		character->turn_angle, 30.f, 180.f,
		[this](float value) {
			character->turn_angle = value;
		});

	sample_description_window.addFloatParameter(
		"Reach distance",
		"The distance at which the target counts as reached and the character stops; must cover the travel of the stop animation.",
		character->target_reach_distance, 0.2f, 3.f,
		[this](float value) {
			character->target_reach_distance = value;
		});

	sample_description_window.addParameterSpacer();

	sample_description_window.addBoolParameter(
		"Show Seeker Debug",
		"Draws the route corridor and the velocities of the character.",
		character->debug_visualizer_enabled != 0,
		[this](bool enabled) {
			character->debug_visualizer_enabled = enabled ? 1 : 0;
		});

	sample_description_window.addBoolParameter(
		"Show Navigation Mesh",
		"Draws the navigation mesh colored by the areas of its polygons.",
		show_navigation_mesh,
		[this](bool enabled) {
			show_navigation_mesh = enabled;
		});
}

void ExperimentalNavigationMeshCharacterLogic::update_status()
{
	if (!character)
	{
		sample_description_window.setStatus("Add the ExperimentalSeekerCharacter component to the world.");
		return;
	}

	sample_description_window.setStatus(String::format(
		"Route: %s\n"
		"Control state: %s, animation state: %s\n"
		"Velocity: %.2f m/s\n"
		"Walk animation: %.2f m/s at rate 1, playing at rate %.2f\n",
		Utils::getRouteStatusName(character->getRouteStatus()),
		character->getControlStateName(),
		character->getLocomotionStateName(),
		character->getVelocity().length(),
		character->animation_speed.get(),
		character->getPlaybackRate()));
}

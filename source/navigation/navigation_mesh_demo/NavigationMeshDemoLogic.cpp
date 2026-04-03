// Interactive navigation mesh demo with multiple Seeker agents. Each Seeker uses
// PathRoute (create2D/create3D) to calculate paths toward a target node. Route radius
// defines minimum clearance from obstacles. Agents rotate toward movement direction
// and validate position against navigation area via inside2D/inside3D checks.
// When a Seeker reaches its PathfindingTarget, the target relocates to a random
// position from a predefined set. Manipulators allow dragging waypoints on XY plane.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineComponentSystem.h>
#include <UniginePathFinding.h>
#include <UnigineGame.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/navigation/Seeker.h"

class NavigationMeshDemoLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NavigationMeshDemoLogic, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, route_radius, 0.15f);
	PROP_PARAM(Node, navigation);
	PROP_PARAM(Node, seekers);

	void setSeekersRouteRadius(float radius);

private:
	void init();
	void update();
	void shutdown();

	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator;
};

REGISTER_COMPONENT(NavigationMeshDemoLogic);

using namespace Unigine;
using namespace Unigine::Math;

// Visualizer is enabled, manipulators are configured for XY-plane movement, and UI is created.
void NavigationMeshDemoLogic::init()
{
	Visualizer::setEnabled(true);
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// Manipulators are restricted to XY translation only (Z is determined by navigation surface)
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setZAxisTranslation(false);
		widget_manipulator->setXAxisRotation(false);
		widget_manipulator->setYAxisRotation(false);
		widget_manipulator->setXAxisScale(false);
		widget_manipulator->setYAxisScale(false);
		widget_manipulator->setZAxisScale(false);
	}

	// Initial route radius is applied to all Seeker agents
	setSeekersRouteRadius(route_radius);

	{
		sample_description_window.createWindow();

		sample_description_window.addFloatParameter(
			"Route radius",
			"The radius required to move the point along the route inside the navigation area.",
			route_radius,
			0.f,
			1.f,
			[this](float radius) { setSeekersRouteRadius(radius); }
		);
	}
}

// Player control is toggled based on manipulator state; navigation mesh is visualized.
void NavigationMeshDemoLogic::update()
{
	// Camera control is disabled while user is dragging a manipulator
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	// Navigation mesh boundaries and walkable areas are rendered for debugging
	if (navigation)
		navigation->renderVisualizer();
}

// Visualizer is disabled, mouse is restored, and UI is cleaned up.
void NavigationMeshDemoLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Route radius is propagated to all child Seeker components.
void NavigationMeshDemoLogic::setSeekersRouteRadius(float radius)
{
	if (seekers)
	{
		// Each child node with a Seeker component receives the updated radius
		for (int i = 0; i < seekers->getNumChildren(); i += 1)
		{
			auto seeker_component = getComponent<Seeker>(seekers->getChild(i));
			if (seeker_component)
				seeker_component->setRouteRadius(radius);
		}

	}
}

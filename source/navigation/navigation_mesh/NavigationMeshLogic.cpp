// Demonstrates pathfinding on a navigation mesh using PathRoute.
// Start and end points can be moved via manipulators. A route is computed
// using create2D() which finds the shortest path constrained to the navmesh.
// If no valid path exists, a red line is drawn between the points.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UniginePathFinding.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

class NavigationMeshLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NavigationMeshLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, path_start)
	PROP_PARAM(Node, path_end)
	PROP_PARAM(Node, navigation)
	PROP_PARAM(Float, route_radius, 0.15f)
	PROP_PARAM(Color, route_color, Unigine::Math::vec4_white)

private:
	void init();
	void update();
	void shutdown();

	Unigine::PathRoutePtr route;
	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator;
};


REGISTER_COMPONENT(NavigationMeshLogic);

using namespace Unigine;
using namespace Unigine::Math;

// PathRoute is created, manipulators are configured, and UI is initialized.
void NavigationMeshLogic::init()
{
	Visualizer::setEnabled(true);
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// Create a PathRoute object to compute paths on the navigation mesh.
	// Radius defines the agent size for path clearance calculations.
	route = PathRoute::create();
	route->setRadius(route_radius);

	// Configure manipulators: restrict movement to XY plane only
	// (Z is constrained to navmesh surface), disable rotation and scaling
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

	{
		sample_description_window.createWindow();

		sample_description_window.addFloatParameter(
			"Route radius",
			"The radius required to move the point along the route inside the navigation area.",
			route_radius,
			0.f,
			1.f,
			[this](float value) {
				route->setRadius(value);
			}
		);
	}
}

// Route is recalculated each frame; path or error line is visualized.
void NavigationMeshLogic::update()
{
	// Player controls are disabled when dragging manipulators
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	// Draw the navigation mesh wireframe for debugging
	if (navigation)
		navigation->renderVisualizer();

	if (path_start && path_end)
	{
		auto start = path_start->getWorldPosition();
		auto end = path_end->getWorldPosition();

		// Compute 2D path (ignoring Z differences, projecting onto navmesh)
		route->create2D(start, end);

		// Draw path if reachable, otherwise show a red "no path" line
		if (route->isReached())
			route->renderVisualizer(route_color);

		else
			Visualizer::renderLine3D(start, end, vec4_red, Game::getIFps());
	}

	{
		auto status = String::format(
			"Route length: %.2f\n"
			"Route points: %d\n",
			route->getDistance(),
			route->getNumPoints()
		);

		sample_description_window.setStatus(status);
	}
}

// Visualizer is disabled, mouse is restored, and UI is cleaned up.
void NavigationMeshLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

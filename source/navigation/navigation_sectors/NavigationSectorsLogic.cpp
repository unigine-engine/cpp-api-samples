// Demonstrates pathfinding across multiple connected navigation sectors.
// NavigationSector defines a box-shaped walkable area (unlike NavigationMesh
// which uses arbitrary geometry). Overlapping or adjacent sectors are
// automatically connected; PathRoute::create2D() finds paths across them.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UniginePathFinding.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

class NavigationSectorsLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NavigationSectorsLogic, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);
	
	PROP_PARAM(Node, path_start);
	PROP_PARAM(Node, path_end);
	PROP_PARAM(Node, navigation);
	PROP_PARAM(Float, route_radius, 0.12f);
	PROP_PARAM(Color, route_color, Unigine::Math::vec4_white);
	
private:
	void init();
	void update();
	void shutdown();
	
	Unigine::PathRoutePtr route;
	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator;
};

REGISTER_COMPONENT(NavigationSectorsLogic);

using namespace Unigine;
using namespace Unigine::Math;

// PathRoute is created, manipulators are configured, and UI is initialized.
void NavigationSectorsLogic::init()
{
	Visualizer::setEnabled(true);
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// PathRoute is created for cross-sector pathfinding
	route = PathRoute::create();
	route->setRadius(route_radius);

	// Manipulators are configured with scaling disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
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

// Route is recalculated; all sectors and path are visualized.
void NavigationSectorsLogic::update()
{
	// Player controls are disabled when dragging manipulators
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	// Each navigation sector is rendered separately (sectors are children of navigation node)
	if (navigation)
	{
		for (int i = 0; i < navigation->getNumChildren(); i += 1)
			navigation->getChild(i)->renderVisualizer();
	}

	if (path_start && path_end)
	{
		auto start = path_start->getWorldPosition();
		auto end = path_end->getWorldPosition();

		// Pathfinding automatically crosses sector boundaries when sectors overlap
		route->create2D(start, end);

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
void NavigationSectorsLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

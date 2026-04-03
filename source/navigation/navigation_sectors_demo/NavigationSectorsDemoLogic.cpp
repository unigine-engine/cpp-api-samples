// Interactive navigation sectors demo with multiple Seeker agents. Uses box-shaped
// NavigationSector areas instead of NavigationMesh geometry. Overlapping sectors
// are automatically connected; Seekers navigate across boundaries seamlessly.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineComponentSystem.h>
#include <UniginePathFinding.h>
#include <UnigineGame.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/navigation/Seeker.h"

class NavigationSectorsDemoLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NavigationSectorsDemoLogic, Unigine::ComponentBase);
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

REGISTER_COMPONENT(NavigationSectorsDemoLogic);

using namespace Unigine;
using namespace Unigine::Math;

// Visualizer is enabled, manipulators are configured, and UI is initialized.
void NavigationSectorsDemoLogic::init()
{
	Visualizer::setEnabled(true);
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// Manipulators are configured with scaling disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
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
			2.f,
			[this](float radius) { setSeekersRouteRadius(radius); }
		);
	}
}

// Player control is toggled based on manipulator state; navigation sectors are visualized.
void NavigationSectorsDemoLogic::update()
{
	// Player controls are disabled when dragging manipulators
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	// Navigation sector boundaries are rendered for debugging
	if (navigation)
		navigation->renderVisualizer();
}

// Visualizer is disabled, mouse is restored, and UI is cleaned up.
void NavigationSectorsDemoLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Route radius is propagated to all child Seeker components.
void NavigationSectorsDemoLogic::setSeekersRouteRadius(float radius)
{
	if (seekers)
	{
		for (int i = 0; i < seekers->getNumChildren(); i += 1)
		{
			auto seeker_component = getComponent<Seeker>(seekers->getChild(i));
			if (seeker_component)
				seeker_component->setRouteRadius(radius);
		}
	}
}

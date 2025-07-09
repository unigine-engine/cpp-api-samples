#include "Lidar.h"

#include <UnigineGame.h>
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

class LidarSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(LidarSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, lidar_node);
	PROP_PARAM(Node, sun);

	PROP_PARAM(Float, rotation_speed, 2.f);

private:
	void init()
	{
		lidar = getComponent<Lidar>(lidar_node);
		window.createWindow(Gui::ALIGN_LEFT);

		window.addBoolParameter("Debug Render Points", "", lidar->is_debug_render_points > 0, [this](bool value) { lidar->is_debug_render_points = (int)value; });
		window.addBoolParameter("Debug Render Textures", "", lidar->is_debug_render_textures > 0, [this](bool value) { lidar->is_debug_render_textures = (int)value; });
		window.addBoolParameter("Debug Render Frustums", "", lidar->is_debug_render_frustums > 0, [this](bool value) { lidar->is_debug_render_frustums = (int)value; });
		window.addBoolParameter("Lights Enabled", "", true, [this](bool value) { sun->setEnabled(value); });

		auto group_box = window.getParameterGroupBox();
		auto save_button = WidgetButton::create("Save Lidar Data");
		save_button->getEventClicked().connect(this, &LidarSample::on_save_button_clicked);
		group_box->addChild(save_button, Gui::ALIGN_LEFT);
	}

	void update()
	{
		lidar->getNode()->rotate(quat(vec3_up, rotation_speed * Game::getIFps()));
	}

	void shutdown()
	{
		window.shutdown();
	}

	void on_save_button_clicked(WidgetPtr const&, int)
	{
		lidar->save_lidar_data_to_las(joinPaths(getWorldRootPath(), "lidar.las"));
	}

	SampleDescriptionWindow window;
	Lidar *lidar = nullptr;
};

REGISTER_COMPONENT(LidarSample);

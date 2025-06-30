#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include <UnigineViewport.h>
#include "ObjectDepth.h"
#include <UniginePhysics.h>

class DiggingTool : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DiggingTool, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, lmap_node, "Landscape Terrain Layer Map");
	PROP_PARAM(Node, digging_object_node, "Digging Object");

private:
	void init();
	void update();
	void shutdown();

	void update_digging_object();
	bool should_update_terrain();
	void enqueue_terrain_update();
	void reset_modifications();

	void on_texture_draw(const Unigine::UGUID &guid, int operation_id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coords, int data_mask);

private:
	struct DrawData
	{
		Unigine::Math::ivec2 resolution;
		Unigine::TexturePtr object_depth_texture;
	};

	using LandscapeOperationID = int;

	Unigine::LandscapeLayerMapPtr layer_map;
	Unigine::MaterialPtr digging_tool_mat;
	Unigine::ObjectPtr digging_object;
	ObjectDepth object_depth;

	Unigine::Math::WorldBoundBox digging_object_wbb;

	float position_spacing = .025f;
	float rotation_spacing = 1.f;

	Unigine::Math::Mat4 target_transform;
	Unigine::Math::Vec3 prev_position;
	Unigine::Math::vec3 prev_rotation;

	Unigine::HashMap<LandscapeOperationID, DrawData> per_operation_draw_data;
	Unigine::EventConnection texture_draw_connection;

private:
	struct Gui
	{
		void init(DiggingTool *sample);
		void update();
		void shutdown();

		void set_manipulator(Unigine::WidgetManipulatorPtr manipulator);

		Unigine::WidgetManipulatorTranslatorPtr translator;
		Unigine::WidgetManipulatorRotatorPtr rotator;
		Unigine::WidgetManipulatorPtr active;
		enum { LOCAL, WORLD } basis = LOCAL;
		SampleDescriptionWindow window;
		DiggingTool *sample = nullptr;
	};

	Gui gui;
};

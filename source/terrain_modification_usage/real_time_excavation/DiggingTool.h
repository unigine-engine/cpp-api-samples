// Implements terrain excavation using object depth projection. A digging object's
// shape is projected onto the terrain surface, modifying height and mask data
// in the underlying LandscapeLayerMap using async texture draw operations.

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

	// Terrain layer to modify with excavation
	PROP_PARAM(Node, lmap_node, "Landscape Terrain Layer Map");
	// Object whose shape defines the excavation volume
	PROP_PARAM(Node, digging_object_node, "Digging Object");

private:
	void init();
	void update();
	void shutdown();

	// Smooth interpolation of digging object position/rotation
	void update_digging_object();
	// Check if object moved enough to warrant terrain update
	bool should_update_terrain();
	// Queue async landscape texture modification
	void enqueue_terrain_update();
	// Clear all terrain modifications made by this tool
	void reset_modifications();

	// Callback invoked when async texture draw completes
	void on_texture_draw(const Unigine::UGUID &guid, int operation_id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coords, int data_mask);

private:
	// Data stored per pending async operation
	struct DrawData
	{
		Unigine::Math::ivec2 resolution;
		// Depth texture rendered from above the digging object
		Unigine::TexturePtr object_depth_texture;
	};

	using LandscapeOperationID = int;

	Unigine::LandscapeLayerMapPtr layer_map;
	// Material containing excavation shader passes
	Unigine::MaterialPtr digging_tool_mat;
	Unigine::ObjectPtr digging_object;
	// Helper class for rendering depth from above
	ObjectDepth object_depth;

	// Current world-space bounding box of digging object
	Unigine::Math::WorldBoundBox digging_object_wbb;

	// Minimum movement before triggering terrain update (world units)
	float position_spacing = .025f;
	// Minimum rotation before triggering terrain update (degrees)
	float rotation_spacing = 1.f;

	// Target transform for smooth interpolation
	Unigine::Math::Mat4 target_transform;
	// Previous frame values for change detection
	Unigine::Math::Vec3 prev_position;
	Unigine::Math::vec3 prev_rotation;

	// Pending operations awaiting callback completion
	Unigine::HashMap<LandscapeOperationID, DrawData> per_operation_draw_data;
	Unigine::EventConnection texture_draw_connection;

private:
	// UI for manipulating the digging object in the scene
	struct Gui
	{
		void init(DiggingTool *sample);
		void update();
		void shutdown();

		void set_manipulator(Unigine::WidgetManipulatorPtr manipulator);

		// Translation and rotation manipulator widgets
		Unigine::WidgetManipulatorTranslatorPtr translator;
		Unigine::WidgetManipulatorRotatorPtr rotator;
		Unigine::WidgetManipulatorPtr active;
		// Coordinate system for manipulator orientation
		enum { LOCAL, WORLD } basis = LOCAL;
		SampleDescriptionWindow window;
		DiggingTool *sample = nullptr;
	};

	Gui gui;
};

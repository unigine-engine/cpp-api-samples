// Demonstrates polygon-based terrain modification and mesh generation.
// Supports mesh creation, decal placement, mask drawing, and height editing.
// Figures are defined by child node positions and triangulated for operations.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/GeometryGenerator.h"
#include "../../utils/LayerMapPolygonEditor.h"
#include "../../user_interface/widget_manipulators/Manipulators.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

// Controller for polygon-based landscape modification features.
class LandscapePolygonsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapePolygonsSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, terrain_param, "Terrain");				// Reference to terrain node
	PROP_ARRAY(Node, figures, "Figures");						// Array of figure parent nodes (children define polygon vertices)

	PROP_PARAM(Node, layer_map_node, "Layer Map");				// Target landscape layer map for modifications
	PROP_PARAM(Node, layer_map_editor_node, "Layer Map Editor");// Editor utility node reference
	PROP_PARAM(Node, manipulators_node, "");					// Node containing manipulator controls
	PROP_PARAM(Material, ortho_material, "DecalOrtho material");// Material applied to ortho decals
	PROP_PARAM(Material, mesh_material, "Mesh render material");// Material used for mesh-to-texture rendering
	PROP_PARAM(Node, clutter_node, "Clutter node");				// Vegetation clutter object to invalidate

private:
	void init();
	void update();
	void shutdown();

	void draw_triangulation_all_figures();		// Visualizes triangulated polygons for all figures
	void init_gui();							// Creates control panel with mode selection and feature buttons
	void reset();								// Reverts landscape modifications and removes created nodes
	void render_mesh_to_texture(Unigine::MeshPtr mesh, Unigine::TexturePtr texture, Unigine::MaterialPtr render_material, Unigine::Math::vec4 color = Unigine::Math::vec4_white);	// Renders mesh to texture using orthographic projection
	void generate_mesh_button();				// Creates ObjectMeshStatic from polygon figures
	void generate_decal_mesh_button();			// Creates DecalMesh from polygon figures
	void generate_decal_ortho_button();			// Creates DecalOrtho with rendered texture from polygons
	void draw_terrain_mask_button();			// Draws mask data onto landscape using polygon shapes
	void level_terrain_button();				// Flattens terrain height within polygon bounds
	void lower_terrain_button();				// Lowers terrain height within polygon bounds
	void revert_points_positions();				// Restores figure and point nodes to initial positions

	// Renders triangle edges using visualizer for debugging.
	static void draw_triangulation(
		const Unigine::Vector<Unigine::Math::Vec3>& mesh_points,
		const Unigine::Vector<unsigned short>& indices);

private:
	Unigine::LandscapeLayerMapPtr layer_map;		// Target landscape layer map

	Unigine::ObjectMeshClutterPtr clutter;			// Clutter object for vegetation invalidation
	bool is_clutter_mask_valid = true;				// Tracks if clutter needs refresh after mask changes
	const int clutter_invalidation_delay_frames = 10;	// Delay before clutter is invalidated
	int invalidation_frame = 0;						// Frame when invalidation was triggered

	Unigine::WidgetListBoxPtr feature_list;			// List widget for feature selection

	Unigine::Vector<Unigine::TexturePtr> ortho_decal_textures;	// Rendered textures for ortho decals
	Unigine::Vector<Unigine::NodePtr> created_nodes;			// Nodes created during generation
	Unigine::Vector<Unigine::Math::Vec3> figures_initial_positions;			// Stored initial figure positions for reset
	Unigine::Vector<Unigine::Vector<Unigine::Math::Vec3>> points_initial_positions;	// Stored initial point positions for reset

	SampleDescriptionWindow description_window;		// UI window for sample controls
	LayerMapPolygonEditor* layer_map_editor = nullptr;	// Polygon editor utility instance
	Manipulators* manipulators = nullptr;			// Transform manipulator controls
};

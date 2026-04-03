#pragma once
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/GeometryGenerator.h"
#include "../../user_interface/widget_manipulators/Manipulators.h"

// Demonstrates procedural mesh generation along a spline path.
// Control points from scene nodes are interpolated into a smooth spline,
// then extruded into geometry (ribbon, square pipe, or round pipe profiles).
// Mesh is updated in real-time via procedural mode when manipulators move points.
// UV mapping supports both tiling and stretch modes along length/width axes.
class SplineGenerationSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(SplineGenerationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_ARRAY(Node, spline_nodes, "Spline nodes"); // Control points defining the spline path

	// Mesh generation parameters
	PROP_PARAM(Float, mesh_width_param, 5.0f);              // Width of generated geometry
	PROP_PARAM(Int, width_subdivision_param, 5);            // Subdivisions across width
	PROP_PARAM(Float, segments_length_param, 3.0f);         // Length of each spline segment

	// UV mapping parameters
	PROP_PARAM(Float, length_tiling, 1.0f);                 // Texture tiling along spline length
	PROP_PARAM(Float, width_tiling, 1.0f);                  // Texture tiling across width
	PROP_PARAM(Toggle, stretch_uv_along_length_param, false); // Stretch UV to fit entire length
	PROP_PARAM(Toggle, stretch_uv_along_width_param, false);  // Stretch UV to fit entire width

	PROP_PARAM(Toggle, show_wireframe_param, false);        // Display wireframe overlay
	PROP_PARAM(Material, spline_obejct_material, "Spline Object Material"); // Material for mesh

	PROP_PARAM(Node, manipulators_node, "Manipulators node"); // Node containing Manipulators component

private:
	// Available geometry profiles for spline extrusion
	enum GENERATION_TYPE
	{
		RIBBON,      // Flat ribbon surface
		SQUARE_PIPE, // Four-sided tube
		ROUND_PIPE   // Circular tube
	};

	void init();     // Initializes mesh object and manipulators
	void update();   // Visualizes spline points and wireframe
	void shutdown(); // Releases UI resources

	void init_gui();                  // Creates parameter controls window
	void generate_spline_mesh();      // Generates flat ribbon geometry
	void generate_spline_square_pipe(); // Generates square tube geometry
	void generate_spline_round_pipe();  // Generates circular tube geometry
	void regenerate();                // Regenerates with current type
	void clear();                     // Clears mesh and spline data

	Unigine::Vector<Unigine::Math::Vec3> get_points();     // Extracts positions from control nodes
	GeometryGenerator::SplineMeshUVSettings get_uv_settings(); // Builds UV settings from params

private:
	Unigine::ObjectMeshStaticPtr spline_mesh_object;             // Procedural mesh object
	Unigine::Vector<Unigine::Math::Vec3> current_spline_points;  // Interpolated spline points
	GENERATION_TYPE last_generation_type = RIBBON;               // Currently selected geometry type

	SampleDescriptionWindow description_window; // UI window for parameters
	Manipulators *manipulators = nullptr;       // Control point manipulation handler
	EventConnections event_connections;         // Stores event subscriptions
};

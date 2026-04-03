// Generates procedural mesh geometry along a spline path using GeometryGenerator.
// Control points are interactive via Manipulators; moving any point regenerates
// the mesh. Supports ribbon, square pipe, and round pipe geometry types.

#include "SplineGenerationSample.h"

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>


REGISTER_COMPONENT(SplineGenerationSample);

using namespace Unigine;
using namespace Math;

// Procedural mesh object and manipulators are initialized; UI is created.
void SplineGenerationSample::init()
{
	// Empty mesh object is created to receive procedural geometry
	spline_mesh_object = ObjectMeshStatic::create();

	// PROCEDURAL_MODE_DYNAMIC keeps mesh in RAM for frequent updates.
	// Second parameter (0) uses default RAM-based rendering.
	spline_mesh_object->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC, 0);

	// Object is allowed to be moved/transformed in the scene
	spline_mesh_object->setImmovable(false);

	if (!spline_obejct_material.get())
	{
		Log::warning("SplineGenerationSample::init spline object material is unspecified.\n");
		return;
	}

	// Manipulators component is retrieved from the specified node to handle control point dragging
	manipulators = ComponentSystem::get()->getComponent<Manipulators>(manipulators_node);
	if (!manipulators)
	{
		Log::warning("SplineGenerationSample::init manipulators_node is unspecified.\n");
		return;
	}

	// Transform changes are subscribed to regenerate spline mesh when any control point moves
	manipulators->getEventTransformChanged().connect(event_connections, [this](ObjectPtr obj) {regenerate(); });

	Visualizer::setEnabled(true);
	init_gui();
}

// Spline points and mesh wireframe are visualized each frame.
void SplineGenerationSample::update()
{
	// Each interpolated spline point is visualized as a small red sphere
	for (auto& point : current_spline_points)
	{
		Visualizer::renderPoint3D(point, 0.1f, vec4_red);
	}

	// Wireframe overlay is rendered on the generated mesh surface
	if (spline_mesh_object)
	{
		Visualizer::renderObjectSurface(spline_mesh_object, 0, vec4_white);
	}
}

// UI window is released and visualizer is disabled.
void SplineGenerationSample::shutdown()
{
	description_window.shutdown();
	Visualizer::setEnabled(false);
}

// Flat ribbon mesh is generated along the spline path.
void SplineGenerationSample::generate_spline_mesh()
{
	// Control point positions are retrieved from scene nodes
	Vector<Vec3> points = get_points();

	// Control points are interpolated into a smooth spline with uniform segment lengths
	if (!GeometryGenerator::computeSplinePoints(points, current_spline_points, segments_length_param))
	{
		Log::message("SplineGenerationSample::generateSplineMesh Invalid spline points");
	}

	// Flat ribbon mesh is generated along the spline path
	MeshPtr mesh = GeometryGenerator::generateSplineMesh(current_spline_points, mesh_width_param, width_subdivision_param, segments_length_param, get_uv_settings());

	// Generated mesh is applied immediately (Force mode blocks until complete)
	spline_mesh_object->applyMoveMeshProceduralForce(mesh, 0);
	spline_mesh_object->setMaterial(spline_obejct_material, 0);
	last_generation_type = RIBBON;
}

// Square pipe mesh is generated along the spline path.
void SplineGenerationSample::generate_spline_square_pipe()
{
	Vector<Vec3> points = get_points();
	if (!GeometryGenerator::computeSplinePoints(points, current_spline_points, segments_length_param))
	{
		Log::message("SplineGenerationSample::generateSplineMesh Invalid spline points");
	}
	MeshPtr mesh = GeometryGenerator::generateSplineMeshSquare(current_spline_points, mesh_width_param, width_subdivision_param, segments_length_param, get_uv_settings());

	spline_mesh_object->applyMoveMeshProceduralForce(mesh, 0);
	spline_mesh_object->setMaterial(spline_obejct_material, 0);
	last_generation_type = SQUARE_PIPE;
}

// Round pipe mesh is generated along the spline path.
void SplineGenerationSample::generate_spline_round_pipe()
{
	Vector<Vec3> points = get_points();
	if (!GeometryGenerator::computeSplinePoints(points, current_spline_points, segments_length_param))
	{
		Log::message("SplineGenerationSample::generateSplineMesh Invalid spline points");
	}

	// Circular tube mesh is generated; width parameter becomes radius (divided by 2)
	MeshPtr mesh = GeometryGenerator::generateSplineMeshRound(current_spline_points, mesh_width_param / 2, width_subdivision_param, segments_length_param, get_uv_settings());
	spline_mesh_object->applyMoveMeshProceduralForce(mesh, 0);
	spline_mesh_object->setMaterial(spline_obejct_material, 0);
	last_generation_type = ROUND_PIPE;
}

// Mesh is regenerated using the last selected geometry type.
void SplineGenerationSample::regenerate()
{
	switch (last_generation_type)
	{
	case SplineGenerationSample::RIBBON:
		generate_spline_mesh();
		break;
	case SplineGenerationSample::SQUARE_PIPE:
		generate_spline_square_pipe();
		break;
	case SplineGenerationSample::ROUND_PIPE:
		generate_spline_round_pipe();
		break;
	default:
		generate_spline_mesh();
		break;
	}
}

// Spline points are cleared and mesh geometry is removed.
void SplineGenerationSample::clear()
{
	current_spline_points.clear();
	// Current geometry is replaced with an empty mesh to clear the visual
	spline_mesh_object->applyMoveMeshProceduralForce(Mesh::create(), 0);
}

// World positions are extracted from the array of control point nodes.
Unigine::Vector<Unigine::Math::Vec3> SplineGenerationSample::get_points()
{
	int pointsNum = spline_nodes.size();
	Vector<Vec3> points = Vector<Vec3>(pointsNum);

	for (int i = 0; i < pointsNum; i++)
	{
		points[i] = spline_nodes[i]->getWorldPosition();
	}
	return points;
}


// UI window with parameter controls and generation buttons is created.
void SplineGenerationSample::init_gui()
{
	description_window.createWindow();
	auto params = description_window.getParameterGroupBox();

	auto featuresLabel = WidgetLabel::create("Generation features:");
	params->addChild(featuresLabel, Gui::ALIGN_LEFT);
	auto window = description_window.getWindow();
	auto buttons_grid_box = WidgetGridBox::create(3, 2, 2);
	params->addChild(buttons_grid_box);

	description_window.addFloatParameter("Mesh Width", "Mesh Width", mesh_width_param, 0.1f, 10.0f, [this](float v) {
		mesh_width_param = v;
		regenerate();
		});

	auto sliderParam = description_window.addIntParameter("Mesh Width Subdivision Number", "Mesh Width Subdivision Number", width_subdivision_param, 1, 20, [this](int v) {
		width_subdivision_param = v;
		regenerate();
		});

	description_window.addFloatParameter("Segment Length", "Segment Length", segments_length_param, 0.1f, 5.0f, [this](float v) {
		segments_length_param = v;
		regenerate();
		});

	description_window.addFloatParameter("UV Length Tiling", "Length Tiling", length_tiling, 0.05, 1.5, [this](float v) {
		length_tiling = v;
		regenerate();
		});
	description_window.addFloatParameter("UV Width Tiling", "Width Tiling", width_tiling, 0.5, 5, [this](float v) {
		width_tiling = v;
		regenerate();
		});
	description_window.addBoolParameter("Stretch UV Along Length", "Stretch UV Along Length", stretch_uv_along_length_param, [this](bool v) {
		stretch_uv_along_length_param = v;
		regenerate();
		});

	description_window.addBoolParameter("Stretch UV Along Width", "Stretch UV Along Width", stretch_uv_along_width_param, [this](bool v) {
		stretch_uv_along_width_param = v;
		regenerate();
		});

	description_window.addBoolParameter("Show Wireframe", "Show Wireframe", show_wireframe_param, [this](bool v) {
		show_wireframe_param = v;
		Visualizer::setEnabled(v);
		});

	auto button = WidgetButton::create("Ribbon");
	button->getEventClicked().connect(this, &SplineGenerationSample::generate_spline_mesh);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	button = WidgetButton::create("Square Pipe");
	button->getEventClicked().connect(this, &SplineGenerationSample::generate_spline_square_pipe);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);

	button = WidgetButton::create("Round Pipe");
	button->getEventClicked().connect(this, &SplineGenerationSample::generate_spline_round_pipe);
	buttons_grid_box->addChild(button, Gui::ALIGN_EXPAND);


	button = WidgetButton::create("Clear");
	button->getEventClicked().connect(this, &SplineGenerationSample::clear);
	params->addChild(button, Gui::ALIGN_BOTTOM);
}

// UV settings structure is populated from current parameter values.
GeometryGenerator::SplineMeshUVSettings SplineGenerationSample::get_uv_settings()
{
	auto settings = GeometryGenerator::SplineMeshUVSettings{};
	settings.length_tiling_amount = length_tiling;
	settings.width_tiling_amount = width_tiling;
	settings.stretch_along_length = (bool)stretch_uv_along_length_param.get();
	settings.stretch_along_width = (bool)stretch_uv_along_width_param.get();
	return settings;
}

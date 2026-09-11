// Interactive showcase of spatial queries on an experimental navigation mesh.
// The query point (and the target point for the boundary raycast) are moved
// via manipulators, the query mode and its parameters are selected in the
// sample window.
//
// The queries themselves are wrapped into the self-contained query_* functions
// below that depend only on the engine API, so any of them can be copied into
// a project together with its result struct. The component is only responsible
// for the sample UI and the visualization of the query results.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../menu_ui/SampleWidgets.h"

class ExperimentalNavigationMeshQueriesLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshQueriesLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, query_point)
	PROP_PARAM(Node, raycast_target)
	PROP_PARAM(Node, navigation)
	PROP_PARAM(Color, query_color, Unigine::Math::vec4_white)

private:
	enum QUERY_MODE
	{
		QUERY_MODE_NEAREST_POINT = 0,
		QUERY_MODE_REACHABLE_POINTS,
		QUERY_MODE_RANDOM_REACHABLE_POINTS,
		QUERY_MODE_RANDOM_POINTS,
		QUERY_MODE_NEAREST_BOUNDARY,
		QUERY_MODE_BOUNDARY_RAYCAST,
		QUERY_MODE_POLYGONS_IN_BOX,
	};

	void init();
	void update();
	void shutdown();

	void init_navigation();
	void apply_settings();

	// query mode visualization, each renders the results of the corresponding
	// query_* function and returns the status text
	Unigine::String update_nearest_point();
	Unigine::String update_reachable_points();
	Unigine::String update_random_points(bool reachable_only);
	Unigine::String update_nearest_boundary();
	Unigine::String update_boundary_raycast();
	Unigine::String update_polygons_in_box();

	Unigine::String get_polygon_info(const char *title, long long polygon) const;

	// sample UI
	void create_ui();
	void update_mode_ui();

	Unigine::ExperimentalNavigationMeshPtr navigation_mesh;
	Unigine::ExperimentalNavigationMeshFilterPtr filter;

	// runtime-adjustable settings, pushed to the filter by apply_settings();
	// initial values are taken from the engine defaults and the bake settings
	// of the navigation mesh in init_navigation()
	int query_mode{QUERY_MODE_NEAREST_POINT};
	float max_cost{15.f};
	int num_random_points{100};
	bool auto_refresh{true};
	float boundary_search_radius{5.f};
	float box_size{5.f};
	float agent_radius{0.f};
	float agent_height{0.f};
	float snap_size{0.f};

	// sample visualization
	bool show_navigation_mesh{true};

	// cached batch for the random point modes; the batch is re-rolled when
	// the query point moves, a setting changes, or the refresh timer expires
	Unigine::Vector<Unigine::Math::Vec3> random_points;
	Unigine::Math::Vec3 last_query_position{Unigine::Math::Vec3_zero};
	float refresh_timer{0.f};
	bool queries_dirty{true};

	// cached navigation mesh geometry used to highlight the mesh surface,
	// invalidated when the mesh tiles change
	Unigine::Vector<Unigine::Math::Vec3> mesh_vertices;
	Unigine::Vector<int> mesh_indices;
	Unigine::Vector<int> mesh_areas;
	bool mesh_geometry_dirty{true};
	Unigine::EventConnections mesh_connections;

	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator{nullptr};

	// per-mode parameter grids, only the grid of the current mode is shown
	Unigine::Vector<Unigine::WidgetGridBoxPtr> mode_grids;
	// refreshers for the widgets that show the same setting in several modes
	Unigine::Vector<std::function<void()>> ui_sync;
	Unigine::EventConnections widget_connections;
};


REGISTER_COMPONENT(ExperimentalNavigationMeshQueriesLogic);

using namespace Unigine;
using namespace Unigine::Math;

// period of re-rolling the random point batches when auto refresh is enabled
static constexpr float RANDOM_REFRESH_INTERVAL = 0.5f;

//////////////////////////////////////////////////////////////////////////
// Navigation mesh queries.
//
// Self-contained input -> result wrappers around the query API of
// ExperimentalNavigationMesh with no UI or visualization dependencies:
// any of them can be copied into a project together with its result struct.
//////////////////////////////////////////////////////////////////////////

// Snaps a point onto the navigation mesh and reports the polygon it landed on.
struct NearestPointResult
{
	bool found{false};
	Unigine::Math::Vec3 point{Unigine::Math::Vec3_zero};
	long long polygon{0};
};

static NearestPointResult query_nearest_point(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &position)
{
	NearestPointResult result;
	result.found = mesh->findNearestPoint(filter, position, result.point);
	if (result.found)
		result.polygon = mesh->findNearestPolygon(filter, result.point);
	return result;
}

// Samples the points reachable from a point within the route cost limit.
struct ReachablePointsResult
{
	Unigine::Vector<Unigine::Math::Vec3> points;
	Unigine::Vector<long long> polygons;
};

static ReachablePointsResult query_reachable_points(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &position, float max_cost)
{
	ReachablePointsResult result;
	mesh->findReachablePoints(filter, position, max_cost, result.points, result.polygons);
	return result;
}

// Generates a batch of random points over the whole navigation mesh.
static Vector<Vec3> query_random_points(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, int count)
{
	Vector<Vec3> points;
	for (int i = 0; i < count; i += 1)
	{
		Vec3 point;
		if (mesh->findRandomPoint(filter, point))
			points.append(point);
	}
	return points;
}

// Generates a batch of random points reachable from a point within the route
// cost limit.
static Vector<Vec3> query_random_reachable_points(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &position, float max_cost, int count)
{
	Vector<Vec3> points;
	for (int i = 0; i < count; i += 1)
	{
		Vec3 point;
		if (mesh->findRandomReachablePoint(filter, position, max_cost, point))
			points.append(point);
	}
	return points;
}

// Finds the nearest navigation mesh boundary within the search radius.
struct NearestBoundaryResult
{
	bool found{false};
	Unigine::Math::Vec3 point{Unigine::Math::Vec3_zero};
	Unigine::Math::vec3 normal{Unigine::Math::vec3_zero};
	float distance{0.f};
};

static NearestBoundaryResult query_nearest_boundary(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &position, float max_radius)
{
	NearestBoundaryResult result;
	result.found = mesh->getNearestBoundary(filter, position, max_radius, result.point, result.normal, result.distance);
	return result;
}

// Casts a segment along the navigation mesh surface and reports where it
// crosses the mesh boundary. When nothing is hit, start_on_mesh tells whether
// the whole segment stays on the mesh or the start point is off the mesh.
struct BoundaryRaycastResult
{
	bool hit{false};
	bool start_on_mesh{false};
	Unigine::Math::Vec3 point{Unigine::Math::Vec3_zero};
	Unigine::Math::vec3 normal{Unigine::Math::vec3_zero};
};

static BoundaryRaycastResult query_boundary_raycast(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &from, const Vec3 &to)
{
	BoundaryRaycastResult result;
	result.hit = mesh->getBoundaryIntersection(filter, from, to, result.point, result.normal);
	result.start_on_mesh = result.hit || mesh->isPolygonValid(mesh->findNearestPolygon(filter, from));
	return result;
}

// Collects the navigation mesh polygons overlapping a box around a point.
static Vector<long long> query_polygons_in_box(const ExperimentalNavigationMeshPtr &mesh,
	const ExperimentalNavigationMeshFilterPtr &filter, const Vec3 &center, float box_size)
{
	Vec3 half_size = Vec3(box_size * 0.5f);
	Vector<long long> polygons;
	mesh->getPolygons(filter, WorldBoundBox(center - half_size, center + half_size), polygons);
	return polygons;
}

// Collects the triangles of the navigation mesh geometry (obtained via
// ExperimentalNavigationMesh::getTriangles) whose centroids fall inside a box
// around a point; returns the numbers of the matching triangles.
static Vector<int> collect_triangles_in_box(const Vector<Vec3> &vertices, const Vector<int> &indices,
	const Vec3 &center, float box_size)
{
	Vec3 box_min = center - Vec3(box_size * 0.5f);
	Vec3 box_max = center + Vec3(box_size * 0.5f);

	Vector<int> triangles;
	int num_triangles = indices.size() / 3;
	for (int i = 0; i < num_triangles; i += 1)
	{
		Vec3 centroid = (vertices[indices[i * 3 + 0]]
			+ vertices[indices[i * 3 + 1]]
			+ vertices[indices[i * 3 + 2]]) / Scalar(3);

		if (centroid.x < box_min.x || centroid.x > box_max.x
			|| centroid.y < box_min.y || centroid.y > box_max.y
			|| centroid.z < box_min.z || centroid.z > box_max.z)
			continue;

		triangles.append(i);
	}
	return triangles;
}

//////////////////////////////////////////////////////////////////////////
// Sample logic.
//////////////////////////////////////////////////////////////////////////

void ExperimentalNavigationMeshQueriesLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	init_navigation();

	// manipulators move the query points, scaling is disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setXAxisScale(false);
		widget_manipulator->setYAxisScale(false);
		widget_manipulator->setZAxisScale(false);
	}

	create_ui();
}

void ExperimentalNavigationMeshQueriesLogic::update()
{
	// disable player controls while dragging manipulators
	if (widget_manipulator)
		Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	if (!navigation_mesh || !query_point)
	{
		sample_description_window.setStatus("No navigation mesh or query point assigned.");
		return;
	}

	if (show_navigation_mesh)
		navigation_mesh->renderVisualizer();

	// green marker at the exact position the queries are made from,
	// drawn on top so it is always visible
	Vec3 query_position = query_point->getWorldPosition();
	Visualizer::renderSolidBox(vec3(0.5f, 0.5f, 0.04f), translate(query_position), vec4_green, Game::getIFps(), false);

	// re-roll the random point batches when the query point moves
	// or periodically while auto refresh is enabled
	if ((query_position - last_query_position).length2() > 1e-8)
		queries_dirty = true;
	last_query_position = query_position;

	refresh_timer += Game::getIFps();
	if (auto_refresh && refresh_timer >= RANDOM_REFRESH_INTERVAL)
		queries_dirty = true;

	String status;
	switch (query_mode)
	{
		case QUERY_MODE_NEAREST_POINT: status = update_nearest_point(); break;
		case QUERY_MODE_REACHABLE_POINTS: status = update_reachable_points(); break;
		case QUERY_MODE_RANDOM_REACHABLE_POINTS: status = update_random_points(true); break;
		case QUERY_MODE_RANDOM_POINTS: status = update_random_points(false); break;
		case QUERY_MODE_NEAREST_BOUNDARY: status = update_nearest_boundary(); break;
		case QUERY_MODE_BOUNDARY_RAYCAST: status = update_boundary_raycast(); break;
		case QUERY_MODE_POLYGONS_IN_BOX: status = update_polygons_in_box(); break;
	}

	String header = String::format(
		"Mesh polygons: %d\n"
		"Walkable area: %.1f m2\n"
		"Tiles loaded: %d/%d\n\n",
		navigation_mesh->getNumPolygons(),
		navigation_mesh->getWalkableArea(),
		navigation_mesh->getNumLoadedTiles(),
		navigation_mesh->getNumTiles());

	sample_description_window.setStatus(header + status);
}

void ExperimentalNavigationMeshQueriesLogic::shutdown()
{
	widget_connections.disconnectAll();
	mesh_connections.disconnectAll();
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Creates the navigation mesh filter used by all queries.
void ExperimentalNavigationMeshQueriesLogic::init_navigation()
{
	navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation.get());

	filter = ExperimentalNavigationMeshFilter::create();

	// start with the parameters the navigation mesh was baked with,
	// so the mesh matches the filter by default
	if (navigation_mesh)
	{
		filter->setNavigationMask(navigation_mesh->getNavigationMask());

		auto bake_settings = navigation_mesh->getBakeSettings();
		filter->setAgentRadius(bake_settings->getAgentRadius());
		filter->setAgentHeight(bake_settings->getAgentHeight());

		// the cached mesh geometry becomes stale when the tiles change
		navigation_mesh->getEventTilesChanged().connect(mesh_connections,
			[this](const WorldBoundBox &) { mesh_geometry_dirty = true; });
	}

	agent_radius = filter->getAgentRadius();
	agent_height = filter->getAgentHeight();
	snap_size = filter->getSnapSize().z;

	apply_settings();
}

// Pushes the current settings to the filter.
void ExperimentalNavigationMeshQueriesLogic::apply_settings()
{
	filter->setAgentRadius(agent_radius);
	filter->setAgentHeight(agent_height);
	filter->setSnapSize(vec3(snap_size));

	queries_dirty = true;
}

String ExperimentalNavigationMeshQueriesLogic::update_nearest_point()
{
	Vec3 position = query_point->getWorldPosition();

	NearestPointResult result = query_nearest_point(navigation_mesh, filter, position);
	if (!result.found)
		return String("Nearest point: not found, move the query point closer to the mesh or increase the snap size\n");

	Visualizer::renderLine3D(position, result.point, query_color, Game::getIFps());
	Visualizer::renderPoint3D(result.point, 0.3f, query_color, false, 0.f, false);

	String status = String::format(
		"Nearest point: %.2f %.2f %.2f\n"
		"Distance: %.2f\n",
		result.point.x, result.point.y, result.point.z,
		(float)length(result.point - position));

	status += String("\n");
	status += get_polygon_info("Point polygon", result.polygon);
	return status;
}

// The reachable points are colored by the area of the polygon they belong to.
String ExperimentalNavigationMeshQueriesLogic::update_reachable_points()
{
	Vec3 position = query_point->getWorldPosition();

	ReachablePointsResult result = query_reachable_points(navigation_mesh, filter, position, max_cost);

	for (int i = 0; i < result.points.size(); i += 1)
	{
		vec4 color = query_color;
		if (i < result.polygons.size())
		{
			color = ExperimentalNavigation::getAreaColor(navigation_mesh->getPolygonAreaIndex(result.polygons[i]));
			color.w = 1.f;
		}
		Visualizer::renderPoint3D(result.points[i], 0.15f, color);
	}

	// the reference circle shows the theoretical maximum: the reachable
	// points stay inside it since the route cost is measured along the mesh
	// surface around obstacles and is scaled by the area costs
	vec4 circle_color = query_color;
	circle_color.w = 0.4f;
	Visualizer::renderCircle(max_cost, translate(position), circle_color, Game::getIFps());

	return String::format(
		"Reachable points: %d\n"
		"Cost limit: %.1f\n",
		result.points.size(), max_cost);
}

String ExperimentalNavigationMeshQueriesLogic::update_random_points(bool reachable_only)
{
	Vec3 position = query_point->getWorldPosition();

	if (queries_dirty)
	{
		random_points = reachable_only
			? query_random_reachable_points(navigation_mesh, filter, position, max_cost, num_random_points)
			: query_random_points(navigation_mesh, filter, num_random_points);
		queries_dirty = false;
		refresh_timer = 0.f;
	}

	for (const Vec3 &point : random_points)
		Visualizer::renderPoint3D(point, 0.15f, query_color);

	String status = String::format(
		"Points requested: %d\n"
		"Points generated: %d\n",
		num_random_points, random_points.size());

	if (reachable_only)
	{
		vec4 circle_color = query_color;
		circle_color.w = 0.4f;
		Visualizer::renderCircle(max_cost, translate(position), circle_color, Game::getIFps());
		status += String::format("Cost limit: %.1f\n", max_cost);
	}

	return status;
}

String ExperimentalNavigationMeshQueriesLogic::update_nearest_boundary()
{
	Vec3 position = query_point->getWorldPosition();

	vec4 circle_color = query_color;
	circle_color.w = 0.4f;
	Visualizer::renderCircle(boundary_search_radius, translate(position), circle_color, Game::getIFps());

	NearestBoundaryResult result = query_nearest_boundary(navigation_mesh, filter, position, boundary_search_radius);
	if (!result.found)
		return String("Nearest boundary: not found within the search radius\n");

	Visualizer::renderLine3D(position, result.point, query_color, Game::getIFps());
	Visualizer::renderPoint3D(result.point, 0.3f, vec4_red, false, 0.f, false);
	Visualizer::renderVector(result.point, result.point + Vec3(result.normal), vec4_blue, 0.25f, false, Game::getIFps());

	return String::format(
		"Nearest boundary: %.2f %.2f %.2f\n"
		"Distance: %.2f\n"
		"Normal: %.2f %.2f %.2f\n",
		result.point.x, result.point.y, result.point.z,
		result.distance,
		result.normal.x, result.normal.y, result.normal.z);
}

String ExperimentalNavigationMeshQueriesLogic::update_boundary_raycast()
{
	if (!raycast_target)
		return String("Assign the raycast target node to use this mode.\n");

	Vec3 from = query_point->getWorldPosition();
	Vec3 to = raycast_target->getWorldPosition();

	BoundaryRaycastResult result = query_boundary_raycast(navigation_mesh, filter, from, to);
	if (result.hit)
	{
		// walkable part of the segment with the query color,
		// the part beyond the boundary in red
		Visualizer::renderLine3D(from, result.point, query_color, Game::getIFps());
		Visualizer::renderLine3D(result.point, to, vec4_red, Game::getIFps());
		Visualizer::renderPoint3D(result.point, 0.3f, vec4_red, false, 0.f, false);
		Visualizer::renderVector(result.point, result.point + Vec3(result.normal), vec4_blue, 0.25f, false, Game::getIFps());

		return String::format(
			"Boundary hit: yes\n"
			"Hit point: %.2f %.2f %.2f\n"
			"Hit distance: %.2f\n"
			"Normal: %.2f %.2f %.2f\n",
			result.point.x, result.point.y, result.point.z,
			(float)length(result.point - from),
			result.normal.x, result.normal.y, result.normal.z);
	}

	if (result.start_on_mesh)
	{
		Visualizer::renderLine3D(from, to, query_color, Game::getIFps());
		return String("Boundary hit: no, the segment does not cross the mesh boundary\n");
	}

	Visualizer::renderLine3D(from, to, vec4_red, Game::getIFps());
	return String("Boundary hit: no, the start point is outside the navigation mesh\n");
}

// The polygons are queried by box overlap, the mesh surface inside the box is
// highlighted with the cached mesh triangles colored by their areas.
String ExperimentalNavigationMeshQueriesLogic::update_polygons_in_box()
{
	Vec3 position = query_point->getWorldPosition();

	Vector<long long> polygons = query_polygons_in_box(navigation_mesh, filter, position, box_size);

	if (mesh_geometry_dirty)
	{
		mesh_vertices.clear();
		mesh_indices.clear();
		mesh_areas.clear();
		navigation_mesh->getTriangles(mesh_vertices, mesh_indices, mesh_areas);
		mesh_geometry_dirty = false;
	}

	// sample-grade visualization: the whole triangle list is filtered every
	// frame, which is fine for a sample-sized mesh
	Vector<int> triangles = collect_triangles_in_box(mesh_vertices, mesh_indices, position, box_size);
	const Vec3 lift = Vec3(0.f, 0.f, 0.03f);
	for (int triangle : triangles)
	{
		vec4 color = vec4_green;
		if (triangle < mesh_areas.size())
			color = ExperimentalNavigation::getAreaColor(mesh_areas[triangle]);
		color.w = 0.45f;

		Visualizer::renderTriangle3D(
			mesh_vertices[mesh_indices[triangle * 3 + 0]] + lift,
			mesh_vertices[mesh_indices[triangle * 3 + 1]] + lift,
			mesh_vertices[mesh_indices[triangle * 3 + 2]] + lift,
			color, Game::getIFps());
	}

	Visualizer::renderBoundBox(BoundBox(vec3(-box_size * 0.5f), vec3(box_size * 0.5f)), translate(position), query_color, Game::getIFps());

	// the counts may differ: polygons are queried by box overlap while the
	// triangles are highlighted by their centroids being inside the box
	String status = String::format(
		"Polygons in box: %d\n"
		"Highlighted triangles: %d\n"
		"Box size: %.1f\n",
		polygons.size(), triangles.size(), box_size);

	// only the polygon nearest to the query point is detailed,
	// there is no aggregate info for the whole box
	status += String("\n");
	status += get_polygon_info("Nearest polygon to the query point", navigation_mesh->findNearestPolygon(filter, position));
	return status;
}

// Formats a titled block with the polygon id, its area (with the cost from
// the area registry) and flags including their registered names.
String ExperimentalNavigationMeshQueriesLogic::get_polygon_info(const char *title, long long polygon) const
{
	if (!navigation_mesh->isPolygonValid(polygon))
		return String::format("%s: none\n", title);

	int area_index = navigation_mesh->getPolygonAreaIndex(polygon);
	int flags = navigation_mesh->getPolygonFlags(polygon);

	String flag_names;
	for (int i = 0; i < ExperimentalNavigation::getNumFlags(); i += 1)
	{
		if ((flags & (1 << i)) == 0)
			continue;
		const char *name = ExperimentalNavigation::getFlagName(i);
		if (name && *name)
		{
			if (!flag_names.empty())
				flag_names += ", ";
			flag_names += name;
		}
	}

	String flags_text = String::format("0x%X", flags);
	if (!flag_names.empty())
		flags_text += String::format(" [%s]", flag_names.get());

	return String::format(
		"%s: %lld\n"
		"  Area: %d \"%s\" (cost %.2f)\n"
		"  Flags: %s\n",
		title,
		polygon,
		area_index,
		ExperimentalNavigation::getAreaName(area_index),
		ExperimentalNavigation::getAreaCost(area_index),
		flags_text.get());
}

//////////////////////////////////////////////////////////////////////////
// Sample UI.
//////////////////////////////////////////////////////////////////////////

void ExperimentalNavigationMeshQueriesLogic::create_ui()
{
	sample_description_window.createWindow();
	auto window = sample_description_window.getWindow();

	// query group: the mode selector followed by the parameter grids of all
	// modes, only the grid of the current mode is visible
	auto query_group = WidgetGroupBox::create("Query", 8, 8);
	window->addChild(query_group, Gui::ALIGN_LEFT);

	auto mode_grid = WidgetGridBox::create(3, 5, 5);
	query_group->addChild(mode_grid, Gui::ALIGN_EXPAND);
	add_switch_parameter(widget_connections, mode_grid,
		"Query mode",
		"The spatial query performed at the query point and visualized.",
		query_mode,
		{"Nearest point", "Reachable points", "Random reachable points", "Random points", "Nearest boundary", "Boundary raycast", "Polygons in box"},
		[this](int mode) {
			query_mode = mode;
			queries_dirty = true;
			update_mode_ui();
		});

	for (int i = 0; i <= QUERY_MODE_POLYGONS_IN_BOX; i += 1)
	{
		auto grid = WidgetGridBox::create(3, 5, 5);
		query_group->addChild(grid, Gui::ALIGN_EXPAND);
		mode_grids.append(grid);
	}

	// the parameters shared by several modes get a widget in the grid of each
	// mode using them; the widgets are refreshed from the variables on a mode
	// switch by update_mode_ui()
	const char *max_cost_tooltip = "The route cost limit of the query. Equals the distance while all area costs are 1.";
	const char *count_tooltip = "The number of points generated by the query.";
	const char *refresh_tooltip = "Re-rolls the random point batch every half a second. The batch is always re-rolled when the query point moves or a setting changes.";

	for (int mode : {(int)QUERY_MODE_REACHABLE_POINTS, (int)QUERY_MODE_RANDOM_REACHABLE_POINTS})
	{
		auto slider = add_float_parameter(widget_connections, mode_grids[mode],
			"Max cost", max_cost_tooltip, max_cost, 0.f, 100.f,
			[this](float value) {
				max_cost = value;
				queries_dirty = true;
			});
		ui_sync.append([this, slider]() { slider->setValue((int)(max_cost * 100.f)); });
	}

	for (int mode : {(int)QUERY_MODE_RANDOM_REACHABLE_POINTS, (int)QUERY_MODE_RANDOM_POINTS})
	{
		auto slider = add_int_parameter(widget_connections, mode_grids[mode],
			"Random points", count_tooltip, num_random_points, 1, 500,
			[this](int value) {
				num_random_points = value;
				queries_dirty = true;
			});
		ui_sync.append([this, slider]() { slider->setValue(num_random_points); });

		auto checkbox = add_bool_parameter(widget_connections, mode_grids[mode],
			"Auto refresh", refresh_tooltip, auto_refresh,
			[this](bool enabled) {
				auto_refresh = enabled;
			});
		ui_sync.append([this, checkbox]() { checkbox->setChecked(auto_refresh); });
	}

	add_float_parameter(widget_connections, mode_grids[QUERY_MODE_NEAREST_BOUNDARY],
		"Boundary radius",
		"The search radius for the nearest boundary query.",
		boundary_search_radius, 0.1f, 20.f,
		[this](float value) {
			boundary_search_radius = value;
		});

	add_float_parameter(widget_connections, mode_grids[QUERY_MODE_POLYGONS_IN_BOX],
		"Box size",
		"The size of the box for the polygons in box query.",
		box_size, 0.1f, 20.f,
		[this](float value) {
			box_size = value;
		});

	// filter group: the settings shared by all query modes, always visible
	auto filter_group = WidgetGroupBox::create("Filter", 8, 8);
	window->addChild(filter_group, Gui::ALIGN_LEFT);

	auto filter_grid = WidgetGridBox::create(3, 5, 5);
	filter_group->addChild(filter_grid, Gui::ALIGN_EXPAND);

	add_float_parameter(widget_connections, filter_grid,
		"Agent radius",
		"The agent radius used to select a suitable navigation mesh.",
		agent_radius, 0.f, 5.f,
		[this](float value) {
			agent_radius = value;
			apply_settings();
		});

	add_float_parameter(widget_connections, filter_grid,
		"Agent height",
		"The agent height used to select a suitable navigation mesh.",
		agent_height, 0.f, 10.f,
		[this](float value) {
			agent_height = value;
			apply_settings();
		});

	add_float_parameter(widget_connections, filter_grid,
		"Snap size",
		"The search extents used to snap the query points onto the navigation mesh.",
		snap_size, 0.f, 10.f,
		[this](float value) {
			snap_size = value;
			apply_settings();
		});

	// visualization group: shared by all query modes, always visible
	auto visualization_group = WidgetGroupBox::create("Visualization", 8, 8);
	window->addChild(visualization_group, Gui::ALIGN_LEFT);

	add_bool_parameter(widget_connections, visualization_group,
		"Show Navigation Mesh",
		"Draws the navigation mesh colored by the areas of its polygons.",
		show_navigation_mesh,
		[this](bool enabled) {
			show_navigation_mesh = enabled;
		});

	update_mode_ui();
}

// Shows only the parameter grid of the current query mode and refreshes the
// widgets duplicated between the modes.
void ExperimentalNavigationMeshQueriesLogic::update_mode_ui()
{
	for (const auto &sync : ui_sync)
		sync();

	for (int i = 0; i < mode_grids.size(); i += 1)
		mode_grids[i]->setHidden(i != query_mode);

	sample_description_window.getWindow()->arrange();
}

// Demonstrates the interoperation between the classic NavigationMesh and the
// experimental navigation system: both nodes are baked from the scene by the
// new baker sharing a common settings block, the classic mesh data can be
// converted into the experimental format and back, and the data can be
// exchanged through a .mesh file.
//
// The conversion operations are self-contained functions that depend only on
// the engine API, so any of them can be copied into a project.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineFileSystem.h>
#include <UniginePathFinding.h>
#include <UnigineExperimentalNavigation.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../menu_ui/SampleWidgets.h"

class ExperimentalNavigationMeshConversionLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshConversionLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, navigation_mesh)
	PROP_PARAM(Node, experimental_navigation_mesh)

private:
	void init();
	void update();
	void shutdown();

	void init_navigation();
	void apply_bake_settings();

	// button handlers
	void on_bake_experimental();
	void on_convert_from_classic();
	void on_bake_classic();
	void on_apply_from_experimental();

	// both file dialogs are created hidden at start with their own callbacks;
	// opening is just unhiding the needed one and giving it the focus
	void create_file_dialogs();

	// sample UI
	void create_ui();
	void update_status();

	Unigine::NavigationMeshPtr classic_mesh;
	Unigine::ExperimentalNavigationMeshPtr experimental_mesh;

	// shared bake settings, pushed to both nodes right before a bake:
	// writing them earlier would invalidate the data loaded from the assets
	float agent_radius{0.f};
	float agent_height{0.f};
	float max_slope_angle{0.f};
	float max_step_height{0.f};
	float cell_size{0.f};
	float cell_height{0.f};
	int tile_resolution{0};
	int partitioning{0};
	float region_min_size{0.f};
	float region_merge_size{0.f};
	float max_edge_error{0.f};
	float detail_sample_distance{0.f};
	float detail_sample_error{0.f};
	bool show_bake_bounds{true};

	Unigine::ExperimentalNavigationBakeQueryPtr bake_query;
	Unigine::String last_action;

	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator{nullptr};
	Unigine::WidgetEditLinePtr save_path_editline;
	Unigine::WidgetEditLinePtr load_path_editline;
	Unigine::WidgetDialogFilePtr save_dialog;
	Unigine::WidgetDialogFilePtr load_dialog;
	Unigine::EventConnections widget_connections;
};


REGISTER_COMPONENT(ExperimentalNavigationMeshConversionLogic);

using namespace Unigine;
using namespace Unigine::Math;

// default directory of the .mesh file dialogs, relative to the data path
static const char *DEFAULT_DIR = "cpp_component_samples/navigation/experimental_navigation_mesh_conversion/";

// the classic node is baked into its own separate asset, so the original
// asset shared with the other samples is left intact
static const char *CLASSIC_BAKED_MESH_FILE = "cpp_component_samples/navigation/experimental_navigation_mesh_conversion/navigation_mesh_baked.mesh";

//////////////////////////////////////////////////////////////////////////
// Conversion operations: no UI dependencies, can be copied into a project.
//////////////////////////////////////////////////////////////////////////

// Bakes the experimental navigation mesh from the scene geometry.
static ExperimentalNavigationBakeQueryPtr bake_experimental_from_scene(const ExperimentalNavigationMeshPtr &experimental)
{
	return ExperimentalBakeNavigation::bakeAsync(experimental);
}

// Bakes the classic navigation mesh from the scene geometry with the new
// baker. The node is repointed to its own asset and cleared first, so the
// original asset shared with the other samples stays intact.
static ExperimentalNavigationBakeQueryPtr bake_classic_with_new_baker(const NavigationMeshPtr &classic)
{
	classic->setMeshPath(CLASSIC_BAKED_MESH_FILE);
	classic->setMesh(Mesh::create());
	return ExperimentalBakeNavigation::bakeAsync(classic);
}

// Converts the classic navigation mesh data into the experimental format:
// the polygon mesh of the classic node is spawned as temporary geometry
// aligned with the experimental node and baked with a dedicated bake mask,
// keeping the rest of the scene out of the bake. The agent radius is zeroed
// for this bake since the classic mesh is already shrunk by it.
static bool convert_classic_to_experimental(const NavigationMeshPtr &classic, const ExperimentalNavigationMeshPtr &experimental)
{
	MeshPtr polygon_mesh = Mesh::create();
	if (classic->getMesh(polygon_mesh) == 0)
		return false;

	ObjectMeshStaticPtr geometry = ObjectMeshStatic::create();
	geometry->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC);
	geometry->applyCopyMeshProceduralForce(polygon_mesh);
	geometry->setImmovable(false);
	geometry->setWorldTransform(experimental->getWorldTransform());

	int temp_bake_mask = 1 << 30;
	for (int i = 0; i < geometry->getNumSurfaces(); i += 1)
	{
		geometry->setExperimentalNavigation(true, i);
		geometry->setExperimentalNavigationBakeMask(temp_bake_mask, i);
	}

	// make the freshly created geometry visible to the baker
	World::updateSpatial();

	int old_bake_mask = experimental->getBakeSettings()->getBakeMask();
	float old_agent_radius = experimental->getBakeSettings()->getAgentRadius();
	experimental->getBakeSettings()->setBakeMask(temp_bake_mask);
	experimental->getBakeSettings()->setAgentRadius(0.0f);
	
	bool baked = ExperimentalBakeNavigation::bakeForce(experimental);

	experimental->getBakeSettings()->setBakeMask(old_bake_mask);
	experimental->getBakeSettings()->setAgentRadius(old_agent_radius);
	geometry.deleteLater();

	return baked;
}

// Applies the experimental navigation mesh data to the classic node in
// memory through the extracted polygon mesh.
static bool apply_experimental_to_classic(const ExperimentalNavigationMeshPtr &experimental, const NavigationMeshPtr &classic)
{
	MeshPtr polygon_mesh = Mesh::create();
	if (!experimental->getMesh(polygon_mesh))
		return false;

	return classic->setMesh(polygon_mesh) != 0;
}

// Saves the polygon mesh of the experimental navigation mesh to a .mesh
// file; the mesh is stored in the local space of the node.
static bool save_experimental_mesh(const ExperimentalNavigationMeshPtr &experimental, const char *path)
{
	MeshPtr polygon_mesh = Mesh::create();
	if (!experimental->getMesh(polygon_mesh))
		return false;
	return polygon_mesh->save(path) != 0;
}

// Loads a .mesh file into the classic navigation mesh.
static bool load_classic_mesh(const NavigationMeshPtr &classic, const char *path)
{
	return classic->loadMesh(path);
}

//////////////////////////////////////////////////////////////////////////
// Sample logic.
//////////////////////////////////////////////////////////////////////////

void ExperimentalNavigationMeshConversionLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	init_navigation();

	// manipulators move the navigation mesh nodes, scaling is disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setXAxisScale(false);
		widget_manipulator->setYAxisScale(false);
		widget_manipulator->setZAxisScale(false);
	}

	create_ui();
}

void ExperimentalNavigationMeshConversionLogic::update()
{
	// disable player controls while dragging manipulators
	if (widget_manipulator)
		Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	if (!classic_mesh || !experimental_mesh)
	{
		sample_description_window.setStatus("Assign both the classic and the experimental navigation mesh nodes.");
		return;
	}

	// both navigation meshes are always visualized, so the results of the
	// operations can be compared side by side
	classic_mesh->renderVisualizer();
	experimental_mesh->renderVisualizer();

	// bake bounds of each node, colored to match its mesh visualization
	if (show_bake_bounds)
	{
		Visualizer::renderBox(classic_mesh->getBakeSettings()->getBakeSize(), classic_mesh->getWorldTransform(), vec4_green, Game::getIFps());
		Visualizer::renderBox(experimental_mesh->getBakeSettings()->getBakeSize(), experimental_mesh->getWorldTransform(), vec4_blue, Game::getIFps());
	}

	// pick up the result of an asynchronous bake
	if (bake_query && bake_query->isReady())
	{
		if (bake_query->getStatus() == ExperimentalNavigationBakeQuery::STATUS_READY)
		{
			last_action = String::format("Bake finished in %.1f s, tiles built: %d.", bake_query->getElapsedTime(), bake_query->getNumBuiltTiles());
		}
		else
			last_action = String::format("Bake failed, failed tiles: %d, failed meshes: %d.", bake_query->getNumFailedTiles(), bake_query->getNumFailedMeshes());
		bake_query.clear();
	}

	update_status();
}

void ExperimentalNavigationMeshConversionLogic::shutdown()
{
	if (save_dialog)
		save_dialog.deleteLater();
	if (load_dialog)
		load_dialog.deleteLater();
	widget_connections.disconnectAll();
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Reads the initial values of the shared settings block from the nodes.
void ExperimentalNavigationMeshConversionLogic::init_navigation()
{
	classic_mesh = checked_ptr_cast<NavigationMesh>(navigation_mesh.get());
	experimental_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(experimental_navigation_mesh.get());

	auto settings = experimental_mesh ? experimental_mesh->getBakeSettings()
		: ExperimentalNavigationBakeSettingsPtr();
	if (!settings)
		return;

	agent_radius = settings->getAgentRadius();
	agent_height = settings->getAgentHeight();
	max_slope_angle = settings->getMaxSlopeAngle();
	max_step_height = settings->getMaxStepHeight();
	cell_size = settings->getCellSize();
	cell_height = settings->getCellHeight();
	tile_resolution = settings->getTileResolution();
	partitioning = settings->getPartitioning();
	region_min_size = settings->getRegionMinSize();
	region_merge_size = settings->getRegionMergeSize();
	max_edge_error = settings->getMaxEdgeError();
	detail_sample_distance = settings->getDetailSampleDistance();
	detail_sample_error = settings->getDetailSampleError();
}

// Pushes the shared settings to both nodes right before a bake;
// the bake size stays per-node.
void ExperimentalNavigationMeshConversionLogic::apply_bake_settings()
{
	ExperimentalNavigationBakeSettingsPtr all_settings[] = {
		classic_mesh ? classic_mesh->getBakeSettings() : ExperimentalNavigationBakeSettingsPtr(),
		experimental_mesh ? experimental_mesh->getBakeSettings() : ExperimentalNavigationBakeSettingsPtr(),
	};

	for (const auto &settings : all_settings)
	{
		if (!settings)
			continue;
		settings->setAgentRadius(agent_radius);
		settings->setAgentHeight(agent_height);
		settings->setMaxSlopeAngle(max_slope_angle);
		settings->setMaxStepHeight(max_step_height);
		settings->setCellSize(cell_size);
		settings->setCellHeight(cell_height);
		settings->setTileResolution(tile_resolution);
		settings->setPartitioning((ExperimentalNavigationBakeSettings::PARTITIONING)partitioning);
		settings->setRegionMinSize(region_min_size);
		settings->setRegionMergeSize(region_merge_size);
		settings->setMaxEdgeError(max_edge_error);
		settings->setDetailSampleDistance(detail_sample_distance);
		settings->setDetailSampleError(detail_sample_error);
	}
}

void ExperimentalNavigationMeshConversionLogic::on_bake_experimental()
{
	if (!experimental_mesh)
		return;
	if (ExperimentalBakeNavigation::isBaking())
	{
		last_action = "The baker is busy, wait for the current bake to finish.";
		return;
	}

	apply_bake_settings();
	bake_query = bake_experimental_from_scene(experimental_mesh);
	last_action = "Baking the experimental navigation mesh from the scene...";
}

void ExperimentalNavigationMeshConversionLogic::on_convert_from_classic()
{
	if (!classic_mesh || !experimental_mesh)
		return;
	if (ExperimentalBakeNavigation::isBaking())
	{
		last_action = "The baker is busy, wait for the current bake to finish.";
		return;
	}

	apply_bake_settings();
	if (convert_classic_to_experimental(classic_mesh, experimental_mesh))
		last_action = "The classic navigation mesh is converted into the experimental format.";
	else
		last_action = "Conversion failed: the classic navigation mesh has no data or the bake failed.";
}

void ExperimentalNavigationMeshConversionLogic::on_bake_classic()
{
	if (!classic_mesh)
		return;
	if (ExperimentalBakeNavigation::isBaking())
	{
		last_action = "The baker is busy, wait for the current bake to finish.";
		return;
	}

	apply_bake_settings();
	bake_query = bake_classic_with_new_baker(classic_mesh);
	last_action = "Baking the classic navigation mesh from the scene with the new baker...";
}

void ExperimentalNavigationMeshConversionLogic::on_apply_from_experimental()
{
	if (!classic_mesh || !experimental_mesh)
		return;

	if (apply_experimental_to_classic(experimental_mesh, classic_mesh))
		last_action = "The experimental navigation mesh is applied to the classic node.";
	else
		last_action = "Applying failed: the experimental navigation mesh has no data.";
}

// Creates both file dialogs hidden and pointed at the default directory.
// The work is done in the OK button callbacks, closing just hides the dialog.
void ExperimentalNavigationMeshConversionLogic::create_file_dialogs()
{
	// saving the experimental navigation mesh
	save_dialog = WidgetDialogFile::create("Save navigation mesh");
	save_dialog->setFilter(".mesh");
	save_dialog->setPath(String::joinPaths(Engine::get()->getDataPath(), DEFAULT_DIR));

	save_dialog->getOkButton()->getEventClicked().connect(widget_connections, [this](const WidgetPtr &, int) {
		String path = save_dialog->getFile();
		if (!path.endsWith(".mesh"))
		{
			path += ".mesh";
			save_dialog->setFile(path);
		}
		if (save_path_editline)
			save_path_editline->setText(path);

		auto split = String::split(path, "/");
		auto filename = split[split.size() - 1];

		if (experimental_mesh && save_experimental_mesh(experimental_mesh, path))
			last_action = String::format("The experimental navigation mesh is saved to \"%s\".", filename);
		else
			last_action = "Saving failed: the experimental navigation mesh has no data or the path is not writable.";

		save_dialog->removeFocus();
		save_dialog->setHidden(true);
	});

	auto close_save = [this](const WidgetPtr &, int) {
		save_dialog->removeFocus();
		save_dialog->setHidden(true);
	};
	save_dialog->getCancelButton()->getEventClicked().connect(widget_connections, close_save);
	save_dialog->getCloseButton()->getEventClicked().connect(widget_connections, close_save);

	WindowManager::getMainWindow()->addChild(save_dialog, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
	save_dialog->setHidden(true);

	// loading a .mesh file into the classic navigation mesh
	load_dialog = WidgetDialogFile::create("Load navigation mesh");
	load_dialog->setFilter(".mesh");
	load_dialog->setPath(String::joinPaths(Engine::get()->getDataPath(), DEFAULT_DIR));

	load_dialog->getOkButton()->getEventClicked().connect(widget_connections, [this](const WidgetPtr &, int) {
		String path = load_dialog->getFile();
		if (load_path_editline)
			load_path_editline->setText(path);

		auto split = String::split(path, "/");
		auto filename = split[split.size() - 1];

		if (!FileSystem::isFileExist(path.get()))
			last_action = String::format("Loading failed: \"%s\" is not a valid asset.", filename);
		else if (classic_mesh && load_classic_mesh(classic_mesh, path))
			last_action = String::format("The classic navigation mesh is loaded from \"%s\".", filename);
		else
			last_action = String::format("Loading failed: cannot read \"%s\".", filename);

		load_dialog->removeFocus();
		load_dialog->setHidden(true);
	});

	auto close_load = [this](const WidgetPtr &, int) {
		load_dialog->removeFocus();
		load_dialog->setHidden(true);
	};
	load_dialog->getCancelButton()->getEventClicked().connect(widget_connections, close_load);
	load_dialog->getCloseButton()->getEventClicked().connect(widget_connections, close_load);

	WindowManager::getMainWindow()->addChild(load_dialog, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
	load_dialog->setHidden(true);
}

void ExperimentalNavigationMeshConversionLogic::update_status()
{
	String status;

	if (experimental_mesh)
	{
		status += String::format(
			"Experimental mesh: %d polygons, walkable area %.1f m2%s\n",
			experimental_mesh->getNumPolygons(),
			experimental_mesh->getWalkableArea(),
			experimental_mesh->isNeedBake() ? ", needs re-bake" : "");
	}

	if (bake_query)
	{
		status += String::format(
			"Baking: %.0f%%, tiles built: %d, pending: %d\n",
			bake_query->getProgress() * 100.f,
			bake_query->getNumBuiltTiles(),
			bake_query->getNumPendingTiles());
	}

	if (!last_action.empty())
	{
		status += String("\n");
		status += last_action;
		status += String("\n");
	}

	sample_description_window.setStatus(status);
}

//////////////////////////////////////////////////////////////////////////
// Sample UI.
//////////////////////////////////////////////////////////////////////////

void ExperimentalNavigationMeshConversionLogic::create_ui()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 600);
	auto window = sample_description_window.getWindow();

	// shared bake settings driving both bakers; the values are pushed to the
	// nodes right before a bake, so tweaking them does not invalidate the
	// currently loaded data
	{
		auto group = WidgetGroupBox::create("Bake settings", 8, 8);
		window->addChild(group, Gui::ALIGN_LEFT);

		auto grid = WidgetGridBox::create(3, 5, 5);
		group->addChild(grid, Gui::ALIGN_EXPAND);

		add_float_parameter(widget_connections, grid, "Agent radius",
			"The agent radius: the walkable surface is shrunk from the boundaries by this distance.",
			agent_radius, 0.f, 2.f,
			[this](float value) { agent_radius = value; });

		add_float_parameter(widget_connections, grid, "Agent height",
			"The agent height: areas with a lower ceiling are excluded from the walkable surface.",
			agent_height, 0.f, 5.f,
			[this](float value) { agent_height = value; });

		add_float_parameter(widget_connections, grid, "Max slope angle",
			"The maximum slope angle (in degrees) considered walkable.",
			max_slope_angle, 0.f, 90.f,
			[this](float value) { max_slope_angle = value; });

		add_float_parameter(widget_connections, grid, "Max step height",
			"The maximum height of a ledge the agent can step over.",
			max_step_height, 0.f, 2.f,
			[this](float value) { max_step_height = value; });

		add_float_parameter(widget_connections, grid, "Cell size",
			"The size of the voxelization cell in the horizontal plane. Smaller cells give a more precise mesh at the cost of the bake time.",
			cell_size, 0.01f, 1.f,
			[this](float value) { cell_size = value; });

		add_float_parameter(widget_connections, grid, "Cell height",
			"The height of the voxelization cell.",
			cell_height, 0.01f, 1.f,
			[this](float value) { cell_height = value; });

		add_int_parameter(widget_connections, grid, "Tile resolution",
			"The size of a navigation mesh tile in cells.",
			tile_resolution, 16, 256,
			[this](int value) { tile_resolution = value; });

		add_switch_parameter(widget_connections, grid, "Partitioning",
			"The region partitioning algorithm: watershed gives the best quality, monotone is the fastest, chunky is a tradeoff for tiled meshes.",
			partitioning, {"Watershed", "Monotone", "Chunky"},
			[this](int value) { partitioning = value; });

		add_float_parameter(widget_connections, grid, "Region min size",
			"Isolated walkable regions smaller than this size are removed.",
			region_min_size, 0.f, 50.f,
			[this](float value) { region_min_size = value; });

		add_float_parameter(widget_connections, grid, "Region merge size",
			"Walkable regions smaller than this size are merged with the neighbors when possible.",
			region_merge_size, 0.f, 100.f,
			[this](float value) { region_merge_size = value; });

		add_float_parameter(widget_connections, grid, "Max edge error",
			"The maximum deviation of the simplified mesh boundary from the raw contour.",
			max_edge_error, 0.f, 5.f,
			[this](float value) { max_edge_error = value; });

		add_float_parameter(widget_connections, grid, "Detail distance",
			"The sampling distance of the detail elevation mesh.",
			detail_sample_distance, 0.f, 16.f,
			[this](float value) { detail_sample_distance = value; });

		add_float_parameter(widget_connections, grid, "Detail error",
			"The maximum elevation error of the detail mesh.",
			detail_sample_error, 0.f, 5.f,
			[this](float value) { detail_sample_error = value; });

		add_bool_parameter(widget_connections, grid, "Show bake bounds",
			"Draws the bake bounds of each navigation mesh: green for the classic node, blue for the experimental one.",
			show_bake_bounds,
			[this](bool enabled) { show_bake_bounds = enabled; });
	}

	// operations on the navigation meshes, one tab per mesh
	auto operations_tabbox = WidgetTabBox::create(4, 4);
	window->addChild(operations_tabbox, Gui::ALIGN_EXPAND);

	// operations on the experimental navigation mesh
	{
		operations_tabbox->setCurrentTab(operations_tabbox->addTab("Experimental Navigation"));

		auto tab = WidgetVBox::create(4, 4);
		operations_tabbox->addChild(tab, Gui::ALIGN_EXPAND);

		add_button(widget_connections, tab, "Bake from scene",
			"Bakes the experimental navigation mesh from the scene geometry asynchronously.",
			[this]() { on_bake_experimental(); });

		add_button(widget_connections, tab, "Convert from classic",
			"Bakes the experimental navigation mesh over the polygon mesh of the classic node, converting its data into the experimental format.",
			[this]() { on_convert_from_classic(); });

		auto grid = WidgetGridBox::create(3, 5, 5);
		tab->addChild(grid, Gui::ALIGN_EXPAND);
		save_path_editline = add_string_field_with_button(widget_connections, grid, "Mesh file",
			"The path of the .mesh file the experimental navigation mesh is saved to.",
			DEFAULT_DIR,
			"Save", "Opens the file dialog to choose where to save the polygon mesh extracted from the experimental navigation mesh.",
			[this]() {
				save_dialog->setHidden(false);
				save_dialog->setPermanentFocus();
			});
	}

	// operations on the classic navigation mesh
	{
		operations_tabbox->setCurrentTab(operations_tabbox->addTab("Classic Navigation"));

		auto tab = WidgetVBox::create(4, 4);
		operations_tabbox->addChild(tab, Gui::ALIGN_EXPAND);

		add_button(widget_connections, tab, "Bake with new baker",
			"Bakes the classic navigation mesh from the scene geometry with the new baker.",
			[this]() { on_bake_classic(); });

		add_button(widget_connections, tab, "Apply from experimental",
			"Sets the polygon mesh extracted from the experimental navigation mesh to the classic node directly in memory.",
			[this]() { on_apply_from_experimental(); });

		auto grid = WidgetGridBox::create(3, 5, 5);
		tab->addChild(grid, Gui::ALIGN_EXPAND);
		load_path_editline = add_string_field_with_button(widget_connections, grid, "Mesh file",
			"The path of the .mesh file loaded into the classic navigation mesh.",
			DEFAULT_DIR,
			"Load", "Opens the file dialog to choose a .mesh file and load it into the classic navigation mesh.",
			[this]() {
				load_dialog->setHidden(false);
				load_dialog->setPermanentFocus();
			});
	}

	operations_tabbox->setCurrentTab(0);

	create_file_dialogs();

	window->arrange();
}

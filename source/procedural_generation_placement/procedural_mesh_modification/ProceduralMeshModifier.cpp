// Modifies procedural mesh geometry every frame with animated wave deformation.
// Demonstrates multi-threaded mesh updates using AsyncQueue for background processing.
// Supports Copy/Move apply modes and manual MeshRender control for VRAM management.

#include "ProceduralMeshModifier.h"

#include <UnigineAsyncQueue.h>
#include <UnigineGame.h>
#include <UnigineProfiler.h>
#include <UnigineThread.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ProceduralMeshModifier);


void ProceduralMeshModifier::init()
{
	// Sample UI with description and controls
	init_gui();

	is_deleted = false;
	updated_meshvram_manual = is_meshvram_manual;

	isize = 30.f / size;

	// Prepare RAM/VRAM mesh objects and the scene object
	mesh_ram = Mesh::create();
	mesh_vram = MeshRender::create();
	object = ObjectMeshStatic::create();
	object->setMeshProceduralMode(current_mode);
	object->setWorldPosition(Vec3_one);
}

void ProceduralMeshModifier::update()
{
	UNIGINE_PROFILER_FUNCTION;

	// Skip if an update/apply is already running
	if (is_running || object->isMeshProceduralActive())
		return;

	is_running = true;
	is_meshvram_manual = updated_meshvram_manual;

	// Recreate object if procedural mode changed
	if (object->getMeshProceduralMode() != current_mode)
	{
		object.deleteLater();
		object = ObjectMeshStatic::create();
		object->setMeshProceduralMode(current_mode);
		object->setWorldPosition(Vec3_one);
	}

	// Choose where to build the mesh: background thread (async) or main thread
	if (is_thread_async)
	{
		// Builds the mesh on a background thread without blocking the main thread.
		// Mesh modifications are processed on a separate thread as long as needed,
		// without impacting performance.
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_BACKGROUND,
			MakeCallback(this, &ProceduralMeshModifier::async_update_ram));
	}
	else
	{
		// Build and apply in this frame on the main thread
		update_ram();
		if (is_meshvram_manual)
			update_vram();
		apply_data();
	}
}

void ProceduralMeshModifier::shutdown()
{
	// Signal for other threads that shutdown() was called on Main thread
	is_deleted = true;

	shutdown_gui();

	// Wait for any active mesh work before clearing resources
	mesh_lock.wait();

	mesh_ram.clear();
	mesh_vram.clear();
	object.deleteLater();
}

void ProceduralMeshModifier::update_mesh(MeshPtr mesh)
{
	UNIGINE_PROFILER_FUNCTION;

	float time = Game::getTime();

	// Ensure exactly one surface and clear previous data
	if (mesh->getNumSurfaces() != 1)
	{
		mesh->clear();
		mesh->addSurface("");
	}
	else
	{
		mesh->clearSurface();
	}

	// Generate a grid of vertices with an animated wave
	auto &vertices = mesh->getVertices();
	vertices.reserve(size * size);

	for (int y = 0; y < size; y++)
	{
		float Y = y * isize - 15.0f;
		float Z = cos(Y + time);

		for (int x = 0; x < size; x++)
		{
			float X = x * isize - 15.0f;
			vertices.append(vec3(X, Y, Z * sin(X + time)));
		}
	}

	// Reserve enough memory for indices so vector won't be reallocated 
	// every time it's capacity ends
	auto &cindices = mesh->getCIndices();
	cindices.reserve((size - 1) * (size - 1) * 6);
	auto &tindices = mesh->getTIndices();
	tindices.reserve((size - 1) * (size - 1) * 6);

	// Append the same indices to both coordinate and triangle index buffers
	auto addIndex = [&cindices, &tindices](int index) {
		cindices.append(index);
		tindices.append(index);
	};

	// Build triangles for each quad in the grid
	for (int y = 0; y < size - 1; y++)
	{
		int offset = size * y;
		for (int x = 0; x < size - 1; x++)
		{
			addIndex(offset);
			addIndex(offset + 1);
			addIndex(offset + size);
			addIndex(offset + size);
			addIndex(offset + 1);
			addIndex(offset + size + 1);
			offset++;
		}
	}

	mesh->createTangents();

	{
		UNIGINE_PROFILER_SCOPED("CreateCollisionData");

		// If you plan to use intersections or collisions with this mesh, 
		// it's recommended to create CollisionData. Otherwise, intersection 
		// and collision checks will be highly inefficient.

		if (is_collision_enabled)
		{
			// Creates both Spatial Tree and Edges 
			// for effective intersection and collision respectively
			mesh->createCollisionData();

			// If needed, you can create only one:

			// mesh->createSpatialTree();		// intersection only
			// mesh->createEdges();				// collision only
		}
		else
			// You can also create a mesh without CollisionData, or remove existing data
			// if it's not needed.  
			// To check whether a mesh has CollisionData, use:
			//		mesh->hasCollisionData();
			//		mesh->hasSpatialTree();
			//		mesh->hasEdges();
			mesh->clearCollisionData();
	}

	// Update bounds after geometry changes
	mesh->createBounds();
}

void ProceduralMeshModifier::update_ram()
{
	UNIGINE_PROFILER_FUNCTION;

	// Return if the main thread side is shutting down
	if (is_deleted.fetch())
		return;

	// Lock mesh_ram so other threads won't interfere mesh update
	ScopedLock sl(mesh_lock);
	update_mesh(mesh_ram);
}

void ProceduralMeshModifier::update_vram()
{
	UNIGINE_PROFILER_FUNCTION;

	// Return if the main thread side is shutting down
	if (is_deleted.fetch())
		return;

	// Upload RAM mesh to VRAM mesh
	ScopedLock sl(mesh_lock);
	mesh_vram->load(mesh_ram);
}

void ProceduralMeshModifier::async_update_ram()
{
	// Build RAM mesh
	update_ram();

	if (is_meshvram_manual)
	{
		// Load MeshRender on the GPU Stream thread when doing it manually
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_GPU_STREAM,
			MakeCallback(this, &ProceduralMeshModifier::async_update_vram));
	}
	else
	{
		// If you don't need to load MeshRender manualy and automatic loading inside
		// apllyMeshProcedural methods is enough, then return to Main thread
		AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_MAIN, MakeCallback([this, check = node]() {
			// Check that sample's logic on Main thread is still alive. If it's not then stop
			// modification and return
			if (!check || check.isDeleted())
				return;
			apply_data();
		}));
	}
}

void ProceduralMeshModifier::async_update_vram()
{
	// Update MeshRender
	update_vram();

	// Return to Main thread to apply new mesh
	AsyncQueue::runAsync(AsyncQueue::ASYNC_THREAD_MAIN, MakeCallback([this, check = node]() {
		// Check that sample's logic on Main thread is still alive. If it's not then stop
		// modification and return
		if (!check || check.isDeleted())
			return;
		apply_data();
	}));
}

// Apply procedural mesh only on the main thread!
void ProceduralMeshModifier::apply_data()
{
	UNIGINE_PROFILER_FUNCTION;

	// In async mode, apply is processed on a separate thread without blocking the main thread.  
	if (is_async_mode)
	{
		if (is_meshvram_manual)
		{
			// Manual MeshRender is supported only with "Move" mode
			object->applyMoveMeshProceduralAsync(mesh_ram, mesh_vram);
		}
		else
		{
			if (is_copy_mode)
				// In "Copy" mode, data from mesh_ram is duplicated for internal use,  
				// while mesh_ram itself remains unchanged.
				object->applyCopyMeshProceduralAsync(mesh_ram, current_mesh_render_flag);
			else
				// In "Move" mode, data is taken from mesh_ram for internal use,  
				// which modifies mesh_ram in the process.
				object->applyMoveMeshProceduralAsync(mesh_ram, current_mesh_render_flag);
		}
	}
	// In force mode, the main thread remains blocked until apply has finished.
	else
	{
		if (is_meshvram_manual)
		{
			object->applyMoveMeshProceduralForce(mesh_ram, mesh_vram);
		}
		else
		{
			if (is_copy_mode)
				object->applyCopyMeshProceduralForce(mesh_ram, current_mesh_render_flag);
			else
				object->applyMoveMeshProceduralForce(mesh_ram, current_mesh_render_flag);
		}
	}

	// Full cycle of mesh modification is finished
	is_running = false;
}

void ProceduralMeshModifier::init_gui()
{
	// Sample UI with description and controls
	sample_description_window.createWindow(Gui::ALIGN_RIGHT);
	auto params = sample_description_window.getParameterGroupBox();

	auto gridbox = WidgetGridBox::create(2, 10);
	params->addChild(gridbox, Gui::ALIGN_EXPAND);

	//	--------Async/Force Mode Selector--------
	auto label = WidgetLabel::create("Thread");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	thread_combo = WidgetComboBox::create();
	thread_combo->addItem("Main");
	thread_combo->addItem("Background");
	gridbox->addChild(thread_combo, Gui::ALIGN_EXPAND);

	thread_combo->setCurrentItem(1);
	thread_combo->getEventChanged().connect(*this, [this]() {
		int item = thread_combo->getCurrentItem();
		is_thread_async = item != 0;
	});

	//	--------Procedural Mode Selector--------
	modes_map["Dynamic"] = ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC;
	modes_map["File"] = ObjectMeshStatic::PROCEDURAL_MODE_FILE;
	modes_map["Blob"] = ObjectMeshStatic::PROCEDURAL_MODE_BLOB;

	label = WidgetLabel::create("Procedural Mode");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	mode_combo = WidgetComboBox::create();
	mode_combo->addItem("Dynamic");
	mode_combo->addItem("File");
	mode_combo->addItem("Blob");
	gridbox->addChild(mode_combo, Gui::ALIGN_EXPAND);

	current_mode = ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC;
	mode_combo->setCurrentItem(0);
	mode_combo->getEventChanged().connect(*this, [this]() {
		auto item = mode_combo->getCurrentItemText();
		current_mode = modes_map[item];
	});

	//	--------MeshRender Flag Selector--------
	usage_map["None"] = 0;
	usage_map["DYNAMIC_VERTEX"] = MeshRender::USAGE_DYNAMIC_VERTEX;
	usage_map["DYNAMIC_INDICES"] = MeshRender::USAGE_DYNAMIC_INDICES;
	usage_map["DYNAMIC_ALL"] = MeshRender::USAGE_DYNAMIC_ALL;

	label = WidgetLabel::create("MeshRender Flag");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	usage_combo = WidgetComboBox::create();
	usage_combo->addItem("None");
	usage_combo->addItem("DYNAMIC_VERTEX");
	usage_combo->addItem("DYNAMIC_INDICES");
	usage_combo->addItem("DYNAMIC_ALL");
	gridbox->addChild(usage_combo, Gui::ALIGN_EXPAND);

	current_mesh_render_flag = 0;
	usage_combo->setCurrentItem(0);
	usage_combo->getEventChanged().connect(*this, [this]() {
		auto item = usage_combo->getCurrentItemText();
		current_mesh_render_flag = usage_map[item];
	});

	//	--------Async/Force Mode Selector--------
	label = WidgetLabel::create("Async Mode");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	async_combo = WidgetComboBox::create();
	async_combo->addItem("Async");
	async_combo->addItem("Force");
	gridbox->addChild(async_combo, Gui::ALIGN_EXPAND);

	async_combo->setCurrentItem(0);
	async_combo->getEventChanged().connect(*this, [this]() {
		int item = async_combo->getCurrentItem();
		is_async_mode = item == 0;
	});

	//	--------MeshRender Dynamic Usage Selector--------
	label = WidgetLabel::create("Apply mode");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	move_combo = WidgetComboBox::create();
	move_combo->addItem("Copy");
	move_combo->addItem("Move");
	gridbox->addChild(move_combo, Gui::ALIGN_EXPAND);

	is_copy_mode = true;
	move_combo->setCurrentItem(0);
	move_combo->getEventChanged().connect(*this, [this]() {
		auto item = move_combo->getCurrentItem();
		is_copy_mode = item == 0;
	});

	//	--------Create Collision Data--------
	label = WidgetLabel::create("Create CollisionData");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	auto collison_checkbox = WidgetCheckBox::create();
	gridbox->addChild(collison_checkbox, Gui::ALIGN_EXPAND);
	collison_checkbox->getEventChanged().connect(*this,
		[this, collison_checkbox]() { is_collision_enabled = collison_checkbox->isChecked(); });

	//	--------Create MeshRender Manualy--------
	label = WidgetLabel::create("Manual MeshRender");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	meshvram_checkbox = WidgetCheckBox::create();
	gridbox->addChild(meshvram_checkbox, Gui::ALIGN_EXPAND);
	meshvram_checkbox->getEventChanged().connect(*this, [this]() {
		updated_meshvram_manual = meshvram_checkbox->isChecked();
		warning_label->setHidden(!updated_meshvram_manual);
	});

	//	--------Create MeshRender Warning--------
	warning_label = WidgetLabel::create("MeshRender can be used only with mode \"Move\". Apply mode "
										"\"Copy\" will be ignored ");
	warning_label->setFontWrap(1);
	warning_label->setHidden(true);
	warning_label->setFontColor(vec4_red);

	params->setFontWrap(1);
	params->addChild(warning_label, Gui::ALIGN_EXPAND);
}

void ProceduralMeshModifier::shutdown_gui()
{
	sample_description_window.shutdown();
}

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Demonstrates real-time procedural mesh modification with proper RAM/VRAM updates.
// An animated wave surface is regenerated each frame using background threads via AsyncQueue.
// Supports multiple update strategies: async/force apply modes, copy/move semantics,
// and optional manual MeshRender creation. Thread synchronization is handled via Mutex
// and AtomicBool to safely coordinate mesh updates across multiple threads.
class ProceduralMeshModifier : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ProceduralMeshModifier, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates procedural mesh generation and proper async RAM/VRAM updates");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	// Build mesh data
	void update_mesh(Unigine::MeshPtr mesh);

	// Update CPU-side Mesh (RAM)
	void update_ram();
	void async_update_ram();

	// Update GPU-side MeshRender (VRAM)
	void update_vram();
	void async_update_vram();

	// Apply updated mesh to the object (respects async/force and copy/move modes)
	void apply_data();

	// Sample UI with description and controls
	void init_gui();
	void shutdown_gui();

private:
	// Grid resolution for generated geometry.
	int size = 128;
	float isize;

	// Scene object and mesh containers
	Unigine::ObjectMeshStaticPtr object;	// Object that receives the procedural mesh
	Unigine::MeshPtr mesh_ram;				// CPU-side mesh data
	Unigine::MeshRenderPtr mesh_vram;		// GPU-side mesh data

	// Parameters to coordinate different threads
	Unigine::Mutex mesh_lock;
	Unigine::AtomicBool is_deleted;

	// Current procedural mode and MeshRender usage flags
	Unigine::ObjectMeshStatic::PROCEDURAL_MODE current_mode;
	int current_mesh_render_flag;

	// Options that define how updates are executed
	bool is_collision_enabled = false;				// Enables creating CollisionData (spatial tree/edges)
	bool is_thread_async = true;					// Enables mesh data creation in async thread
	bool is_async_mode = true;						// Enables apply*Async instead of apply*Force
	bool is_copy_mode = true;						// Enables applyCopy* instead of applyMove*
	Unigine::AtomicBool is_meshvram_manual = false;	// Create MeshRender manually instead of use applyMove* overload
	bool updated_meshvram_manual;					// Lock mesh_vram state to prevent changes during updates on other threads.

	// Prevent starting a new update while the previous one is in progress
	bool is_running = false;

	// Sample UI with description and controls
	SampleDescriptionWindow sample_description_window;

	Unigine::WidgetComboBoxPtr thread_combo;
	Unigine::WidgetComboBoxPtr async_combo;
	Unigine::WidgetComboBoxPtr move_combo;

	Unigine::Map<Unigine::String, Unigine::ObjectMeshStatic::PROCEDURAL_MODE> modes_map;
	Unigine::Map<Unigine::String, int> usage_map;
	Unigine::WidgetComboBoxPtr mode_combo;
	Unigine::WidgetComboBoxPtr usage_combo;

	Unigine::WidgetCheckBoxPtr meshvram_checkbox;

	Unigine::WidgetLabelPtr warning_label;
};

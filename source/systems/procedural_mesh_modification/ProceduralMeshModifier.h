#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ProceduralMeshModifier : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ProceduralMeshModifier, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	// updates geometry
	void update_mesh(Unigine::MeshPtr mesh);

	// updates Mesh
	void update_ram();
	void async_update_ram();

	// updates MeshRender
	void update_vram();
	void async_update_vram();

	// applies updated mesh
	void apply_data();

	// UI methods
	void init_gui();
	void shutdown_gui();

private:
	int size = 128;
	float isize;

	Unigine::ObjectMeshStaticPtr object;
	Unigine::MeshPtr mesh_ram;
	Unigine::MeshRenderPtr mesh_vram;

	// parameters to coordinate different threads
	Unigine::Mutex mesh_lock;
	Unigine::AtomicBool is_deleted;

	Unigine::ObjectMeshStatic::PROCEDURAL_MODE current_mode;
	int current_mesh_render_flag;

	// enables creating CollisionData for mesh
	bool is_collision_enabled = false;
	// enables mesh creation in async thread
	bool is_thread_async = true;
	// enables apply*MeshProceduralAsync instead of apply*MeshProceduralForce
	bool is_async_mode = true;
	// enables applyCopyMeshProcedural* instead of applyMoveMeshProcedural*
	bool is_copy_mode = true;
	// enables creating MeshRender yourself and not by engine inside applyMeshProcedural methods
	Unigine::AtomicBool is_meshvram_manual = false;
	// reserve meshvram manual state, so it won't be changed mid mesh update in different thread
	bool updated_meshvram_manual;

	// check if we already in the process of modifying a mesh
	bool is_running = false;

	// UI
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

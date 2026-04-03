#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Demonstrates asynchronous procedural mesh generation for large object grids.
// Objects are created in bulk using runGenerateMeshProceduralAsync, which builds
// mesh data on background threads. Compares three procedural modes (Dynamic, File, Blob)
// and tracks RAM/VRAM usage via atomic counters for thread-safe statistics.
class ProceduralMeshGenerator : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ProceduralMeshGenerator, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates procedural mesh generation and object management with async updates");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void create_objects();
	void create_mesh(Unigine::MeshPtr mesh);
	void create_done(Unigine::ObjectMeshStaticPtr obj);

	void clear_objects();

	void update_stats();

	void init_gui();
	void shutdown_gui();

	void set_gui_enabled(bool enabled);
	void on_generate_button();
	void on_clear_button();

private:
	// Grid size and object count
	int size = 128;
	int new_size;
	int num_objects;

	// Spacing between objects
	float offset = 0.5f;

	// Use atomic variables since these parameters 
	// are modified by multiple asynchronous threads
	Unigine::AtomicInt64 dynamic_ram = 0;
	Unigine::AtomicInt64 dynamic_vram = 0;
	Unigine::AtomicInt64 num_created_objects = 0;

	// Generation state
	bool is_creating_objects = false;
	bool is_deleting_done = false;

	// Generated objects
	Unigine::Vector<Unigine::ObjectMeshStaticPtr> box_objects;

	// Procedural mode and MeshRender usage flag
	Unigine::ObjectMeshStatic::PROCEDURAL_MODE current_mode;
	Unigine::ObjectMeshStatic::PROCEDURAL_MODE last_mode;
	int current_mesh_render_flag;

	// Sample UI with description and controls
	SampleDescriptionWindow sample_description_window;

	Unigine::Map<Unigine::String, Unigine::ObjectMeshStatic::PROCEDURAL_MODE> modes_map;
	Unigine::Map<Unigine::String, int> flags_map;

	Unigine::WidgetComboBoxPtr mode_combo;
	Unigine::WidgetComboBoxPtr flags_combo;

	Unigine::WidgetEditLinePtr editline;
	Unigine::WidgetSpinBoxPtr spinbox;

	Unigine::WidgetButtonPtr generate_button;
	Unigine::WidgetButtonPtr clear_button;
};

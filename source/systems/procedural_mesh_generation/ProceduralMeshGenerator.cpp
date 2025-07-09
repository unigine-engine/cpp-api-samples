#include "ProceduralMeshGenerator.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ProceduralMeshGenerator);


constexpr float B2MB = 1.f / (1024 * 1024);

void ProceduralMeshGenerator::init()
{
	num_objects = size * size;
	new_size = size;

	init_gui();
}

void ProceduralMeshGenerator::update()
{
	// check that all objects are created
	if (is_creating_objects && num_created_objects == num_objects)
	{
		set_gui_enabled(true);
		is_creating_objects = false;
	}

	// check that deleting of all objects is finished
	if (is_deleting_done)
	{
		is_deleting_done = false;
		if (is_creating_objects)
			create_objects();
		else
			set_gui_enabled(true);
	}

	// change to new filed size if there are no active object creation processes
	if (!is_creating_objects && new_size != size)
	{
		size = new_size;
		num_objects = size * size;
	}

	// update memory usage statistic
	update_stats();
}

void ProceduralMeshGenerator::shutdown()
{
	clear_objects();
	shutdown_gui();
}

void ProceduralMeshGenerator::create_objects()
{
	is_creating_objects = true;

	if (box_objects.size() > 0)
	{
		clear_objects();
		return;
	}

	float field_offset = (1.f + offset) * size / 2.f;

	box_objects.reserve(num_objects);
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			auto obj = ObjectMeshStatic::create();
			obj->setWorldPosition(
				Vec3(x + x * offset - field_offset, y + y * offset - field_offset, 1.5));

			obj->setMeshProceduralMode(current_mode);

			// this method will itself generate new mesh by create_mesh callback, load MeshRender,
			// apply new mesh to object and then call create_done callback when everything is done
			obj->runGenerateMeshProceduralAsync(
				MakeCallback(this, &ProceduralMeshGenerator::create_mesh),
				MakeCallback(this, &ProceduralMeshGenerator::create_done, obj), current_mesh_render_flag);

			box_objects.append(obj);
		}
	}
}

void ProceduralMeshGenerator::create_mesh(MeshPtr mesh)
{
	mesh->addBoxSurface("box", vec3(1));

	// dynamic procedural meshes are constantly stored in memory, so we can calculate it's memory
	// usage right after creation
	if (current_mode != ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC)
		return;
	dynamic_ram += mesh->getMemoryUsage();
}

void ProceduralMeshGenerator::create_done(ObjectMeshStaticPtr obj)
{
	num_created_objects++;

	// dynamic procedural meshes are constantly stored in memory, so we can calculate it's memory
	// usage right after creation; file and blob procedural meshes don't occupy vram till active
	if (current_mode != ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC)
		return;

	MeshRenderPtr m_vram = obj->getMeshCurrentVRAM();
	if (m_vram)
	{
		dynamic_ram += m_vram->getSystemMemoryUsage();
		dynamic_vram += m_vram->getVideoMemoryUsage();
	}
}

void ProceduralMeshGenerator::clear_objects()
{
	num_created_objects = 0;
	dynamic_ram = 0;
	dynamic_vram = 0;

	for (auto &obj : box_objects)
	{
		obj.deleteLater();
	}
	box_objects.clear();
	is_deleting_done = true;
}

void ProceduralMeshGenerator::update_stats()
{
	int ram = 0, active_vram = 0;
	auto &stat = Render::getStreamingStatistics();

	switch (last_mode)
	{
	// Memory usage by Dynamic procedural meshes was already calculated right after meshes creation
	// and is constant throught all the time we use these meshes
	case Unigine::ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC:
		ram = int(dynamic_ram * B2MB);
		active_vram = int(dynamic_vram * B2MB);
		break;
	// File and Blob procedural meshes occupy memory only when thea are activce, otherwise they
	// are unloaded to cache
	case Unigine::ObjectMeshStatic::PROCEDURAL_MODE_BLOB:
	case Unigine::ObjectMeshStatic::PROCEDURAL_MODE_FILE:
		ram = int(stat.static_meshes.ram.active * B2MB);
		active_vram = int(stat.static_meshes.vram.active * B2MB);
		break;
	default: break;
	}

	String status = String::format("Num ready objects: %d\nRAM: %d MB\nVRAM Active: %d MB",
		num_created_objects.fetch(), ram, active_vram);
	sample_description_window.setStatus(status);
}

void ProceduralMeshGenerator::init_gui()
{
	sample_description_window.createWindow(Gui::ALIGN_RIGHT);
	auto params = sample_description_window.getParameterGroupBox();

	auto gridbox = WidgetGridBox::create(2, 10);
	params->addChild(gridbox);

	//	--------Procedural Mode Selector--------
	modes_map["Dynamic"] = ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC;
	modes_map["File"] = ObjectMeshStatic::PROCEDURAL_MODE_FILE;
	modes_map["Blob"] = ObjectMeshStatic::PROCEDURAL_MODE_BLOB;

	auto label = WidgetLabel::create("Procedural Mode");
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

	//	--------MeshRender Dynamic Usage Selector--------
	flags_map["None"] = 0;
	flags_map["DYNAMIC_VERTEX"] = MeshRender::USAGE_DYNAMIC_VERTEX;
	flags_map["DYNAMIC_INDICES"] = MeshRender::USAGE_DYNAMIC_INDICES;
	flags_map["DYNAMIC_ALL"] = MeshRender::USAGE_DYNAMIC_ALL;

	label = WidgetLabel::create("MeshRender Flag");
	gridbox->addChild(label, Gui::ALIGN_LEFT);
	
	flags_combo = WidgetComboBox::create();
	flags_combo->addItem("None");
	flags_combo->addItem("DYNAMIC_VERTEX");
	flags_combo->addItem("DYNAMIC_INDICES");
	flags_combo->addItem("DYNAMIC_ALL");
	gridbox->addChild(flags_combo, Gui::ALIGN_EXPAND);
	
	current_mesh_render_flag = 0;
	flags_combo->setCurrentItem(0);
	flags_combo->getEventChanged().connect(*this, [this]() {
		auto item = flags_combo->getCurrentItemText();
		current_mesh_render_flag = flags_map[item];
	});

	//	--------Fiels Size Input--------
	label = WidgetLabel::create("Field Size");
	gridbox->addChild(label, Gui::ALIGN_LEFT);

	auto spinbox_hbox = WidgetHBox::create();
	gridbox->addChild(spinbox_hbox, Gui::ALIGN_EXPAND);

	spinbox = WidgetSpinBox::create(1, 1000);
	spinbox->setValue(size);
	spinbox->getEventChanged().connect(*this, [this]() { new_size = spinbox->getValue(); });

	editline = WidgetEditLine::create();
	editline->setValidator(Gui::VALIDATOR_UINT);
	editline->getEventFocusOut().connect(*this, [this]() {
		int text = atoi(editline->getText());
		new_size = clamp(text, spinbox->getMinValue(), spinbox->getMaxValue());
		if (text != new_size)
			editline->setText(String::itoa(new_size));
	});
	editline->setText(String::itoa(size));
	editline->addAttach(spinbox);

	spinbox_hbox->addChild(editline);
	spinbox_hbox->addChild(spinbox, Gui::ALIGN_EXPAND);

	//	--------Spacer--------
	auto spacer = WidgetSpacer::create();
	spacer->setOrientation(1);
	params->addChild(spacer, Gui::ALIGN_EXPAND);

	//	--------Create buttons--------
	auto hbox = WidgetHBox::create(10);
	params->addChild(hbox, Gui::ALIGN_EXPAND);
	generate_button = WidgetButton::create("Generate");
	generate_button->getEventClicked().connect(this, &ProceduralMeshGenerator::on_generate_button);
	hbox->addChild(generate_button, Gui::ALIGN_EXPAND);
	clear_button = WidgetButton::create("Clear");
	clear_button->getEventClicked().connect(this, &ProceduralMeshGenerator::on_clear_button);
	hbox->addChild(clear_button, Gui::ALIGN_EXPAND);
}

void ProceduralMeshGenerator::shutdown_gui()
{
	sample_description_window.shutdown();
	modes_map.clear();
	flags_map.clear();
}

void ProceduralMeshGenerator::set_gui_enabled(bool enabled)
{
	generate_button->setEnabled(enabled);
	clear_button->setEnabled(enabled);
}

void ProceduralMeshGenerator::on_generate_button()
{
	set_gui_enabled(false);

	last_mode = current_mode;

	create_objects();
}

void ProceduralMeshGenerator::on_clear_button()
{
	set_gui_enabled(false);
	clear_objects();
}

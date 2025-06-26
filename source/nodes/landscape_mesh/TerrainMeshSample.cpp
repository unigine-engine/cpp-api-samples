#include "TerrainMeshSample.h"

#include <UnigineVisualizer.h>

REGISTER_COMPONENT(TerrainMeshSample);


using namespace Unigine;
using namespace Math;

void TerrainMeshSample::init()
{
	Visualizer::setEnabled(true);
	terrain_mesh = getComponent<TerrainMesh>(terrain_mesh_prop.get());
	update_terrain_mesh_parameters();
	init_gui();
}

void TerrainMeshSample::update()
{
	status_label->setText(String::format("Status: %i/%i",
		terrain_mesh->getCompletedFetchPositions(), terrain_mesh->getTotalFetchPositions()));
}

void TerrainMeshSample::shutdown()
{
	Visualizer::setEnabled(false);
	shutdown_gui();
}

void TerrainMeshSample::update_terrain_mesh_parameters()
{
	Mat4 transform;
	composeTransform(transform, Vec3(position.x, position.y, 0), quat(0.0f, 0.0f, rotation),
		vec3(scale.x, scale.y, 1.0f));
	terrain_mesh->getNode()->setTransform(transform);

	terrain_mesh->setResolution(resolution);
	terrain_mesh->setMaxFetchers(concurrent_fetches);
}

void TerrainMeshSample::init_gui()
{
	sample_description_window.createWindow();
	window = sample_description_window.getWindow();

	auto group_box = sample_description_window.getParameterGroupBox();

	auto hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create("Position");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("X");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	position_x_edit_line = WidgetEditLine::create(
		String::ftoa(static_cast<float>(terrain_mesh->getNode()->getPosition().x)));
	position_x_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	position_x_edit_line->getEventChanged().connect(this, &TerrainMeshSample::position_x_edit_line_callback);
	hbox->addChild(position_x_edit_line, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Y");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	position_y_edit_line = WidgetEditLine::create(
		String::ftoa(static_cast<float>(terrain_mesh->getNode()->getPosition().y)));
	position_y_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	position_y_edit_line->getEventChanged().connect(this, &TerrainMeshSample::position_y_edit_line_callback);
	hbox->addChild(position_y_edit_line, Gui::ALIGN_LEFT);


	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Size");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("X");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	size_x_edit_line = WidgetEditLine::create(String::ftoa(terrain_mesh->getNode()->getScale().x));
	size_x_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	size_x_edit_line->getEventChanged().connect(this, &TerrainMeshSample::size_x_edit_line_callback);
	hbox->addChild(size_x_edit_line, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Y");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	size_y_edit_line = WidgetEditLine::create(String::ftoa(terrain_mesh->getNode()->getScale().y));
	size_y_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	size_y_edit_line->getEventChanged().connect(this, &TerrainMeshSample::size_y_edit_line_callback);
	hbox->addChild(size_y_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Angle");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	rotation_edit_line = WidgetEditLine::create(
		String::ftoa(terrain_mesh->getNode()->getRotation().getAngle(vec3_up)));
	rotation_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	rotation_edit_line->getEventChanged().connect(this, &TerrainMeshSample::rotation_edit_line_callback);
	hbox->addChild(rotation_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Resolution");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("X");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	resolution_x_edit_line = WidgetEditLine::create(String::itoa(resolution.x));
	resolution_x_edit_line->setValidator(Gui::VALIDATOR_UINT);
	resolution_x_edit_line->getEventChanged().connect(this, &TerrainMeshSample::resolution_x_edit_line_callback);
	hbox->addChild(resolution_x_edit_line, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Y");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	resolution_y_edit_line = WidgetEditLine::create(String::itoa(resolution.y));
	resolution_y_edit_line->setValidator(Gui::VALIDATOR_UINT);
	resolution_y_edit_line->getEventChanged().connect(this, &TerrainMeshSample::resolution_y_edit_line_callback);
	hbox->addChild(resolution_y_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Concurrent fetches");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	concurrent_fetches_edit_line = WidgetEditLine::create(String::itoa(concurrent_fetches));
	concurrent_fetches_edit_line->setValidator(Gui::VALIDATOR_UINT);
	concurrent_fetches_edit_line->getEventChanged().connect(this, &TerrainMeshSample::concurrent_fetches_edit_line_callback);
	hbox->addChild(concurrent_fetches_edit_line, Gui::ALIGN_LEFT);


	draw_bounding_box_check_box = WidgetCheckBox::create("Draw Bounding Box");
	draw_bounding_box_check_box->setChecked(true);
	draw_bounding_box_check_box->getEventChanged().connect(this, &TerrainMeshSample::draw_bounding_box_check_box_callback);
	group_box->addChild(draw_bounding_box_check_box, Gui::ALIGN_LEFT);

	draw_wireframe_check_box = WidgetCheckBox::create("Draw Wireframe");
	draw_wireframe_check_box->getEventChanged().connect(this, &TerrainMeshSample::draw_wireframe_check_box_callback);
	group_box->addChild(draw_wireframe_check_box, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	auto button = WidgetButton::create("Generate");
	button->getEventClicked().connect(this, &TerrainMeshSample::generate_button_callback);
	hbox->addChild(button, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	status_label = WidgetLabel::create("Status: 0/0");
	hbox->addChild(status_label, Gui::ALIGN_LEFT);

	window->arrange();
}

void TerrainMeshSample::shutdown_gui()
{
	window.deleteLater();
}

void TerrainMeshSample::position_x_edit_line_callback()
{
	position.x = String::atof(position_x_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::position_y_edit_line_callback()
{
	position.y = String::atof(position_y_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::size_x_edit_line_callback()
{
	scale.x = String::atof(size_x_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::size_y_edit_line_callback()
{
	scale.y = String::atof(size_y_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::rotation_edit_line_callback()
{
	rotation = String::atof(rotation_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::resolution_x_edit_line_callback()
{
	resolution.x = String::atoi(resolution_x_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::resolution_y_edit_line_callback()
{
	resolution.y = String::atoi(resolution_y_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::concurrent_fetches_edit_line_callback()
{
	concurrent_fetches = String::atoi(concurrent_fetches_edit_line->getText());
	update_terrain_mesh_parameters();
}

void TerrainMeshSample::draw_bounding_box_check_box_callback()
{
	terrain_mesh->setDrawBoundingBox(draw_bounding_box_check_box->isChecked());
}

void TerrainMeshSample::draw_wireframe_check_box_callback()
{
	terrain_mesh->setDrawWireframe(draw_wireframe_check_box->isChecked());
}

void TerrainMeshSample::generate_button_callback()
{
	terrain_mesh->run();
}

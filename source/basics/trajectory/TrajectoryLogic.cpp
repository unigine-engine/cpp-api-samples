#include "TrajectoryLogic.h"

#include "TrajectoryMovement.h"
#include "UnigineVisualizer.h"
#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(TrajectoryLogic);

void TrajectoryLogic::init()
{
	Game::getMainPlayers(main_players);

	UNIGINE_ASSERT(main_players.size() >= (TOTAL_PLAYERS - 1));
	Game::setPlayer(main_players[current_player]);

	init_gui();
	Visualizer::setEnabled(true);
}

void TrajectoryLogic::update() {}

void TrajectoryLogic::shutdown()
{
	sample_description_window.shutdown();
	Visualizer::setEnabled(false);
}

void TrajectoryLogic::init_gui()
{

	sample_description_window.createWindow();

	sample_description_window.addFloatParameter("Velocity", "Velicity", 5.f, 1.f, 50.f, [this](float v) {
		getComponent<SimpleTrajectoryMovement>(airplane_1)->velocity = v;
		getComponent<SplineTrajectoryMovement>(airplane_2)->velocity = v;
		getComponent<SavedPathTrajectory>(airplane_3)->velocity = v;
	});

	auto parameters = sample_description_window.getParameterGroupBox();

	auto change_camera_button = WidgetButton::create();
	parameters->addChild(change_camera_button, Gui::ALIGN_LEFT);
	change_camera_button->setText("Switch Camera");
	change_camera_button->getEventClicked().connect(this, &TrajectoryLogic::switch_trajectory_callback);

	auto gridbox = WidgetGridBox::create();
	parameters->addChild(gridbox, Gui::ALIGN_LEFT);
	enable_visualize_path = WidgetCheckBox::create();
	enable_visualize_path->setChecked(false);
	enable_visualize_path->getEventClicked().connect(this,  &TrajectoryLogic::enable_visualize_callback);
	auto visualize_label = WidgetLabel::create("Visualize Path");
	gridbox->addChild(visualize_label);
	gridbox->addChild(enable_visualize_path);
}

void TrajectoryLogic::update_velocity_callback()
{
	int new_velocity = velocity_widget->getValue();

	velocity_value_label->setText(String::itoa(new_velocity));

	getComponent<SimpleTrajectoryMovement>(airplane_1)->velocity = (float)new_velocity;
	getComponent<SplineTrajectoryMovement>(airplane_2)->velocity = (float)new_velocity;
	getComponent<SavedPathTrajectory>(airplane_3)->velocity = (float)new_velocity;
}

void TrajectoryLogic::switch_trajectory_callback()
{
	current_player = Players((int(current_player) + 1) % TOTAL_PLAYERS);
	Game::setPlayer(main_players[current_player]);
}

void TrajectoryLogic::enable_visualize_callback()
{
	bool enabled = enable_visualize_path->isChecked();
	getComponent<SimpleTrajectoryMovement>(airplane_1)->debug = enabled;
	getComponent<SplineTrajectoryMovement>(airplane_2)->debug = enabled;
	getComponent<SavedPathTrajectory>(airplane_3)->debug = enabled;
}

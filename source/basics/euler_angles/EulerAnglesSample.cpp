#include "EulerAnglesSample.h"

#include "UnigineVisualizer.h"


REGISTER_COMPONENT(EulerAnglesSample);

using namespace Unigine;
using namespace Unigine::Math;



void EulerAnglesSample::init()
{
	init_gui();
	update_decomposition_angles();

	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);
}

void EulerAnglesSample::update()
{
	Vec3 position = plane->getWorldPosition();
	vec3 direction_x = plane->getWorldDirection(Math::AXIS_X);
	vec3 direction_y = plane->getWorldDirection(Math::AXIS_Y);
	vec3 direction_z = plane->getWorldDirection(Math::AXIS_Z);

	// render local axes of the plane
	{
		Visualizer::renderVector(position, position + Vec3(direction_x) * 1.5f, vec4_red);
		Visualizer::renderVector(position, position + Vec3(direction_y) * 1.5f, vec4_green);
		Visualizer::renderVector(position, position + Vec3(direction_z) * 1.5f, vec4_blue);

		Visualizer::renderMessage3D(position + Vec3(direction_x) * 1.5f, vec3_zero, "X", vec4_black, 1);
		Visualizer::renderMessage3D(position + Vec3(direction_y) * 1.5f, vec3_zero, "Y", vec4_black, 1);
		Visualizer::renderMessage3D(position + Vec3(direction_z) * 1.5f, vec3_zero, "Z", vec4_black, 1);
	}

	// render global axes
	{
		Vec3 offset = { -2.0f, -2.0f, 0.2f };

		Visualizer::renderVector(offset, offset + Vec3_right * 2.0f, vec4_red);
		Visualizer::renderVector(offset, offset + Vec3_forward * 2.0f, vec4_green);
		Visualizer::renderVector(offset, offset + Vec3_up * 2.0f, vec4_blue);
	}


	// render orbits (circles)
	{
		Mat4 orbit_x_transform = Mat4_identity;
		Mat4 orbit_y_transform = Mat4_identity;
		Mat4 orbit_z_transform = Mat4_identity;
		vec3 radii = vec3_zero;

		// calculate orbit transforms
		{
			float x = euler_angles.x;
			float y = euler_angles.y;
			float z = euler_angles.z;

			float big_radius = 1.4f;
			float middle_radius = 1.3f;
			float small_radius = 1.2f;

			orbit_x_transform = Mat4 { rotate(quat{ euler_angles.x, 0.0f, 0.0f }) };
			orbit_y_transform = Mat4 { rotate(quat{ euler_angles.x, euler_angles.y, 0.0f }) };
			orbit_z_transform = Mat4 { rotate(quat{ euler_angles.x, euler_angles.y, euler_angles.z }) };
			radii = vec3 { big_radius, middle_radius, small_radius };

			switch (composition_sequence)
			{
				case XYZ:
					orbit_x_transform = Mat4 { composeRotationXYZ(vec3(x, 0.0f, 0.0f)) };
					orbit_y_transform = Mat4 { composeRotationXYZ(vec3(x, y, 0.0f)) };
					orbit_z_transform = Mat4 { composeRotationXYZ(vec3(x, y, z)) };
					radii = vec3(big_radius, middle_radius, small_radius);
					break;

				case XZY:
					orbit_x_transform = Mat4 { composeRotationXZY(vec3(x, 0.0f, 0.0f)) };
					orbit_z_transform = Mat4 { composeRotationXZY(vec3(x, 0.0f, z)) };
					orbit_y_transform = Mat4 { composeRotationXZY(vec3(x, y, z)) };
					radii = vec3(big_radius, small_radius, middle_radius);
					break;

				case YXZ:
					orbit_y_transform = Mat4 { composeRotationYXZ(vec3(0.0f, y, 0.0f)) };
					orbit_x_transform = Mat4 { composeRotationYXZ(vec3(x, y, 0.0f)) };
					orbit_z_transform = Mat4 { composeRotationYXZ(vec3(x, y, z)) };
					radii = vec3(middle_radius, big_radius, small_radius);
					break;

				case YZX:
					orbit_y_transform = Mat4 { composeRotationYZX(vec3(0.0f, y, 0.0f)) };
					orbit_z_transform = Mat4 { composeRotationYZX(vec3(0.0f, y, z)) };
					orbit_x_transform = Mat4 { composeRotationYZX(vec3(x, y, z)) };
					radii = vec3(small_radius, big_radius, middle_radius);
					break;

				case ZXY:
					orbit_z_transform = Mat4 { composeRotationZXY(vec3(0.0f, 0.0f, z)) };
					orbit_x_transform = Mat4 { composeRotationZXY(vec3(x, 0.0f, z)) };
					orbit_y_transform = Mat4 { composeRotationZXY(vec3(x, y, z)) };
					radii = vec3(middle_radius, small_radius, big_radius);
					break;

				case ZYX:
					orbit_z_transform = Mat4 { composeRotationZYX(vec3(0.0f, 0.0f, z)) };
					orbit_y_transform = Mat4 { composeRotationZYX(vec3(0.0f, y, z)) };
					orbit_x_transform = Mat4 { composeRotationZYX(vec3(x, y, z)) };
					radii = vec3(small_radius, middle_radius, big_radius);
					break;
			}

			orbit_x_transform = translate(position) * orbit_x_transform * Mat4 { rotate(quat{ 0, 90, 0 }) };
			orbit_y_transform = translate(position) * orbit_y_transform * Mat4 { rotate(quat{ 90, 0, 0 }) };
			orbit_z_transform = translate(position) * orbit_z_transform;
		}

		Visualizer::renderCircle(radii.x, orbit_x_transform, vec4_red);
		Visualizer::renderCircle(radii.y, orbit_y_transform, vec4_green);
		Visualizer::renderCircle(radii.z, orbit_z_transform, vec4_blue);

		// render orbit arrows
		{
			Vec3 arrow_start = position + orbit_x_transform.getAxisY() * radii.x;
			Vec3 arrow_finish = arrow_start + orbit_x_transform.getAxisY() * 0.1f;
			Visualizer::renderVector(arrow_start, arrow_finish, vec4_red, 1.0f);

			arrow_start = position + orbit_y_transform.getAxisY() * radii.y;
			arrow_finish = arrow_start + orbit_y_transform.getAxisY() * 0.1f;
			Visualizer::renderVector(arrow_start, arrow_finish, vec4_green, 1.0f);

			arrow_start = position + orbit_z_transform.getAxisY() * radii.z;
			arrow_finish = arrow_start + orbit_z_transform.getAxisY() * 0.1f;
			Visualizer::renderVector(arrow_start, arrow_finish, vec4_blue, 1.0f);
		}
	}
}

void EulerAnglesSample::shutdown()
{
	sample_description_window.shutdown();
	Visualizer::setEnabled(visualizer_enabled);
}


void EulerAnglesSample::update_rotation()
{
	mat4 rotation = mat4_identity;

	switch (composition_sequence)
	{
		case XYZ: rotation = composeRotationXYZ(euler_angles); break;
		case XZY: rotation = composeRotationXZY(euler_angles); break;
		case YXZ: rotation = composeRotationYXZ(euler_angles); break;
		case YZX: rotation = composeRotationYZX(euler_angles); break;
		case ZXY: rotation = composeRotationZXY(euler_angles); break;
		case ZYX: rotation = composeRotationZYX(euler_angles); break;
	}

	plane->setWorldRotation(quat{ rotation }, true);
}

void EulerAnglesSample::update_decomposition_angles()
{
	mat3 rotation = plane->getWorldRotation().getMat3();

	switch (decomposition_sequence)
	{
		case XYZ: decomposition_angles = decomposeRotationXYZ(rotation); break;
		case XZY: decomposition_angles = decomposeRotationXZY(rotation); break;
		case YXZ: decomposition_angles = decomposeRotationYXZ(rotation); break;
		case YZX: decomposition_angles = decomposeRotationYZX(rotation); break;
		case ZXY: decomposition_angles = decomposeRotationZXY(rotation); break;
		case ZYX: decomposition_angles = decomposeRotationZYX(rotation); break;
	}

	status = String::format("Decomposition angles:\nPitch (X):\t%.2f\nRoll (Y):\t%.2f\nYaw (Z):\t%.2f\n",
		decomposition_angles.x, decomposition_angles.y, decomposition_angles.z);
	sample_description_window.setStatus(status);
}

void EulerAnglesSample::init_gui()
{
	sample_description_window.createWindow();

	auto pitch_slider = sample_description_window.addIntParameter("Pitch (X)", "Pitch (X)", 0, -180, 180, [this](int value) {
		euler_angles.x = float(value);
		update_rotation();
		update_decomposition_angles();
	});

	auto roll_slider = sample_description_window.addIntParameter("Roll (Y)", "Roll (Y)", 0, -180, 180, [this](int value) {
		euler_angles.y = float(value);
		update_rotation();
		update_decomposition_angles();
	});

	auto yaw_slider = sample_description_window.addIntParameter("Yaw (Z)", "Yaw (Z)", 0, -180, 180, [this](int value) {
		euler_angles.z = float(value);
		update_rotation();
		update_decomposition_angles();
	});

	auto parameters = sample_description_window.getParameterGroupBox();

	WidgetComboBoxPtr composition_combo_box;
	WidgetComboBoxPtr decomposition_combo_box;

	{
		auto hbox = WidgetHBox::create();
		hbox->addChild(WidgetLabel::create("Composition sequence: "), Gui::ALIGN_LEFT);

		auto combo_box = WidgetComboBox::create();

		combo_box->addItem("XYZ");
		combo_box->addItem("XZY");
		combo_box->addItem("YXZ");
		combo_box->addItem("YZX");
		combo_box->addItem("ZXY");
		combo_box->addItem("ZYX");

		combo_box->getEventChanged().connect(*this, [this, combo_box] {
			composition_sequence = ROTATION_SEQUENCE(combo_box->getCurrentItem());
			update_rotation();
		});

		composition_combo_box = combo_box;

		hbox->addChild(combo_box);
		parameters->addChild(hbox, Gui::ALIGN_LEFT);
	};

	{
		auto hbox = WidgetHBox::create();
		hbox->addChild(WidgetLabel::create("Decomposition sequence: "), Gui::ALIGN_LEFT);

		auto combo_box = WidgetComboBox::create();

		combo_box->addItem("XYZ");
		combo_box->addItem("XZY");
		combo_box->addItem("YXZ");
		combo_box->addItem("YZX");
		combo_box->addItem("ZXY");
		combo_box->addItem("ZYX");

		combo_box->getEventChanged().connect(*this, [this, combo_box] {
			decomposition_sequence = ROTATION_SEQUENCE(combo_box->getCurrentItem());
			update_decomposition_angles();
		});

		decomposition_combo_box = combo_box;

		hbox->addChild(combo_box);
		parameters->addChild(hbox, Gui::ALIGN_LEFT);
	};

	auto reset_button = WidgetButton::create("Reset");
	reset_button->getEventClicked().connect(*this, [this, yaw_slider, roll_slider, pitch_slider, composition_combo_box, decomposition_combo_box] {
		euler_angles = vec3_zero;
		update_rotation();
		update_decomposition_angles();

		yaw_slider->setValue(0);
		pitch_slider->setValue(0);
		roll_slider->setValue(0);
		composition_combo_box->setCurrentItem(0);
		decomposition_combo_box->setCurrentItem(0);
	});

	parameters->addChild(reset_button, Gui::ALIGN_LEFT);
}

#include "RayIntersection.h"

REGISTER_COMPONENT(RayIntersection)

using namespace Unigine;
using namespace Math;

void RayIntersection::init()
{
	// check parts of laser
	if (!laser_ray || !laser_hit)
	{
		Log::error("laser_ray or laser_hit is missing");
		return;
	}

	// create an intersection object to obtain the necessary information
	// about the intersection result
	intersection = WorldIntersectionNormal::create();

	// save the source laser ray scale for changing length after intersection
	laser_ray_scale = laser_ray->getWorldScale();

	init_gui();
}

void RayIntersection::update()
{
	// check parts of laser
	if (!laser_ray || !laser_hit)
		return;

	// get points to detect intersection based on the direction of the laser ray
	Vec3 first_point = laser_ray->getWorldPosition();
	Vec3 second_point =
		first_point + Vec3(laser_ray->getWorldDirection(AXIS_Y)) * laser_distance;

	// try to get intersection object
	ObjectPtr hit_object = World::getIntersection(
		first_point, second_point, intersection_mask, intersection);
	if (hit_object)
	{
		// show hit_object name and color it
		// int wall_idx = log2(hit_object->getIntersectionMask(0));
		int wall_idx = hit_object->getParent()->getChildIndex(hit_object);
		current_hit->setText(String::format(
				"Current Hit Object: <b><font color=\"%s\">%s</font></b>",
				colors[wall_idx], walls[wall_idx]).get());

		// set current laser ray length
		float length =
			(intersection->getPoint() - laser_ray->getWorldPosition()).length();
		laser_ray_scale.y = length;
		laser_ray->setWorldScale(laser_ray_scale);

		// activate laserHit if it was hidden earlier
		if (!laser_hit->isEnabled())
			laser_hit->setEnabled(true);

		// update laser hit transform based on intersection information
		laser_hit->setWorldPosition(intersection->getPoint());
		laser_hit->setWorldDirection(intersection->getNormal(), vec3_up, AXIS_Y);

	} else
	{
		// hide hit_object name
		auto parameters = window.getParameterGroupBox();
		static_ptr_cast<WidgetLabel>(parameters->getChild(3))
			->setText(
				"Current Hit Object: <font color=\"#ffffff\">None</font>");

		// set default ray length
		laser_ray_scale.y = laser_distance;
		laser_ray->setWorldScale(laser_ray_scale);

		// hide hit point
		laser_hit->setEnabled(false);
	}
}

void RayIntersection::shutdown()
{
	mask_text.deleteLater();
	window.shutdown();
}

void RayIntersection::init_gui()
{
	window.createWindow();

	auto parameters = window.getParameterGroupBox();

	int walls_size = walls.size();

	auto grid = WidgetGridBox::create(4, 10, 10);
	auto hbox = WidgetHBox::create();
	auto line = WidgetEditLine::create();
	auto label = WidgetLabel::create("Laser Mask: ");
	current_hit = WidgetLabel::create("Current Hit Object: None");
	current_hit->setFontRich(true);
	line->setWidth(100);
	line->setCapacity(8);
	line->setText("00000000");
	line->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(label);
	hbox->addChild(line);
	hbox->setPadding(0, 0, 5, 5);

	parameters->addChild(grid);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	parameters->addChild(current_hit, Gui::ALIGN_LEFT);

	prev_text = line->getText();
	line->getEventKeyPressed().connect(
		*this, [](const WidgetPtr &check, int key) {
			if (key == Input::KEY_ENTER)
			{
				check->removeFocus();
			}
		});
		
	line->getEventChanged().connect(*this, [this](const WidgetPtr &edit_line) {
		auto temp = static_ptr_cast<WidgetEditLine>(edit_line);
		String text = temp->getText();
		if (prev_text == text)
			return;

		String new_text;
		for (int i = 0, some = text.size(); i < some; ++i)
		{
			if (text[i] == '0' || text[i] == '1')
			{
				new_text.append(text[i]);
			} else
			{
				temp->setCursor(i);
			}
		}
		prev_text = temp->getText();
		temp->setText(new_text);
	});
	line->getEventFocusOut().connect(
		*this, [this, grid](const WidgetPtr &edit_line) {
			auto temp = static_ptr_cast<WidgetEditLine>(edit_line);
			String text = temp->getText();
			if (text.size() < 8)
			{
				String new_text;
				for (int i = text.size(); i < 8; ++i)
				{
					new_text.append('0');
				}
				new_text = new_text + text;
				temp->setText(new_text);
				prev_text = new_text;
			}

			for (int i = 0; i < 8; ++i)
			{
				static_ptr_cast<WidgetCheckBox>(grid->getChild(7 - i))
					->setChecked(temp->getText()[i] - '0' > 0);
			}
		});

	for (int i = 0; i < walls_size; ++i)
	{
		auto checkbox = WidgetCheckBox::create(walls[i]);
		int wall_intersection_mask = 1 << i;

		int ind = walls_size - 1 - i;
		checkbox->getEventClicked().connect(
			*this, [this, wall_intersection_mask, ind, line](
					   const WidgetPtr &check) {
				String new_text = line->getText();
				if (static_ptr_cast<WidgetCheckBox>(check)->isChecked())
				{
					new_text[ind] = '1';
					intersection_mask =
						intersection_mask | wall_intersection_mask;
				} else
				{
					new_text[ind] = '0';
					intersection_mask =
						intersection_mask & ~wall_intersection_mask;
				}
				line->setText(new_text);
			});

		checkbox->setChecked((intersection_mask & wall_intersection_mask) > 0);

		grid->addChild(checkbox, Gui::ALIGN_LEFT);
	}
}

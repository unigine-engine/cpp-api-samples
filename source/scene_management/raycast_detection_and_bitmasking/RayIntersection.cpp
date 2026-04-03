// Ray intersection demonstration using World::getIntersection. A laser ray is cast
// each frame, and intersection mask checkboxes control which objects can be hit.
// Hit point visualizer aligns to intersection normal. Laser length adjusts to distance.

#include "RayIntersection.h"

REGISTER_COMPONENT(RayIntersection)

using namespace Unigine;
using namespace Math;

// Intersection object is created and laser scale is cached for dynamic adjustment.
void RayIntersection::init()
{
	// Check parts of laser
	if (!laser_ray || !laser_hit)
	{
		Log::error("laser_ray or laser_hit is missing");
		return;
	}

	// Create an intersection object to obtain the necessary information
	// about the intersection result
	intersection = WorldIntersectionNormal::create();

	// Save the source laser ray scale for changing length after intersection
	laser_ray_scale = laser_ray->getWorldScale();

	init_gui();
}

// Ray is cast along laser direction; hit point and length are updated each frame.
void RayIntersection::update()
{
	// Check parts of laser
	if (!laser_ray || !laser_hit)
		return;

	// Get points to detect intersection based on the direction of the laser ray
	Vec3 first_point = laser_ray->getWorldPosition();
	Vec3 second_point = first_point + Vec3(laser_ray->getWorldDirection(AXIS_Y)) * laser_distance;

	// Perform ray intersection test with current mask filter
	ObjectPtr hit_object = World::getIntersection(
		first_point, second_point, intersection_mask, intersection);
	if (hit_object && hit_object->getParent())
	{
		// Determine wall index from sibling order and update label with color
		int wall_idx = hit_object->getParent()->getChildIndex(hit_object);
		current_hit->setText(String::format(
			"Current Hit Object: <b><font color=\"%s\">%s</font></b>",
			colors[wall_idx], walls[wall_idx])
				.get());

		// Adjust laser beam length to match intersection distance
		float length = (intersection->getPoint() - laser_ray->getWorldPosition()).length();
		laser_ray_scale.y = length;
		laser_ray->setWorldScale(laser_ray_scale);

		// Show hit indicator if previously hidden
		if (!laser_hit->isEnabled())
			laser_hit->setEnabled(true);

		// Position hit indicator at intersection point aligned to surface normal
		laser_hit->setWorldPosition(intersection->getPoint());
		laser_hit->setWorldDirection(intersection->getNormal(), vec3_up, AXIS_Y);
	}
	else
	{
		// Clear hit label when no intersection
		auto parameters = window.getParameterGroupBox();
		static_ptr_cast<WidgetLabel>(parameters->getChild(3))
			->setText(
				"Current Hit Object: <font color=\"#ffffff\">None</font>");

		// Extend laser to maximum distance when no hit
		laser_ray_scale.y = laser_distance;
		laser_ray->setWorldScale(laser_ray_scale);

		// Hide hit indicator
		laser_hit->setEnabled(false);
	}
}

// UI resources are cleaned up on component destruction.
void RayIntersection::shutdown()
{
	mask_text.deleteLater();
	window.shutdown();
}

// UI is built with checkboxes for each wall color and binary mask input field.
void RayIntersection::init_gui()
{
	window.createWindow();

	auto parameters = window.getParameterGroupBox();

	int walls_size = walls.size();

	// Create grid for wall checkboxes and binary mask input
	auto grid = WidgetGridBox::create(4, 10, 10);
	auto hbox = WidgetHBox::create();
	auto line = WidgetEditLine::create();
	auto label = WidgetLabel::create("Laser Mask: ");
	current_hit = WidgetLabel::create("Current Hit Object: None");
	current_hit->setFontRich(true);
	// Configure binary mask text input (8 bits for 8 walls)
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

	// Remove focus on Enter key to apply mask changes
	prev_text = line->getText();
	line->getEventKeyPressed().connect(
		*this, [](const WidgetPtr &check, int key) {
			if (key == Input::KEY_ENTER)
			{
				check->removeFocus();
			}
		});

	// Filter input to allow only binary digits (0 and 1)
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
			}
			else
			{
				temp->setCursor(i);
			}
		}
		prev_text = temp->getText();
		temp->setText(new_text);
	});
	// Sync checkboxes with mask text when focus leaves input field
	line->getEventFocusOut().connect(
		*this, [this, grid](const WidgetPtr &edit_line) {
			auto temp = static_ptr_cast<WidgetEditLine>(edit_line);
			String text = temp->getText();
			// Pad to 8 digits with leading zeros
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

			// Update checkbox states from binary string
			for (int i = 0; i < 8; ++i)
			{
				static_ptr_cast<WidgetCheckBox>(grid->getChild(7 - i))
					->setChecked(temp->getText()[i] - '0' > 0);
			}
		});

	// Create checkbox for each wall color
	for (int i = 0; i < walls_size; ++i)
	{
		auto checkbox = WidgetCheckBox::create(walls[i]);
		int wall_intersection_mask = 1 << i;

		// Toggle bit in mask and sync text field when checkbox changes
		int ind = walls_size - 1 - i;
		checkbox->getEventClicked().connect(
			*this, [this, wall_intersection_mask, ind, line](const WidgetPtr &check) {
				String new_text = line->getText();
				if (static_ptr_cast<WidgetCheckBox>(check)->isChecked())
				{
					new_text[ind] = '1';
					intersection_mask = intersection_mask | wall_intersection_mask;
				}
				else
				{
					new_text[ind] = '0';
					intersection_mask = intersection_mask & ~wall_intersection_mask;
				}
				line->setText(new_text);
			});

		// Initialize checkbox state from current mask
		checkbox->setChecked((intersection_mask & wall_intersection_mask) > 0);

		grid->addChild(checkbox, Gui::ALIGN_LEFT);
	}
}

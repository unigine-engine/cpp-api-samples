#pragma once

#include <UnigineComponentSystem.h>

class Manipulators : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Manipulators, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component enables you to manipulate objects in the world using manipulators (WidgetManipulator)."
		"There should be only one property associated with this component used in the world."
		"This property should be assigned to a dedicated node used to control the logic of this component.");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Mask, intersection_mask, 1, "Intersection mask", "Mask to define which objects can be selected.");
	PROP_PARAM(Toggle, transform_parent, 1);

	void setXAxisRotation(bool value);
	bool isXAxisRotation() const { return is_x_axis_rotation; }
	void setYAxisRotation(bool value);
	bool isYAxisRotation() const { return is_y_axis_rotation; }
	void setZAxisRotation(bool value);
	bool isZAxisRotation() const { return is_z_axis_rotation; }

	void setXAxisTranslation(bool value);
	bool isXAxisTranslation() { return is_x_axis_translation; }
	void setYAxisTranslation(bool value);
	bool isYAxisTranslation() { return is_y_axis_translation; }
	void setZAxisTranslation(bool value);
	bool isZAxisTranslation() { return is_z_axis_translation; }

	void setXAxisScale(bool value);
	bool isXAxisScale() { return is_x_axis_scale; }
	void setYAxisScale(bool value);
	bool isYAxisScale() { return is_y_axis_scale; }
	void setZAxisScale(bool value);
	bool isZAxisScale() { return is_z_axis_scale; }

	void setLocalBasis(bool value);
	bool isLocalBasis() { return is_local_basis; }

	bool isActive() const { return bool(obj); }

protected:
	void init();
	void update();
	void shutdown();

private:
	void apply_transform();
	Unigine::ObjectPtr get_node_under_cursor();
	void switch_manipulator(const Unigine::WidgetManipulatorPtr &currentManipulator);
	void unselect();
	void set_manipulators_basis();

	Unigine::ObjectPtr obj;
	Unigine::GuiPtr gui;
	Unigine::WidgetManipulatorPtr current_object_manipulator;

	Unigine::WidgetManipulatorTranslatorPtr object_translator;
	Unigine::WidgetManipulatorRotatorPtr object_rotator;
	Unigine::WidgetManipulatorScalerPtr object_scaler;

	bool is_x_axis_rotation;
	bool is_y_axis_rotation;
	bool is_z_axis_rotation;
	bool is_x_axis_translation;
	bool is_y_axis_translation;
	bool is_z_axis_translation;
	bool is_x_axis_scale;
	bool is_y_axis_scale;
	bool is_z_axis_scale;

	bool is_local_basis;
};

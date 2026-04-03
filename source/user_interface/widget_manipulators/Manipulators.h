// Object manipulation system using WidgetManipulator widgets. Objects are selected
// via mouse click ray intersection and transformed using translator, rotator, and
// scaler widgets. Hotkeys (W/E/R) switch between manipulator modes.

#pragma once

#include <UnigineComponentSystem.h>

// Provides interactive object manipulation with translation, rotation, and scale widgets.
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

	// Intersection mask filters which objects can be selected via raycasting
	PROP_PARAM(Mask, intersection_mask, 1, "Intersection mask", "Mask to define which objects can be selected.");
	// When enabled, parent node is transformed instead of selected child
	PROP_PARAM(Toggle, transform_parent, 1);

	// Rotation axis control - enables/disables X axis on rotator widget
	void setXAxisRotation(bool value);
	bool isXAxisRotation() const
	{
		return is_x_axis_rotation;
	}
	// Rotation axis control - enables/disables Y axis on rotator widget
	void setYAxisRotation(bool value);
	bool isYAxisRotation() const
	{
		return is_y_axis_rotation;
	}
	// Rotation axis control - enables/disables Z axis on rotator widget
	void setZAxisRotation(bool value);
	bool isZAxisRotation() const
	{
		return is_z_axis_rotation;
	}
	// Sets all rotation axes to same enabled state
	void setAxesRotation(bool value);

	// Translation axis control - enables/disables X axis on translator widget
	void setXAxisTranslation(bool value);
	bool isXAxisTranslation()
	{
		return is_x_axis_translation;
	}
	// Translation axis control - enables/disables Y axis on translator widget
	void setYAxisTranslation(bool value);
	bool isYAxisTranslation()
	{
		return is_y_axis_translation;
	}
	// Translation axis control - enables/disables Z axis on translator widget
	void setZAxisTranslation(bool value);
	bool isZAxisTranslation()
	{
		return is_z_axis_translation;
	}
	// Sets all translation axes to same enabled state
	void setAxesTranslation(bool value);

	// Scale axis control - enables/disables X axis on scaler widget
	void setXAxisScale(bool value);
	bool isXAxisScale()
	{
		return is_x_axis_scale;
	}
	// Scale axis control - enables/disables Y axis on scaler widget
	void setYAxisScale(bool value);
	bool isYAxisScale()
	{
		return is_y_axis_scale;
	}
	// Scale axis control - enables/disables Z axis on scaler widget
	void setZAxisScale(bool value);
	bool isZAxisScale()
	{
		return is_z_axis_scale;
	}
	// Sets all scale axes to same enabled state
	void setAxesScale(bool value);

	// Switches between local (object) and world coordinate basis
	void setLocalBasis(bool value);
	bool isLocalBasis()
	{
		return is_local_basis;
	}

	// Returns true when an object is currently selected for manipulation
	bool isActive() const
	{
		return bool(obj);
	}
	// Syncs manipulator widget transform with selected object
	void updateManipulatorTransform();
	// Event fired when object transform changes via manipulator
	Unigine::Event<Unigine::ObjectPtr>& getEventTransformChanged()
	{
		return transform_changed;
	}

protected:
	void init();
	void update();
	void shutdown();

private:
	// Applies manipulator widget transform to selected object
	void apply_transform();
	// Performs raycast from camera through mouse position to find object
	Unigine::ObjectPtr get_node_under_cursor();
	// Activates specified manipulator and hides others
	void switch_manipulator(const Unigine::WidgetManipulatorPtr& currentManipulator);
	// Clears selection and hides all manipulators
	void unselect();
	// Updates manipulator basis to match current local/world mode
	void set_manipulators_basis();

	// Currently selected object being manipulated
	Unigine::ObjectPtr obj;
	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Currently active manipulator widget
	Unigine::WidgetManipulatorPtr current_object_manipulator;

	// Position manipulation widget (hotkey W)
	Unigine::WidgetManipulatorTranslatorPtr object_translator;
	// Rotation manipulation widget (hotkey E)
	Unigine::WidgetManipulatorRotatorPtr object_rotator;
	// Scale manipulation widget (hotkey R)
	Unigine::WidgetManipulatorScalerPtr object_scaler;

	// Per-axis enabled flags for rotation manipulator
	bool is_x_axis_rotation;
	bool is_y_axis_rotation;
	bool is_z_axis_rotation;
	// Per-axis enabled flags for translation manipulator
	bool is_x_axis_translation;
	bool is_y_axis_translation;
	bool is_z_axis_translation;
	// Per-axis enabled flags for scale manipulator
	bool is_x_axis_scale;
	bool is_y_axis_scale;
	bool is_z_axis_scale;

	// Basis mode flag (true = local object basis, false = world basis)
	bool is_local_basis;
	// Event invoker for transform change notifications
	Unigine::EventInvoker<Unigine::ObjectPtr> transform_changed;
};

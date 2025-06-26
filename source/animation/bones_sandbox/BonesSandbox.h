#pragma once
#include <UnigineWidgets.h>
#include <UnigineObjects.h>

class WidgetVec3 final
{
public:
	WidgetVec3() = default;
	~WidgetVec3() = default;

	void init();
	void setValue(const Unigine::Math::Vec3 &value);
	Unigine::Math::Vec3 getValue() const;

	Unigine::WidgetPtr getWidget() const { return main_hbox; }

	Unigine::EventInvoker<> &getEventChanged() { return event_changed; }

private:
	Unigine::WidgetHBoxPtr main_hbox;

	Unigine::WidgetSpinBoxDoublePtr x_spin_box;
	Unigine::WidgetSpinBoxDoublePtr y_spin_box;
	Unigine::WidgetSpinBoxDoublePtr z_spin_box;

	Unigine::WidgetEditLinePtr x_line;
	Unigine::WidgetEditLinePtr y_line;
	Unigine::WidgetEditLinePtr z_line;

	Unigine::EventInvoker<> event_changed;
	Unigine::EventConnections widget_connections;
};

class WidgetVec2 final
{
public:
	WidgetVec2() = default;
	~WidgetVec2() = default;

	void init();
	void setValue(const Unigine::Math::Vec2 &value);
	Unigine::Math::Vec2 getValue() const;

	Unigine::WidgetPtr getWidget() const { return main_hbox; }

	Unigine::EventInvoker<> &getEventChanged() { return event_changed; }

private:
	Unigine::WidgetHBoxPtr main_hbox;

	Unigine::WidgetSpinBoxDoublePtr x_spin_box;
	Unigine::WidgetSpinBoxDoublePtr y_spin_box;

	Unigine::WidgetEditLinePtr x_line;
	Unigine::WidgetEditLinePtr y_line;

	Unigine::EventInvoker<> event_changed;
	Unigine::EventConnections widget_connections;
};

class BoneSelection final
{
public:
	BoneSelection() = default;
	~BoneSelection() = default;

	void init(const Unigine::ObjectMeshSkinnedPtr &obj);
	int getSelectedBone() const;

	Unigine::WidgetPtr getWidget() const { return main_group; }

private:
	void init_widgets();

private:
	Unigine::ObjectMeshSkinnedPtr skinned;

	Unigine::WidgetGroupBoxPtr main_group;
	Unigine::WidgetScrollBoxPtr scroll_box;
	Unigine::WidgetTreeBoxPtr tree_box;
	Unigine::EventConnection widget_connection;

	int last_visualized_bone{-1};
};

class LookAtEditor final
{
public:
	LookAtEditor() = default;
	~LookAtEditor() = default;

	void init(Unigine::ObjectMeshSkinnedPtr &obj, const BoneSelection *selection);

	int getSelectedChainID() const;

	Unigine::WidgetPtr getWidget() const { return main_group; }

private:
	void init_widgets();

	void update_chains_list();
	void update_bones_list();

	void chain_changed();
	void bone_changed();

private:
	Unigine::ObjectMeshSkinnedPtr skinned;
	const BoneSelection *bone_selection{nullptr};

	Unigine::WidgetGroupBoxPtr main_group;
	Unigine::WidgetGridBoxPtr grid_box;

	Unigine::WidgetGroupBoxPtr chains_group;
	Unigine::WidgetButtonPtr add_chain_button;
	Unigine::WidgetButtonPtr remove_chain_button;
	Unigine::WidgetListBoxPtr chains_list_box;

	Unigine::WidgetGroupBoxPtr chain_settings_group;
	Unigine::WidgetCheckBoxPtr chain_enabled_check_box;
	Unigine::WidgetComboBoxPtr chain_constraint_combo_box;
	Unigine::WidgetSliderPtr chain_weight_slider;
	WidgetVec3 target_widget;
	WidgetVec3 pole_widget;

	Unigine::WidgetGroupBoxPtr bones_group;
	Unigine::WidgetButtonPtr add_bone_button;
	Unigine::WidgetButtonPtr remove_bone_button;
	Unigine::WidgetListBoxPtr bones_list_box;

	Unigine::WidgetGroupBoxPtr bone_settings_group;
	Unigine::WidgetSliderPtr bone_weight_slider;
	WidgetVec3 bone_up_widget;
	WidgetVec3 bone_axis_widget;

	Unigine::EventConnections widget_connections;

	int last_visualized_bone{-1};
};

class IKEditor final
{
public:
	IKEditor() = default;
	~IKEditor() = default;

	void init(Unigine::ObjectMeshSkinnedPtr &obj, const BoneSelection *selection);

	int getSelectedChainID() const;

	Unigine::WidgetPtr getWidget() const { return main_group; }

private:
	void init_widgets();

	void update_chains_list();
	void update_bones_list();

	void chain_changed();
	void bone_changed();

private:
	Unigine::ObjectMeshSkinnedPtr skinned;
	const BoneSelection *bone_selection{nullptr};

	Unigine::WidgetGroupBoxPtr main_group;
	Unigine::WidgetGridBoxPtr grid_box;

	Unigine::WidgetGroupBoxPtr chains_group;
	Unigine::WidgetButtonPtr add_chain_button;
	Unigine::WidgetButtonPtr remove_chain_button;
	Unigine::WidgetListBoxPtr chains_list_box;

	Unigine::WidgetGroupBoxPtr chain_settings_group;
	Unigine::WidgetCheckBoxPtr chain_enabled_check_box;
	Unigine::WidgetComboBoxPtr chain_constraint_combo_box;
	Unigine::WidgetSliderPtr chain_weight_slider;
	WidgetVec3 target_widget;
	WidgetVec3 pole_widget;
	Unigine::WidgetCheckBoxPtr chain_effector_rotation_check_box;
	WidgetVec3 rotation_widget;
	Unigine::WidgetSliderPtr chain_iterations_slider;
	Unigine::WidgetSpinBoxDoublePtr chain_tolerance_spin_box;
	Unigine::WidgetEditLinePtr chain_tolerance_line;

	Unigine::WidgetGroupBoxPtr bones_group;
	Unigine::WidgetButtonPtr add_bone_button;
	Unigine::WidgetButtonPtr remove_bone_button;
	Unigine::WidgetListBoxPtr bones_list_box;

	Unigine::EventConnections widget_connections;

	int last_visualized_bone{-1};
};

class ConstraintEditor final
{
public:
	ConstraintEditor() = default;
	~ConstraintEditor() = default;

	void init(Unigine::ObjectMeshSkinnedPtr &obj, const BoneSelection *selection);

	Unigine::WidgetPtr getWidget() const { return main_group; }

private:
	void init_widgets();

	void update_constraints_list();
	void constraint_changed();

private:
	Unigine::ObjectMeshSkinnedPtr skinned;
	const BoneSelection *bone_selection{nullptr};

	Unigine::WidgetGroupBoxPtr main_group;
	Unigine::WidgetGridBoxPtr grid_box;

	Unigine::WidgetGroupBoxPtr constraints_group;
	Unigine::WidgetButtonPtr add_constraint_button;
	Unigine::WidgetButtonPtr remove_constraint_button;
	Unigine::WidgetListBoxPtr constraints_list_box;

	Unigine::WidgetGroupBoxPtr constraint_settings_group;
	Unigine::WidgetCheckBoxPtr enabled_check_box;
	WidgetVec3 yaw_axis_widget;
	WidgetVec3 pitch_axis_widget;
	WidgetVec3 roll_axis_widget;
	WidgetVec2 yaw_angles_widget;
	WidgetVec2 pitch_angles_widget;
	WidgetVec2 roll_angles_widget;

	Unigine::EventConnections widget_connections;

	int last_visualized_bone{-1};
};

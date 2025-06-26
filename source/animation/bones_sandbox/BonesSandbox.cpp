#include "BonesSandbox.h"

using namespace Unigine;
using namespace Math;

template<typename T>
Ptr<T> createWidgetOutline(const char *text)
{
	Ptr<T> widget = T::create(text);
	widget->setFontOutline(1);
	return widget;
}

template<typename T>
Ptr<T> createWidgetOutline()
{
	Ptr<T> widget = T::create();
	widget->setFontOutline(1);
	return widget;
}

WidgetVBoxPtr createNamedField(const char *name, const WidgetPtr &widget, int flags = Gui::ALIGN_LEFT)
{
	WidgetLabelPtr label = WidgetLabel::create(name);
	label->setFontOutline(1);

	WidgetVBoxPtr vbox = WidgetVBox::create();

	WidgetHBoxPtr hbox = WidgetHBox::create();
	vbox->addChild(hbox, Gui::ALIGN_EXPAND);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	hbox->addChild(widget, flags);

	return vbox;
}

WidgetVBoxPtr createNamedField(const char *name, const WidgetPtr &widget_0, const WidgetPtr &widget_1, int flags = Gui::ALIGN_LEFT)
{
	WidgetLabelPtr label = WidgetLabel::create(name);
	label->setFontOutline(1);

	WidgetVBoxPtr vbox = WidgetVBox::create();

	WidgetHBoxPtr hbox = WidgetHBox::create();
	vbox->addChild(hbox, Gui::ALIGN_EXPAND);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	hbox->addChild(widget_0, flags);
	hbox->addChild(widget_1, flags);

	return vbox;
}

// Widget Vec3

void WidgetVec3::init()
{
	main_hbox.deleteLater();
	main_hbox = WidgetHBox::create(5);

	x_line = createWidgetOutline<WidgetEditLine>();
	x_line->setValidator(Gui::VALIDATOR_FLOAT);
	x_spin_box = WidgetSpinBoxDouble::create(-10.0, 10.0, 0.0, 0.1);
	x_spin_box->addAttach(x_line);
	main_hbox->addChild(createNamedField("X: ", x_line, x_spin_box));

	y_line = createWidgetOutline<WidgetEditLine>();
	y_line->setValidator(Gui::VALIDATOR_FLOAT);
	y_spin_box = WidgetSpinBoxDouble::create(-10.0, 10.0, 0.0, 0.1);
	y_spin_box->addAttach(y_line);
	main_hbox->addChild(createNamedField("Y: ", y_line, y_spin_box));

	z_line = createWidgetOutline<WidgetEditLine>();
	z_line->setValidator(Gui::VALIDATOR_FLOAT);
	z_spin_box = WidgetSpinBoxDouble::create(-10.0, 10.0, 0.0, 0.1);
	z_spin_box->addAttach(z_line);
	main_hbox->addChild(createNamedField("Z: ", z_line, z_spin_box));

	x_spin_box->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	y_spin_box->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	z_spin_box->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	x_line->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	y_line->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	z_line->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
}

void WidgetVec3::setValue(const Vec3 &value)
{
	if (x_line == nullptr || y_line == nullptr || z_line == nullptr)
		return;

	x_line->setText(String::ftoa(toFloat(value.x)));
	y_line->setText(String::ftoa(toFloat(value.y)));
	z_line->setText(String::ftoa(toFloat(value.z)));
}

Vec3 WidgetVec3::getValue() const
{
	if (x_line == nullptr || y_line == nullptr || z_line == nullptr)
		return Vec3_zero;

	return Vec3(String::atof(x_line->getText()), String::atof(y_line->getText()), String::atof(z_line->getText()));
}

// Widget Vec2

void WidgetVec2::init()
{
	main_hbox.deleteLater();
	main_hbox = WidgetHBox::create(5);

	x_line = createWidgetOutline<WidgetEditLine>();
	x_line->setValidator(Gui::VALIDATOR_FLOAT);
	x_spin_box = WidgetSpinBoxDouble::create(-10.0, 10.0, 0.0, 0.1);
	x_spin_box->addAttach(x_line);
	main_hbox->addChild(createNamedField("X: ", x_line, x_spin_box));

	y_line = createWidgetOutline<WidgetEditLine>();
	y_line->setValidator(Gui::VALIDATOR_FLOAT);
	y_spin_box = WidgetSpinBoxDouble::create(-10.0, 10.0, 0.0, 0.1);
	y_spin_box->addAttach(y_line);
	main_hbox->addChild(createNamedField("Y: ", y_line, y_spin_box));

	x_spin_box->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	y_spin_box->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	x_line->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
	y_line->getEventChanged().connect(widget_connections, [this]() { event_changed.run(); });
}

void WidgetVec2::setValue(const Vec2 &value)
{
	if (x_line == nullptr || y_line == nullptr)
		return;

	x_line->setText(String::ftoa(toFloat(value.x)));
	y_line->setText(String::ftoa(toFloat(value.y)));
}

Vec2 WidgetVec2::getValue() const
{
	if (x_line == nullptr || y_line == nullptr)
		return Vec2_zero;

	return Vec2(String::atof(x_line->getText()), String::atof(y_line->getText()));
}

// Bone Selection

void BoneSelection::init(const ObjectMeshSkinnedPtr &obj)
{
	skinned = obj;

	if (main_group == nullptr)
		init_widgets();

	tree_box->clear();

	if (skinned.isValid())
	{
		int num_bones = skinned->getNumBones();
		for (int i = 0; i < num_bones; i++)
		{
			const char *name = skinned->getBoneName(i);
			tree_box->addItem(String::format("%s (%d)", name, i));
		}

		for (int i = 0; i < num_bones; i++)
		{
			int parent = skinned->getBoneParent(i);
			if (parent != -1)
				tree_box->setItemParent(i, parent);
		}
	}
}

int BoneSelection::getSelectedBone() const
{
	if (tree_box == nullptr)
		return -1;

	return tree_box->getCurrentItem();
}

void BoneSelection::init_widgets()
{
	main_group = createWidgetOutline<WidgetGroupBox>("<b>Bone Selection</b>");
	main_group->setFontRich(1);
	main_group->setFontSize(14);

	scroll_box = WidgetScrollBox::create();
	scroll_box->setWidth(350);
	scroll_box->setHeight(200);
	main_group->addChild(scroll_box, Gui::ALIGN_LEFT);

	tree_box = createWidgetOutline<WidgetTreeBox>();
	tree_box->setMultiSelection(false);
	scroll_box->addChild(tree_box, Gui::ALIGN_EXPAND);

	tree_box->getEventChanged().connect(widget_connection, [this]()
	{
		if (last_visualized_bone != -1)
			skinned->removeVisualizeBone(last_visualized_bone);

		last_visualized_bone = tree_box->getCurrentItem();
		if (last_visualized_bone != -1)
			skinned->addVisualizeBone(last_visualized_bone);
	});
}

// Look At Editor

void LookAtEditor::init(ObjectMeshSkinnedPtr &obj, const BoneSelection *selection)
{
	if (main_group == nullptr)
		init_widgets();

	skinned = obj;
	bone_selection = selection;

	update_chains_list();
}

int LookAtEditor::getSelectedChainID() const
{
	if (chains_list_box == nullptr)
		return 1;

	int index = chains_list_box->getCurrentItem();
	if (index == -1)
		return -1;

	return skinned->getLookAtChainID(index);
}

void LookAtEditor::init_widgets()
{
	// groups
	main_group = createWidgetOutline<WidgetGroupBox>("<b>Look At Editor</b>");
	main_group->setFontRich(1);
	main_group->setFontSize(14);

	grid_box = WidgetGridBox::create();
	main_group->addChild(grid_box, Gui::ALIGN_EXPAND);

	// chains
	chains_group = createWidgetOutline<WidgetGroupBox>("Chains");
	grid_box->addChild(chains_group, Gui::ALIGN_EXPAND);

	add_chain_button = createWidgetOutline<WidgetButton>("Add Look At Chain");
	chains_group->addChild(add_chain_button, Gui::ALIGN_LEFT);
	add_chain_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (skinned.isValid() == false)
			return;

		skinned->addLookAtChain();
		update_chains_list();
	});

	remove_chain_button = createWidgetOutline<WidgetButton>("Remove Look At Chain");
	chains_group->addChild(remove_chain_button, Gui::ALIGN_LEFT);
	remove_chain_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (skinned.isValid() == false)
			return;

		int id = getSelectedChainID();
		if (id != -1)
		{
			skinned->removeLookAtChain(id);
			update_chains_list();
		}
	});

	chains_list_box = createWidgetOutline<WidgetListBox>();
	chains_list_box->setMultiSelection(false);
	chains_group->addChild(chains_list_box, Gui::ALIGN_LEFT);
	chains_list_box->getEventChanged().connect(widget_connections, [this]()
	{
		chain_changed();
	});

	// chain settings
	chain_settings_group = createWidgetOutline<WidgetGroupBox>("Chain Settings");
	grid_box->addChild(chain_settings_group, Gui::ALIGN_EXPAND);

	chain_enabled_check_box = createWidgetOutline<WidgetCheckBox>("Enabled");
	chain_enabled_check_box->setChecked(true);
	chain_settings_group->addChild(chain_enabled_check_box, Gui::ALIGN_LEFT);
	chain_enabled_check_box->getEventClicked().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setLookAtChainEnabled(chain_enabled_check_box->isChecked(), id);
	});

	chain_constraint_combo_box = createWidgetOutline<WidgetComboBox>();
	chain_constraint_combo_box->addItem("NONE");
	chain_constraint_combo_box->addItem("POLE_VECTOR");
	chain_constraint_combo_box->addItem("BONE_ROTATIONS");
	chain_settings_group->addChild(createNamedField("Constraint: ", chain_constraint_combo_box), Gui::ALIGN_LEFT);
	chain_constraint_combo_box->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setLookAtChainConstraint(static_cast<ObjectMeshSkinned::CHAIN_CONSTRAINT>(chain_constraint_combo_box->getCurrentItem()), id);
	});

	chain_weight_slider = WidgetSlider::create(0, 1000, 1000);
	chain_settings_group->addChild(createNamedField("Weight: ", chain_weight_slider, Gui::ALIGN_EXPAND));
	chain_weight_slider->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		float weight = chain_weight_slider->getValue() / 1000.0f;
		skinned->setLookAtChainWeight(weight, id);
	});

	target_widget.init();
	chain_settings_group->addChild(createNamedField("Target: ", target_widget.getWidget(), Gui::ALIGN_RIGHT));
	target_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setLookAtChainTargetWorldPosition(target_widget.getValue(), id);
	});

	pole_widget.init();
	chain_settings_group->addChild(createNamedField("Pole: ", pole_widget.getWidget(), Gui::ALIGN_RIGHT));
	pole_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setLookAtChainPoleWorldPosition(pole_widget.getValue(), id);
	});

	// bones
	bones_group = createWidgetOutline<WidgetGroupBox>("Bones In Chain");
	grid_box->addChild(bones_group, Gui::ALIGN_EXPAND);

	add_bone_button = createWidgetOutline<WidgetButton>("Add Bone From Selection");
	bones_group->addChild(add_bone_button, Gui::ALIGN_LEFT);
	add_bone_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (bone_selection == nullptr)
			return;

		int id = getSelectedChainID();
		if (id == -1)
			return;

		int bone = bone_selection->getSelectedBone();
		if (bone == -1)
			return;

		skinned->addLookAtChainBone(bone, id);
		update_bones_list();
	});

	remove_bone_button = createWidgetOutline<WidgetButton>("Remove Bone");
	bones_group->addChild(remove_bone_button, Gui::ALIGN_LEFT);
	remove_bone_button->getEventClicked().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		int index = bones_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->removeLookAtChainBone(index, id);
		update_bones_list();
	});

	bones_list_box = createWidgetOutline<WidgetListBox>();
	bones_list_box->setMultiSelection(false);
	bones_group->addChild(bones_list_box, Gui::ALIGN_LEFT);
	bones_list_box->getEventChanged().connect(widget_connections, [this]()
	{
		bone_changed();
	});

	// bone settings
	bone_settings_group = createWidgetOutline<WidgetGroupBox>("Bone Settings");
	grid_box->addChild(bone_settings_group, Gui::ALIGN_EXPAND);

	bone_weight_slider = WidgetSlider::create(0, 1000, 1000);
	bone_settings_group->addChild(createNamedField("Weight: ", bone_weight_slider, Gui::ALIGN_EXPAND));
	bone_weight_slider->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		int index = bones_list_box->getCurrentItem();
		if (index == -1)
			return;

		float weight = bone_weight_slider->getValue() / 1000.0f;
		skinned->setLookAtChainBoneWeight(weight, index, id);
	});

	bone_up_widget.init();
	bone_settings_group->addChild(createNamedField("Up: ", bone_up_widget.getWidget(), Gui::ALIGN_RIGHT));
	bone_up_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		int index = bones_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setLookAtChainBoneUp(bone_up_widget.getValue(), index, id);
	});

	bone_axis_widget.init();
	bone_settings_group->addChild(createNamedField("Axis: ", bone_axis_widget.getWidget(), Gui::ALIGN_RIGHT));
	bone_axis_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		int index = bones_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setLookAtChainBoneAxis(bone_axis_widget.getValue(), index, id);
	});
}

void LookAtEditor::update_chains_list()
{
	chains_list_box->clear();
	if (skinned.isValid() == false)
	{
		chain_changed();
		return;
	}

	int num = skinned->getNumLookAtChains();
	for (int i = 0; i < num; i++)
	{
		int id = skinned->getLookAtChainID(i);
		chains_list_box->addItem(String::format("%d) chain ID %d", i + 1, id));
	}
	chain_changed();
}

void LookAtEditor::update_bones_list()
{
	bones_list_box->clear();
	int id = getSelectedChainID();
	if (id == -1)
	{
		bone_changed();
		return;
	}

	int num = skinned->getLookAtChainNumBones(id);
	for (int i = 0; i < num; i++)
	{
		int bone = skinned->getLookAtChainBone(i, id);
		const char *name = skinned->getBoneName(bone);
		bones_list_box->addItem(String::format("%d) %s (%d)", i + 1, name, bone));
	}
	bone_changed();
}

void LookAtEditor::chain_changed()
{
	skinned->clearVisualizeLookAtChain();

	int id = getSelectedChainID();
	if (id == -1)
	{
		chain_settings_group->setEnabled(false);
		chain_enabled_check_box->setChecked(false);
		chain_constraint_combo_box->setCurrentItem(0);
		chain_weight_slider->setValue(1000);
		target_widget.setValue(Vec3_zero);
		pole_widget.setValue(Vec3_zero);

		bones_group->setEnabled(false);
	} else
	{
		chain_settings_group->setEnabled(true);
		chain_enabled_check_box->setChecked(skinned->isLookAtChainEnabled(id));
		chain_constraint_combo_box->setCurrentItem(skinned->getLookAtChainConstraint(id));
		chain_weight_slider->setValue(ftoi(1000 * skinned->getLookAtChainWeight(id)));
		target_widget.setValue(skinned->getLookAtChainTargetWorldPosition(id));
		pole_widget.setValue(skinned->getLookAtChainPoleWorldPosition(id));

		bones_group->setEnabled(true);

		skinned->addVisualizeLookAtChain(id);
	}

	update_bones_list();
}

void LookAtEditor::bone_changed()
{
	int id = getSelectedChainID();
	int bone_index = -1;
	if (id != -1)
		bone_index = bones_list_box->getCurrentItem();

	if (bone_index == -1)
	{
		bone_settings_group->setEnabled(false);
		bone_weight_slider->setValue(1000);
		bone_up_widget.setValue(Vec3_up);
		bone_axis_widget.setValue(Vec3_forward);

		if (last_visualized_bone != -1)
			skinned->removeVisualizeBone(last_visualized_bone);
	} else
	{
		bone_settings_group->setEnabled(true);
		bone_weight_slider->setValue(ftoi(1000 * skinned->getLookAtChainBoneWeight(bone_index, id)));
		bone_up_widget.setValue(skinned->getLookAtChainBoneUp(bone_index, id));
		bone_axis_widget.setValue(skinned->getLookAtChainBoneAxis(bone_index, id));

		int bone = skinned->getLookAtChainBone(bone_index, id);

		if (last_visualized_bone != -1)
			skinned->removeVisualizeBone(last_visualized_bone);
		skinned->addVisualizeBone(bone);
		last_visualized_bone = bone;
	}
}

// IK Editor

void IKEditor::init(ObjectMeshSkinnedPtr &obj, const BoneSelection *selection)
{
	if (main_group == nullptr)
		init_widgets();

	skinned = obj;
	bone_selection = selection;

	update_chains_list();
}

int IKEditor::getSelectedChainID() const
{
	if (chains_list_box == nullptr)
		return 1;

	int index = chains_list_box->getCurrentItem();
	if (index == -1)
		return -1;

	return skinned->getIKChainID(index);
}

void IKEditor::init_widgets()
{
	// groups
	main_group = createWidgetOutline<WidgetGroupBox>("<b>IK Editor</b>");
	main_group->setFontRich(1);
	main_group->setFontSize(14);

	grid_box = WidgetGridBox::create();
	main_group->addChild(grid_box, Gui::ALIGN_EXPAND);

	// chains
	chains_group = createWidgetOutline<WidgetGroupBox>("Chains");
	grid_box->addChild(chains_group, Gui::ALIGN_EXPAND);

	add_chain_button = createWidgetOutline<WidgetButton>("Add IK Chain");
	chains_group->addChild(add_chain_button, Gui::ALIGN_LEFT);
	add_chain_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (skinned.isValid() == false)
			return;

		skinned->addIKChain();
		update_chains_list();
	});

	remove_chain_button = createWidgetOutline<WidgetButton>("Remove IK Chain");
	chains_group->addChild(remove_chain_button, Gui::ALIGN_LEFT);
	remove_chain_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (skinned.isValid() == false)
			return;

		int id = getSelectedChainID();
		if (id != -1)
		{
			skinned->removeIKChain(id);
			update_chains_list();
		}
	});

	chains_list_box = createWidgetOutline<WidgetListBox>();
	chains_list_box->setMultiSelection(false);
	chains_group->addChild(chains_list_box, Gui::ALIGN_LEFT);
	chains_list_box->getEventChanged().connect(widget_connections, [this]()
	{
		chain_changed();
	});

	// chain settings
	chain_settings_group = createWidgetOutline<WidgetGroupBox>("Chain Settings");
	grid_box->addChild(chain_settings_group, Gui::ALIGN_EXPAND);

	chain_enabled_check_box = createWidgetOutline<WidgetCheckBox>("Enabled");
	chain_enabled_check_box->setChecked(true);
	chain_settings_group->addChild(chain_enabled_check_box, Gui::ALIGN_LEFT);
	chain_enabled_check_box->getEventClicked().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainEnabled(chain_enabled_check_box->isChecked(), id);
	});

	chain_constraint_combo_box = createWidgetOutline<WidgetComboBox>();
	chain_constraint_combo_box->addItem("NONE");
	chain_constraint_combo_box->addItem("POLE_VECTOR");
	chain_constraint_combo_box->addItem("BONE_ROTATIONS");
	chain_settings_group->addChild(createNamedField("Constraint: ", chain_constraint_combo_box), Gui::ALIGN_LEFT);
	chain_constraint_combo_box->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainConstraint(static_cast<ObjectMeshSkinned::CHAIN_CONSTRAINT>(chain_constraint_combo_box->getCurrentItem()), id);
	});

	chain_weight_slider = WidgetSlider::create(0, 1000, 1000);
	chain_settings_group->addChild(createNamedField("Weight: ", chain_weight_slider, Gui::ALIGN_EXPAND));
	chain_weight_slider->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		float weight = chain_weight_slider->getValue() / 1000.0f;
		skinned->setIKChainWeight(weight, id);
	});

	target_widget.init();
	chain_settings_group->addChild(createNamedField("Target: ", target_widget.getWidget(), Gui::ALIGN_RIGHT));
	target_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainTargetWorldPosition(target_widget.getValue(), id);
	});

	pole_widget.init();
	chain_settings_group->addChild(createNamedField("Pole: ", pole_widget.getWidget(), Gui::ALIGN_RIGHT));
	pole_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainPoleWorldPosition(pole_widget.getValue(), id);
	});

	chain_effector_rotation_check_box = createWidgetOutline<WidgetCheckBox>("Use Effector Rotation");
	chain_effector_rotation_check_box->setChecked(true);
	chain_settings_group->addChild(chain_effector_rotation_check_box, Gui::ALIGN_LEFT);
	chain_effector_rotation_check_box->getEventClicked().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainUseEffectorRotation(chain_effector_rotation_check_box->isChecked(), id);
	});

	rotation_widget.init();
	chain_settings_group->addChild(createNamedField("Effector Rotation: ", rotation_widget.getWidget(), Gui::ALIGN_RIGHT));
	rotation_widget.getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		quat rot(composeRotationZXY(vec3(rotation_widget.getValue())));
		skinned->setIKChainEffectorWorldRotation(rot, id);
	});

	chain_iterations_slider = WidgetSlider::create(0, 32, 8);
	chain_settings_group->addChild(createNamedField("Iterations: ", chain_iterations_slider, Gui::ALIGN_EXPAND));
	chain_iterations_slider->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainNumIterations(chain_iterations_slider->getValue(), id);
	});

	chain_tolerance_line = createWidgetOutline<WidgetEditLine>();
	chain_tolerance_line->setValidator(Gui::VALIDATOR_FLOAT);
	chain_tolerance_spin_box = WidgetSpinBoxDouble::create(0.0, 1.0, 0.0, 0.005);
	chain_tolerance_spin_box->addAttach(chain_tolerance_line);
	chain_settings_group->addChild(createNamedField("Tolerance: ", chain_tolerance_line, chain_tolerance_spin_box));
	chain_tolerance_line->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainTolerance(String::atof(chain_tolerance_line->getText()), id);
	});
	chain_tolerance_spin_box->getEventChanged().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		skinned->setIKChainTolerance(toFloat(chain_tolerance_spin_box->getValue()), id);
	});

	// bones
	bones_group = createWidgetOutline<WidgetGroupBox>("Bones In Chain");
	grid_box->addChild(bones_group, Gui::ALIGN_EXPAND);

	add_bone_button = createWidgetOutline<WidgetButton>("Add Bone From Selection");
	bones_group->addChild(add_bone_button, Gui::ALIGN_LEFT);
	add_bone_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (bone_selection == nullptr)
			return;

		int id = getSelectedChainID();
		if (id == -1)
			return;

		int bone = bone_selection->getSelectedBone();
		if (bone == -1)
			return;

		skinned->addIKChainBone(bone, id);
		update_bones_list();
	});

	remove_bone_button = createWidgetOutline<WidgetButton>("Remove Bone");
	bones_group->addChild(remove_bone_button, Gui::ALIGN_LEFT);
	remove_bone_button->getEventClicked().connect(widget_connections, [this]()
	{
		int id = getSelectedChainID();
		if (id == -1)
			return;

		int index = bones_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->removeIKChainBone(index, id);
		update_bones_list();
	});

	bones_list_box = createWidgetOutline<WidgetListBox>();
	bones_list_box->setMultiSelection(false);
	bones_group->addChild(bones_list_box, Gui::ALIGN_LEFT);
	bones_list_box->getEventChanged().connect(widget_connections, [this]()
	{
		bone_changed();
	});
}

void IKEditor::update_chains_list()
{
	chains_list_box->clear();
	if (skinned.isValid() == false)
	{
		chain_changed();
		return;
	}

	int num = skinned->getNumIKChains();
	for (int i = 0; i < num; i++)
	{
		int id = skinned->getIKChainID(i);
		chains_list_box->addItem(String::format("%d) chain ID %d", i + 1, id));
	}
	chain_changed();
}

void IKEditor::update_bones_list()
{
	bones_list_box->clear();
	int id = getSelectedChainID();
	if (id == -1)
	{
		bone_changed();
		return;
	}

	int num = skinned->getIKChainNumBones(id);
	for (int i = 0; i < num; i++)
	{
		int bone = skinned->getIKChainBone(i, id);
		const char *name = skinned->getBoneName(bone);
		bones_list_box->addItem(String::format("%d) %s (%d)", i + 1, name, bone));
	}
	bone_changed();
}

void IKEditor::chain_changed()
{
	skinned->clearVisualizeIKChain();

	int id = getSelectedChainID();
	if (id == -1)
	{
		chain_settings_group->setEnabled(false);
		chain_enabled_check_box->setChecked(false);
		chain_constraint_combo_box->setCurrentItem(0);
		chain_weight_slider->setValue(1000);
		target_widget.setValue(Vec3_zero);
		pole_widget.setValue(Vec3_zero);
		chain_effector_rotation_check_box->setChecked(false);
		rotation_widget.setValue(Vec3(decomposeRotationZXY(mat3(quat_identity))));
		chain_iterations_slider->setValue(0);
		chain_tolerance_spin_box->setValue(0.0);

		bones_group->setEnabled(false);
	} else
	{
		chain_settings_group->setEnabled(true);
		chain_enabled_check_box->setChecked(skinned->isIKChainEnabled(id));
		chain_constraint_combo_box->setCurrentItem(skinned->getIKChainConstraint(id));
		chain_weight_slider->setValue(ftoi(1000 * skinned->getIKChainWeight(id)));
		target_widget.setValue(skinned->getIKChainTargetWorldPosition(id));
		pole_widget.setValue(skinned->getIKChainPoleWorldPosition(id));
		chain_effector_rotation_check_box->setChecked(skinned->isIKChainUseEffectorRotation(id));
		rotation_widget.setValue(Vec3(decomposeRotationZXY(mat3(skinned->getIKChainEffectorWorldRotation(id)))));
		chain_iterations_slider->setValue(skinned->getIKChainNumIterations(id));
		chain_tolerance_spin_box->setValue(toDouble(skinned->getIKChainTolerance(id)));

		bones_group->setEnabled(true);

		skinned->addVisualizeIKChain(id);
	}

	update_bones_list();
}

void IKEditor::bone_changed()
{
	int id = getSelectedChainID();
	int bone_index = -1;
	if (id != -1)
		bone_index = bones_list_box->getCurrentItem();

	if (bone_index != -1)
	{
		int bone = skinned->getIKChainBone(bone_index, id);

		if (last_visualized_bone != -1)
			skinned->removeVisualizeBone(last_visualized_bone);
		skinned->addVisualizeBone(bone);
		last_visualized_bone = bone;
	}
}

// Constraint Editor

void ConstraintEditor::init(ObjectMeshSkinnedPtr &obj, const BoneSelection *selection)
{
	if (main_group == nullptr)
		init_widgets();

	skinned = obj;
	bone_selection = selection;

	update_constraints_list();
}

void ConstraintEditor::init_widgets()
{
	// groups
	main_group = createWidgetOutline<WidgetGroupBox>("<b>Constraint Editor</b>");
	main_group->setFontRich(1);
	main_group->setFontSize(14);

	grid_box = WidgetGridBox::create();
	main_group->addChild(grid_box, Gui::ALIGN_EXPAND);

	// constraints
	constraints_group = createWidgetOutline<WidgetGroupBox>("Constraints");
	grid_box->addChild(constraints_group, Gui::ALIGN_EXPAND);

	add_constraint_button = createWidgetOutline<WidgetButton>("Add Bone From Selection");
	constraints_group->addChild(add_constraint_button, Gui::ALIGN_LEFT);
	add_constraint_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (bone_selection == nullptr)
			return;

		if (skinned == nullptr)
			return;

		int bone = bone_selection->getSelectedBone();
		if (bone == -1)
			return;

		int num = skinned->getNumBoneConstraints();
		for (int i = 0; i < num; i++)
		{
			if (skinned->getBoneConstraintBoneIndex(i) == bone)
				return;
		}

		skinned->addBoneConstraint(bone);
		update_constraints_list();
	});

	remove_constraint_button = createWidgetOutline<WidgetButton>("Remove Bone");
	constraints_group->addChild(remove_constraint_button, Gui::ALIGN_LEFT);
	remove_constraint_button->getEventClicked().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->removeBoneConstraint(index);
		update_constraints_list();
	});

	constraints_list_box = createWidgetOutline<WidgetListBox>();
	constraints_list_box->setMultiSelection(false);
	constraints_group->addChild(constraints_list_box, Gui::ALIGN_LEFT);
	constraints_list_box->getEventChanged().connect(widget_connections, [this]()
	{
		constraint_changed();
	});

	// constraint settings
	constraint_settings_group = createWidgetOutline<WidgetGroupBox>("Constraint Settings");
	grid_box->addChild(constraint_settings_group, Gui::ALIGN_EXPAND);

	enabled_check_box = createWidgetOutline<WidgetCheckBox>("Enabled");
	constraint_settings_group->addChild(enabled_check_box, Gui::ALIGN_LEFT);
	enabled_check_box->getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setBoneConstraintEnabled(enabled_check_box->isChecked(), index);
	});

	yaw_axis_widget.init();
	constraint_settings_group->addChild(createNamedField("Yaw Axis: ", yaw_axis_widget.getWidget(), Gui::ALIGN_RIGHT));
	yaw_axis_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setBoneConstraintYawAxis(vec3(yaw_axis_widget.getValue()), index);
	});

	pitch_axis_widget.init();
	constraint_settings_group->addChild(createNamedField("Pitch Axis: ", pitch_axis_widget.getWidget(), Gui::ALIGN_RIGHT));
	pitch_axis_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setBoneConstraintPitchAxis(vec3(pitch_axis_widget.getValue()), index);
	});

	roll_axis_widget.init();
	constraint_settings_group->addChild(createNamedField("Roll Axis: ", roll_axis_widget.getWidget(), Gui::ALIGN_RIGHT));
	roll_axis_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		skinned->setBoneConstraintRollAxis(vec3(roll_axis_widget.getValue()), index);
	});

	yaw_angles_widget.init();
	constraint_settings_group->addChild(createNamedField("Yaw Angles: ", yaw_angles_widget.getWidget(), Gui::ALIGN_RIGHT));
	yaw_angles_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		vec2 angles(yaw_angles_widget.getValue());
		skinned->setBoneConstraintYawAngles(angles.x, angles.y, index);
	});

	pitch_angles_widget.init();
	constraint_settings_group->addChild(createNamedField("Pitch Angles: ", pitch_angles_widget.getWidget(), Gui::ALIGN_RIGHT));
	pitch_angles_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		vec2 angles(pitch_angles_widget.getValue());
		skinned->setBoneConstraintPitchAngles(angles.x, angles.y, index);
	});

	roll_angles_widget.init();
	constraint_settings_group->addChild(createNamedField("Roll Angles: ", roll_angles_widget.getWidget(), Gui::ALIGN_RIGHT));
	roll_angles_widget.getEventChanged().connect(widget_connections, [this]()
	{
		if (skinned == nullptr)
			return;

		int index = constraints_list_box->getCurrentItem();
		if (index == -1)
			return;

		vec2 angles(roll_angles_widget.getValue());
		skinned->setBoneConstraintRollAngles(angles.x, angles.y, index);
	});
}

void ConstraintEditor::update_constraints_list()
{
	constraints_list_box->clear();
	if (skinned.isValid() == false)
	{
		constraint_changed();
		return;
	}

	int num = skinned->getNumBoneConstraints();
	for (int i = 0; i < num; i++)
	{
		int bone = skinned->getBoneConstraintBoneIndex(i);
		const char *name = skinned->getBoneName(bone);

		constraints_list_box->addItem(String::format("%d) %s (%d)", i + 1, name, bone));
	}
	constraint_changed();
}

void ConstraintEditor::constraint_changed()
{
	if (skinned == nullptr)
		return;

	skinned->clearVisualizeConstraint();

	int index = constraints_list_box->getCurrentItem();
	if (index == -1)
	{
		constraint_settings_group->setEnabled(false);
		enabled_check_box->setChecked(false);
		yaw_axis_widget.setValue(Vec3_zero);
		pitch_axis_widget.setValue(Vec3_zero);
		roll_axis_widget.setValue(Vec3_zero);
		yaw_angles_widget.setValue(Vec2(-180.0f, 180.0f));
		pitch_angles_widget.setValue(Vec2(-180.0f, 180.0f));
		roll_angles_widget.setValue(Vec2(-180.0f, 180.0f));
	} else
	{
		constraint_settings_group->setEnabled(true);
		enabled_check_box->setChecked(skinned->isBoneConstraintEnabled(index));
		yaw_axis_widget.setValue(Vec3(skinned->getBoneConstraintYawAxis(index)));
		pitch_axis_widget.setValue(Vec3(skinned->getBoneConstraintPitchAxis(index)));
		roll_axis_widget.setValue(Vec3(skinned->getBoneConstraintRollAxis(index)));
		yaw_angles_widget.setValue(Vec2(skinned->getBoneConstraintYawMinAngle(index), skinned->getBoneConstraintYawMaxAngle(index)));
		pitch_angles_widget.setValue(Vec2(skinned->getBoneConstraintPitchMinAngle(index), skinned->getBoneConstraintPitchMaxAngle(index)));
		roll_angles_widget.setValue(Vec2(skinned->getBoneConstraintRollMinAngle(index), skinned->getBoneConstraintRollMaxAngle(index)));

		skinned->addVisualizeConstraint(index);
	}
}

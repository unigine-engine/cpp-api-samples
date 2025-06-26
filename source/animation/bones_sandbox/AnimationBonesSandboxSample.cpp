#include "AnimationBonesSandboxSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesSandboxSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesSandboxSample::init()
{
#ifndef DEBUG
	if (Engine::get()->getBuildConfiguration() == Engine::BUILD_CONFIG_RELEASE)
		Log::warning("Current build configuration is Release. Visualization of ObjectMeshSkinned "
					 "Bones is not available with this build configuration\n");
#endif

	skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesSandboxSample::init(): skinned is null\n");
		return;
	}

	skinned->setVisualizeAllBones(true);

	// LookAt chain
	{
		int id = skinned->addLookAtChain();
		skinned->setLookAtChainTargetWorldPosition(Vec3(0.0f, 2.0f, 1.5f), id);
		skinned->setLookAtChainPoleWorldPosition(Vec3(0.0f, 0.0f, 3.0f), id);

		int index = -1;

		index = skinned->addLookAtChainBone("spine_1", id);
		skinned->setLookAtChainBoneAxis(Vec3(0.0f, 0.f, 1.0f), index, id);
		skinned->setLookAtChainBoneUp(Vec3(0.0f, 1.0f, 0.0f), index, id);
		skinned->setLookAtChainBoneWeight(0.05f, index, id);

		index = skinned->addLookAtChainBone("spine_2", id);
		skinned->setLookAtChainBoneAxis(Vec3(0.0f, 0.f, 1.0f), index, id);
		skinned->setLookAtChainBoneUp(Vec3(0.0f, 1.0f, 0.0f), index, id);
		skinned->setLookAtChainBoneWeight(0.1f, index, id);

		index = skinned->addLookAtChainBone("spine_3", id);
		skinned->setLookAtChainBoneAxis(Vec3(0.0f, 0.f, 1.0f), index, id);
		skinned->setLookAtChainBoneUp(Vec3(0.0f, 1.0f, 0.0f), index, id);
		skinned->setLookAtChainBoneWeight(0.15f, index, id);

		index = skinned->addLookAtChainBone("neck", id);
		skinned->setLookAtChainBoneAxis(Vec3(0.0f, 0.f, 1.0f), index, id);
		skinned->setLookAtChainBoneUp(Vec3(0.0f, 1.0f, 0.0f), index, id);
		skinned->setLookAtChainBoneWeight(0.2f, index, id);

		index = skinned->addLookAtChainBone("head", id);
		skinned->setLookAtChainBoneAxis(Vec3(0.0f, 0.f, 1.0f), index, id);
		skinned->setLookAtChainBoneUp(Vec3(0.0f, 1.0f, 0.0f), index, id);
		skinned->setLookAtChainBoneWeight(1.0f, index, id);
	}

	// IK chain
	{
		int id = skinned->addIKChain();
		int bone = skinned->findBone("foot_l");
		skinned->setIKChainTargetWorldPosition(skinned->getBoneWorldTransform(bone).getTranslate(), id);
		bone = skinned->findBone("shin_l");
		skinned->setIKChainPoleWorldPosition(skinned->getBoneWorldTransform(bone).getTranslate() + Vec3_forward, id);

		skinned->addIKChainBone("thigh_l", id);
		skinned->addIKChainBone("shin_l", id);
		skinned->addIKChainBone("foot_l", id);
	}

	// constraints
	{
		int index = skinned->addBoneConstraint("head");
		skinned->setBoneConstraintYawAngles(-40.0f, 40.0f, index);
		skinned->setBoneConstraintYawAxis(vec3(0.0f, 1.0f, 0.0f), index);
		skinned->setBoneConstraintPitchAngles(-40.0f, 40.0f, index);
		skinned->setBoneConstraintPitchAxis(vec3(1.0f, 0.0f, 0.0f), index);
		skinned->setBoneConstraintRollAngles(-0.0f, 0.0f, index);
		skinned->setBoneConstraintRollAxis(vec3(0.0f, 0.0f, 1.0f), index);
	}

	main_hbox = WidgetHBox::create();
	main_hbox->setLifetime(Widget::LIFETIME_WINDOW);

	main_vbox = WidgetVBox::create(0, 5);
	main_vbox->setBackground(1);
	main_hbox->addChild(main_vbox, Gui::ALIGN_EXPAND);

	bone_selection.init(skinned);
	main_vbox->addChild(bone_selection.getWidget(), Gui::ALIGN_LEFT);

	look_at_editor.init(skinned, &bone_selection);
	main_vbox->addChild(look_at_editor.getWidget(), Gui::ALIGN_LEFT);

	ik_editor.init(skinned, &bone_selection);
	main_vbox->addChild(ik_editor.getWidget(), Gui::ALIGN_LEFT);

	constraint_editor.init(skinned, &bone_selection);
	main_vbox->addChild(constraint_editor.getWidget(), Gui::ALIGN_LEFT);

	editors_window = EngineWindowViewport::create("Bones Sandbox Editors", 600, 1000);
	editors_window->setCanBeNested(false);
	editors_window->setCanCreateGroup(false);
	editors_window->addChild(main_hbox, Gui::ALIGN_EXPAND);
	editors_window->setPosition(ivec2(40, 40));
	editors_window->show();

	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesSandboxSample::shutdown()
{
	editors_window.deleteLater();

	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}

#include "ComponentSystemSample.h"

#include "Pawn.h"
#include "Spinner.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ComponentSystemSample);

using namespace Unigine;
using namespace Math;

void ComponentSystemSample::init()
{
	// create static camera
	//	player = PlayerDummy::create();
	//	Game::setPlayer(player);

	// create invisible ground for Player Actor (Component)
	//	ground = ObjectMeshDynamic::create();
	//	MeshPtr mesh_plane = Mesh::create();
	//	mesh_plane->addPlaneSurface("plane", 60, 60, 10);
	//	ground->setMesh(mesh_plane);
	//	ground->setPosition(Vec3(0, 0, -0.5f));
	//	ground->setViewportMask(0, 0);

	// Note:
	// If you create objects without any components, you have to
	// declare their smart pointers (Ptr) outside of init() method
	// (just like the PlayerDummy and the LightWorld above).
	// Otherwise that kind of objects will be destroyed right after
	// init() method finished.

	// On the other hand, you can create objects in a local scope
	// (like ObjectMeshDynamics below) and they will not be destroyed,
	// but you have to use release() method + adding this node to
	// the Editor

	// create objects
	ObjectMeshDynamicPtr obj[4];
	obj[0] = create_box(translate(Vec3(-16.0f, 0.0f, 0.0f)), vec3(1.0f));
	obj[1] = create_box(translate(Vec3(16.0f, 0.0f, 0.0f)), vec3(1.0f));
	obj[2] = create_box(translate(Vec3(0.0f, -16.0f, 0.0f)), vec3(1.0f));
	obj[3] = create_box(translate(Vec3(0.0f, 16.0f, 0.0f)), vec3(1.0f));

	// there are two ways to create components on nodes:
	// 1) via component system
	ComponentSystem::get()->addComponent<Spinner>(obj[0]);
	ComponentSystem::get()->addComponent<Spinner>(obj[1]);

	// 2) via property
	obj[2]->addProperty("Spinner");
	obj[3]->setProperty(0, "Spinner");

	// set up spinners (set "spawn_node" variable)
	ObjectMeshDynamicPtr projectile_obj = create_box(Mat4_identity, vec3(0.15f));
	projectile_obj->setEnabled(0);
	for (int i = 0; i < 4; i++)
		ComponentSystem::get()->getComponent<Spinner>(obj[i])->spawn_node = projectile_obj;

	// create player
	ObjectMeshDynamicPtr my_pawn_object = create_box(translate(Vec3(1.0f, 1.0f, 0.0f)),
		vec3(1.3f, 1.3f, 0.3f));
	my_pawn = ComponentSystem::get()->addComponent<Pawn>(my_pawn_object);
	my_pawn->setDestroyCallback(MakeCallback(this, &ComponentSystemSample::my_pawn_destroyed));
	time = 0;

	// create info label
	label = WidgetLabel::create(Gui::getCurrent());
	label->setPosition(10, 10);
	label->setFontSize(24);
	label->setFontOutline(1);
	Gui::getCurrent()->addChild(label, Gui::ALIGN_OVERLAP);
}

void ComponentSystemSample::update()
{
	// increase time while player is alive
	if (my_pawn)
		time += Game::getIFps();

	// show info
	label->setText(String::format("Player:\n"
								  "Health Points: %d\n"
								  "Time: %.1f sec\n"
								  "\n"
								  "Statisics:\n"
								  "Components: %d",
		(my_pawn ? my_pawn->health.get() : 0), time, ComponentSystem::get()->getNumComponents())
			.get());
}

void ComponentSystemSample::shutdown()
{
	// nodes will be deleted automatically during world shutdown,
	// but widgets can live outside the world. We need to delete them manually.
	label.deleteLater();
}

Unigine::ObjectMeshDynamicPtr ComponentSystemSample::create_box(
	const Unigine::Math::Mat4 &transform, const Unigine::Math::vec3 &size)
{
	MeshPtr mesh = Mesh::create();
	mesh->addBoxSurface("box", size);

	ObjectMeshDynamicPtr object = ObjectMeshDynamic::create(1);
	object->setMesh(mesh);
	object->setWorldTransform(transform);

	return object;
}

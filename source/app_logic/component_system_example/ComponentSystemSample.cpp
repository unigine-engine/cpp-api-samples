// Demonstrates Component System usage: creating nodes with components at runtime,
// two attachment methods (C++ API vs property-based), component-to-component communication,
// and proper memory management (widgets must be deleted manually, nodes auto-delete).

#include "ComponentSystemSample.h"

#include "Pawn.h"
#include "Spinner.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ComponentSystemSample);

using namespace Unigine;
using namespace Math;

// Scene is initialized: spinners, pawn, and UI are created and configured.
void ComponentSystemSample::init()
{
	// Create objects
	ObjectMeshDynamicPtr obj[4];
	obj[0] = create_box(translate(Vec3(-16.0f, 0.0f, 0.0f)), vec3(1.0f));
	obj[1] = create_box(translate(Vec3(16.0f, 0.0f, 0.0f)), vec3(1.0f));
	obj[2] = create_box(translate(Vec3(0.0f, -16.0f, 0.0f)), vec3(1.0f));
	obj[3] = create_box(translate(Vec3(0.0f, 16.0f, 0.0f)), vec3(1.0f));

	// Two methods to attach components to nodes:
	// 1) C++ API: ComponentSystem::addComponent<T>() - type-safe, immediate
	ComponentSystem::get()->addComponent<Spinner>(obj[0]);
	ComponentSystem::get()->addComponent<Spinner>(obj[1]);

	// 2) Property-based: add property by name - used for editor-created components
	obj[2]->addProperty("Spinner");
	obj[3]->setProperty(0, "Spinner");

	// Set up spinners (set "spawn_node" variable)
	ObjectMeshDynamicPtr projectile_obj = create_box(Mat4_identity, vec3(0.15f));
	projectile_obj->setEnabled(0);
	for (int i = 0; i < 4; i++)
		ComponentSystem::get()->getComponent<Spinner>(obj[i])->spawn_node = projectile_obj;

	// Create player
	ObjectMeshDynamicPtr my_pawn_object = create_box(translate(Vec3(1.0f, 1.0f, 0.0f)),
		vec3(1.3f, 1.3f, 0.3f));
	my_pawn = ComponentSystem::get()->addComponent<Pawn>(my_pawn_object);
	my_pawn->setDestroyCallback(MakeCallback(this, &ComponentSystemSample::my_pawn_destroyed));
	time = 0;

	// Create info label
	label = WidgetLabel::create(Gui::getCurrent());
	label->setPosition(10, 10);
	label->setFontSize(24);
	label->setFontOutline(1);
	Gui::getCurrent()->addChild(label, Gui::ALIGN_OVERLAP);
}

// Survival time is updated and statistics are displayed in UI.
void ComponentSystemSample::update()
{
	// Increase time while player is alive
	if (my_pawn)
		time += Game::getIFps();

	// Show info
	label->setText(String::format("Player:\n"
								  "Health Points: %d\n"
								  "Time: %.1f sec\n"
								  "\n"
								  "Statisics:\n"
								  "Components: %d",
		(my_pawn ? my_pawn->health.get() : 0), time, ComponentSystem::get()->getNumComponents())
			.get());
}

// GUI widgets are released manually (nodes are auto-deleted during world shutdown).
void ComponentSystemSample::shutdown()
{
	// Nodes will be deleted automatically during world shutdown,
	// but widgets can live outside the world. They need to be deleted manually.
	label.deleteLater();
}

// Creates a box mesh with the given transform and size.
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

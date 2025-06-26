#include "ClusterSample.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineWidgets.h>
#include <UnigineWorld.h>

REGISTER_COMPONENT(ClusterSample);

using namespace Unigine;
using namespace Math;

// z-coordinate for meshes
const float OFFSET_Z = 0.5f;

void ClusterSample::init()
{
	cluster = checked_ptr_cast<ObjectMeshCluster>(clusterNode.get());
	if (!cluster)
		Log::error("ClusterSample::init(): can not get clusterNode property\n");

	sample_description_window.createWindow();
	update_gui();
}

void ClusterSample::update()
{
	if (Console::isActive())
		return;

	// remove/add mesh in cluster
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
	{
		// select mesh or empty space by mouse
		ivec2 mouse = Input::getMousePosition();
		Vec3 p0 = Game::getPlayer()->getWorldPosition();
		Vec3 p1 = p0 + Vec3(Game::getPlayer()->getDirectionFromMainWindow(mouse.x, mouse.y)) * 100;

		// check intersection with cluster or ground
		ObjectPtr obj = World::getIntersection(p0, p1, intersection_mask.get(), intersection);
		if (obj)
		{
			// if obj is ObjectMeshCluster then remove mesh
			if (obj == cluster)
			{
				int num = intersection->getInstance();
				cluster->removeMeshTransform(num);
			}
			else
			{
				// create transformation matrix for new mesh
				Vec3 point = intersection->getPoint();
				point.z = OFFSET_Z;

				// for add single mesh in local space
				int new_index = cluster->addMeshTransform();
				cluster->setMeshTransform(new_index,
					mat4(cluster->getIWorldTransform() * translate(point)));
				// for add a lot of meshes in global space
				// Vector<Mat4> transforms = Vector<Mat4>(translate(point), 1);
				// cluster->appendMeshes(transforms);
			}
		}
		update_gui();
	}
}

void ClusterSample::shutdown()
{
	shutdown_gui();
}

void ClusterSample::init_gui() {}

void ClusterSample::update_gui()
{
	sample_description_window.setStatus(
		String::format("Number of meshes in the cluster: %d", cluster->getNumMeshes()).get());
}

void ClusterSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

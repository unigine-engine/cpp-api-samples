// Interactive ObjectMeshCluster demonstration. Clicking on the ground adds
// instances at the clicked position; clicking on existing instances removes them.
// Demonstrates spatial tree updates required after modifying cluster contents.

#include "ClusterSample.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineWidgets.h>
#include <UnigineWorld.h>

REGISTER_COMPONENT(ClusterSample);

using namespace Unigine;
using namespace Math;

// Fixed height offset for placed instances (above ground plane)
const float OFFSET_Z = 0.5f;

void ClusterSample::init()
{
	// Cast generic Node reference to ObjectMeshCluster for type-safe API access
	cluster = checked_ptr_cast<ObjectMeshCluster>(clusterNode.get());
	if (!cluster)
		Log::error("ClusterSample::init(): can not get clusterNode property\n");

	sample_description_window.createWindow();
	update_gui();
}

void ClusterSample::update()
{
	// Skip input processing when console is open to avoid conflicts
	if (Console::isActive())
		return;

	// Handle left mouse click for add/remove operations
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
	{
		// Cast ray from camera through mouse cursor position
		ivec2 mouse = Input::getMousePosition();
		Vec3 p0 = Game::getPlayer()->getWorldPosition();
		// Ray extends 100 units from camera in view direction
		Vec3 p1 = p0 + Vec3(Game::getPlayer()->getDirectionFromMainWindow(mouse.x, mouse.y)) * 100;

		// Perform world intersection test against objects matching the mask
		ObjectPtr obj = World::getIntersection(p0, p1, intersection_mask.get(), intersection);
		if (obj)
		{
			// Check if the hit object is our cluster (remove instance)
			if (obj == cluster)
			{
				// getInstance() returns which cluster instance was hit
				int num = intersection->getInstance();
				cluster->removeMeshTransform(num);
			}
			else
			{
				// Hit ground or other object - add new instance at click point
				Vec3 point = intersection->getPoint();
				// Override Z to place instance at fixed height
				point.z = OFFSET_Z;

				// Add instance using local-space transform relative to cluster
				int new_index = cluster->addMeshTransform();
				// Convert world position to cluster's local space
				cluster->setMeshTransform(new_index,
					mat4(cluster->getIWorldTransform() * translate(point)));

				// Alternative: add multiple instances in world space at once
				// Vector<Mat4> transforms = Vector<Mat4>(translate(point), 1);
				// cluster->appendMeshes(transforms);
			}

			// Spatial tree must be updated after any cluster modification.
			// This rebuilds the internal acceleration structure used for:
			// - Frustum culling (visibility determination)
			// - Intersection queries (raycasting against instances)
			// - LOD calculations based on camera distance
			cluster->updateSpatialTree();
		}

		// Refresh UI to show updated instance count
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
	// Display current number of mesh instances in the cluster
	sample_description_window.setStatus(
		String::format("Number of meshes in the cluster: %d", cluster->getNumMeshes()).get());
}

void ClusterSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

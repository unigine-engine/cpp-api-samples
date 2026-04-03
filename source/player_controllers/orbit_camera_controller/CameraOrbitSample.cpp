// Implements an orbiting camera that rotates around a target node.
// Camera stays at configurable distance, with vertical angle limits
// to prevent gimbal lock. Direction is computed from spherical
// coordinates (horizontal and vertical angles).

#include "../../utils/CameraControls.h"
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Unigine::Math;

// Third-person orbit camera with distance zoom and angle constraints.
class CameraOrbitSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(CameraOrbitSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, controls_node);					// Node with CameraControls component
	PROP_PARAM(Float, angularSpeed, 90.0f)				// Rotation speed in degrees per second
	PROP_PARAM(Float, zoomSpeed, 3.0f);					// Zoom speed in units per second
	PROP_PARAM(Float, minDistance, 5.0f);				// Closest allowed distance to target
	PROP_PARAM(Float, maxDistance, 10.0f);				// Farthest allowed distance from target
	PROP_PARAM(Float, minVerticalAngle, -89.9f);		// Lower pitch limit (near -90 to avoid gimbal lock)
	PROP_PARAM(Float, maxVerticalAngle, 89.9f);			// Upper pitch limit (near 90 to avoid gimbal lock)
	PROP_PARAM(Node, target);							// Node that camera orbits around

private:
	CameraControls *controls = nullptr;
	PlayerDummyPtr camera = nullptr;
	float horizontalAngle = 0.0f;	// Yaw angle around target (spherical phi)
	float verticalAngle = 0.0f;		// Pitch angle (spherical theta)
	float distance = 0.0f;			// Current distance from target
	Input::MOUSE_HANDLE init_mouse_handle;	// Saved mouse mode for restoration on shutdown

private:
	// Initial angles are extracted from current camera-to-target direction.
	void init()
	{
		camera = static_ptr_cast<PlayerDummy>(node);
		if (!camera)
		{
			return;
		}

		if (!target)
		{
			return;
		}

		controls = getComponent<CameraControls>(controls_node);
		if (!controls)
		{
			return;
		}

		init_mouse_handle = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

		// Direction from camera to target is used to compute initial spherical angles
		vec3 direction(target->getWorldPosition() - camera->getWorldPosition());
		direction.normalize();

		// Horizontal direction (XY projection) is needed for angle decomposition
		vec3 horizontalDirection = direction;
		horizontalDirection.z = 0;
		horizontalDirection.normalize();

		// Vertical angle: elevation from horizontal plane
		verticalAngle = getAngle(direction, horizontalDirection);
		verticalAngle *= -sign(direction.z);
		verticalAngle = clamp(verticalAngle, minVerticalAngle, maxVerticalAngle);

		// Horizontal angle: rotation from forward axis
		horizontalAngle = getAngle(horizontalDirection, vec3_forward);
		horizontalAngle *= sign(direction.x);

		// Camera starts at middle distance, looking at target
		distance = minDistance + (maxDistance - minDistance) * 0.5f;
		camera->setWorldDirection(direction, vec3_up);
		camera->setWorldPosition(target->getWorldPosition() - Vec3(direction * distance));
	}

	// Spherical angles and distance are updated from input, then camera is repositioned.
	void update()
	{
		if (!camera || !target || !controls)
			return;

		// Pitch is clamped to prevent flipping over the poles
		verticalAngle -= controls->getTurnUp() * angularSpeed * Game::getIFps();
		verticalAngle += controls->getTurnDown() * angularSpeed * Game::getIFps();
		verticalAngle = clamp(verticalAngle, minVerticalAngle, maxVerticalAngle);

		horizontalAngle += controls->getTurnRight() * angularSpeed * Game::getIFps();
		horizontalAngle -= controls->getTurnLeft() * angularSpeed * Game::getIFps();

		// Yaw wraps around at ±180 degrees
		if (horizontalAngle < -180 || 180 < horizontalAngle)
			horizontalAngle = clamp(-horizontalAngle, -180.0f, 180.0f);

		// Direction is reconstructed from spherical coordinates (yaw then pitch)
		vec3 cameraDirection = vec3_forward * rotateZ(horizontalAngle);
		cameraDirection = cameraDirection * rotate(cross(cameraDirection, vec3_up), verticalAngle);

		distance -= controls->getZoomIn() * zoomSpeed * Game::getIFps();
		distance += controls->getZoomOut() * zoomSpeed * Game::getIFps();
		distance = clamp(distance, minDistance, maxDistance);

		// Camera is placed along direction vector at specified distance from target
		camera->setWorldDirection(cameraDirection, vec3_up);
		camera->setWorldPosition(target->getWorldPosition() - Vec3(cameraDirection * distance));
	}

	// Restores original mouse handling mode when component is destroyed.
	void shutdown()
	{
		Input::setMouseHandle(init_mouse_handle);
	}
};

REGISTER_COMPONENT(CameraOrbitSample);

// RTS-style top-down camera with zoom, rotation, edge scrolling, and
// middle-mouse panning. Uses spherical coordinates (phi/theta) for viewing
// angle, with theta coupled to distance for a more top-down view when zoomed out.
// Supports smooth interpolation to target position/angle.

#include "TopDownController.h"
#include "Selection.h"
#include <UnigineConsole.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(TopDownController);

using namespace Unigine;
using namespace Math;

// Instantly places the camera pivot to the specified position.
void TopDownController::setPosition(const Unigine::Math::Vec3& pos)
{
	camera_pivot_position = pos;
	camera_pivot_position.z = 2.0f;
	current_camera_pivot_position = camera_pivot_position;
}

// Sets the target position to which the camera approaches smoothly.
void TopDownController::setTargetPosition(const Unigine::Math::Vec3& pos)
{
	camera_pivot_position = pos;
	camera_pivot_position.z = 2.0f;
}

// Instantly sets the distance and recalculates theta angle.
void TopDownController::setDistance(float dist)
{
	distance = dist;

	distance = clamp(distance, min_distance, max_distance);
	theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);

	current_distance = distance;
	current_theta = theta;
}

// Sets the target distance for smooth zoom animation.
void TopDownController::setTargetDistance(float dist)
{
	distance = dist;

	distance = clamp(distance, min_distance, max_distance);
	theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);
}

// Initializes camera parameters from properties, calculates degrees_per_unit ratio,
// sets up mouse handling, and creates the Selection component for unit selection.
void TopDownController::init()
{
	// Setting default values from properties
	phi = phiProp.get();
	current_phi = phi;
	theta = thetaProp.get();
	current_theta = theta;
	min_theta = thetaMinMax.get().x;
	max_theta = thetaMinMax.get().y;

	distance = Distance.get();
	current_distance = distance;
	min_distance = distanceMinMax.get().x;
	max_distance = distanceMinMax.get().y;

	interpolation_factor = zoomSpeed.get();

	if (max_distance != min_distance)
		degrees_per_unit = (max_theta - min_theta) / (max_distance - min_distance);
	else
		degrees_per_unit = 0.0f;

	// Setting up camera
	camera = checked_ptr_cast<Player>(node);
	if (!camera)
	{
		Log::error("TopDownController::init(): camera is not valid\n");
		return;
	}

	camera_pivot_position = node->getPosition();
	camera_pivot_position.z = 2.0f;
	current_camera_pivot_position = camera_pivot_position;

	// Setting up input handle, mouse will not be grabbed
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	// Setting camera to the start position
	vec3 cameraViewDirection = quat(vec3_up, current_phi) * vec3_forward;
	cameraViewDirection = quat(cross(vec3_up, cameraViewDirection), -current_theta) * cameraViewDirection * -1;
	cameraViewDirection.normalize();

	camera->setViewDirection(cameraViewDirection);
	camera->setWorldPosition(current_camera_pivot_position - Vec3(cameraViewDirection) * current_distance);

	// Creating a logic node and assigning the Selection component to it
	NodeDummyPtr logic = NodeDummy::create();
	selection = ComponentSystem::get()->addComponent<Selection>(logic);
}

// Processes input for zoom, rotation, panning, focus, and edge scrolling.
// Updates target values that will be interpolated toward in postUpdate.
void TopDownController::update()
{
	if (!camera)
		return;

	// Input processing only when the Console is inactive
	if (!Console::isActive())
	{
		// When the middle mouse button is pressed, save initial vector for panning
		if (Input::isMouseButtonDown(Input::MOUSE_BUTTON::MOUSE_BUTTON_MIDDLE))
		{
			auto object = World::getIntersection(camera->getWorldPosition(), camera->getWorldPosition() + Vec3(camera->getDirectionFromMainWindow(Input::getMousePosition().x, Input::getMousePosition().y) * 10000), ~0, intersection);
			if (object)
			{
				is_previous_hooked = true;
				previous_mouse_to_intersection_point_vector = intersection->getPoint() - camera->getWorldPosition();
			}
			else
			{
				is_previous_hooked = false;
			}
		}

		// Process input for zoom (mouse wheel)
		int mouseAxis = Input::getMouseWheel();
		if (mouseAxis != 0)
		{
			distance = clamp(distance - mouseAxis, min_distance, max_distance);
			theta = clamp(theta - mouseAxis * degrees_per_unit, min_theta, max_theta);
		}

		// Process input for rotation around the Z axis (Q/E keys)
		if (Input::isKeyPressed(Input::KEY_Q))
			phi -= 50.0f * Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_E))
			phi += 50.0f * Game::getIFps();

		// When key F is pressed and an object is selected, focus the camera on selection
		if (Input::isKeyPressed(Input::KEY_F) && selection->hasSelection())
		{
			distance = selection->getBoundRadius();
			camera_pivot_position = selection->getCenter();
			camera_pivot_position.z = 2.0f;

			distance = clamp(distance, min_distance, max_distance);
			theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);
		}

		// If middle button is held down, update intersection vector to move the camera
		if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_MIDDLE) && is_previous_hooked)
		{
			current_mouse_to_intersection_point_vector = Vec3(camera->getDirectionFromMainWindow(Input::getMousePosition().x, Input::getMousePosition().y));
			current_mouse_to_intersection_point_vector *= previous_mouse_to_intersection_point_vector.z / current_mouse_to_intersection_point_vector.z;

			Vec3 displacement = current_mouse_to_intersection_point_vector - previous_mouse_to_intersection_point_vector;

			camera_pivot_position -= displacement;
			current_camera_pivot_position -= displacement;
			previous_mouse_to_intersection_point_vector = current_mouse_to_intersection_point_vector;
		}
		// Only when middle button is up, allow edge scrolling
		else
		{
			Vec3 forward = quat(vec3_up, phi) * Vec3_forward * -1;
			forward.normalize();
			Vec3 right = quat(vec3_up, phi) * Vec3_right * -1;
			right.normalize();
			ivec2 mousePosition = Input::getMousePosition();
			auto windowPos = WindowManager::getMainWindow()->getPosition();
			auto windowSize = WindowManager::getMainWindow()->getRenderSize();

			if (mousePosition.x < windowPos.x + 10)
				camera_pivot_position -= right * 10.0f * Game::getIFps();

			if (mousePosition.y < windowPos.y + 10)
				camera_pivot_position += forward * 10.0f * Game::getIFps();

			if (mousePosition.x > windowPos.x + windowSize.x - 10)
				camera_pivot_position += right * 10.0f * Game::getIFps();

			if (mousePosition.y > windowPos.y + windowSize.y - 10)
				camera_pivot_position -= forward * 10.0f * Game::getIFps();
		}
	}

	// Smooth movement and rotation of the camera toward target values
	current_phi = lerp(current_phi, phi, interpolation_factor * Game::getIFps());
	current_theta = lerp(current_theta, theta, interpolation_factor * Game::getIFps());
	current_distance = lerp(current_distance, distance, interpolation_factor * Game::getIFps());
	current_camera_pivot_position = lerp(current_camera_pivot_position, camera_pivot_position, interpolation_factor * Game::getIFps());
}

// Applies interpolated camera position and orientation after all updates.
void TopDownController::postUpdate()
{
	// After all calculations, move camera to the computed position
	vec3 cameraViewDirection = quat(vec3_up, current_phi) * vec3_forward;
	cameraViewDirection = quat(cross(vec3_up, cameraViewDirection), -current_theta) * cameraViewDirection * -1;
	cameraViewDirection.normalize();

	camera->setViewDirection(cameraViewDirection);
	camera->setPosition(current_camera_pivot_position - Vec3(cameraViewDirection) * current_distance);
}

// Restores mouse handle mode and disables visualizer.
void TopDownController::shutdown()
{
	// Let the mouse be grabbed again
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	Visualizer::setEnabled(false);
}

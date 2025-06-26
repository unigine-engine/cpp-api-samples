#include "TopDownController.h"
#include "Selection.h"
#include <UnigineConsole.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(TopDownController);

using namespace Unigine;
using namespace Math;

// instantly places the camera to the specified position
void TopDownController::setPosition(const Unigine::Math::Vec3& pos)
{
	camera_pivot_position = pos;
	camera_pivot_position.z = 2.0f;
	current_camera_pivot_position = camera_pivot_position;
}

// sets the target position to which the camera approaches smoothly
void TopDownController::setTargetPosition(const Unigine::Math::Vec3& pos)
{
	camera_pivot_position = pos;
	camera_pivot_position.z = 2.0f;
}

// instantly sets the distance value for the camera
void TopDownController::setDistance(float dist)
{
	distance = dist;

	distance = clamp(distance, min_distance, max_distance);
	theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);

	current_distance = distance;
	current_theta = theta;
}

// sets the target distance to which the camera rolls or out smoothly
void TopDownController::setTargetDistance(float dist)
{
	distance = dist;

	distance = clamp(distance, min_distance, max_distance);
	theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);
}

// initialization
void TopDownController::init()
{
	// setting default values
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

	// setting up camera
	camera = checked_ptr_cast<Player>(node);
	if (!camera)
	{
		Log::error("TopDownController::init(): camera is not valid\n");
		return;
	}

	camera_pivot_position = node->getPosition();
	camera_pivot_position.z = 2.0f;
	current_camera_pivot_position = camera_pivot_position;

	// setting up input handle, mouse will not be grabbed
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	// setting camera to the start position
	vec3 cameraViewDirection = quat(vec3_up, current_phi) * vec3_forward;
	cameraViewDirection = quat(cross(vec3_up, cameraViewDirection), -current_theta) * cameraViewDirection * -1;
	cameraViewDirection.normalize();

	camera->setViewDirection(cameraViewDirection);
	camera->setWorldPosition(current_camera_pivot_position - Vec3(cameraViewDirection) * current_distance);

	// creating a logic node and assigning a component to it
	NodeDummyPtr logic = NodeDummy::create();
	selection = ComponentSystem::get()->addComponent<Selection>(logic);
}

void TopDownController::update()
{
	if (!camera)
		return;

	// input processing only when the Console is inactive
	if (!Console::isActive())
	{
		// when the middle mouse button is pressed, save initial vector for mouse_to_intersection_point_vector
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

		// process input for zoom
		int mouseAxis = Input::getMouseWheel();
		if (mouseAxis != 0)
		{
			distance = clamp(distance - mouseAxis, min_distance, max_distance);
			theta = clamp(theta - mouseAxis * degrees_per_unit, min_theta, max_theta);
		}

		// process input for rotation around the Z axis
		if (Input::isKeyPressed(Input::KEY_Q))
			phi -= 50.0f * Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_E))
			phi += 50.0f * Game::getIFps();

		// when key F is pressed and an object is selected, focus the camera selection
		if (Input::isKeyPressed(Input::KEY_F) && selection->hasSelection())
		{
			distance = selection->getBoundRadius();
			camera_pivot_position = selection->getCenter();
			camera_pivot_position.z = 2.0f;

			distance = clamp(distance, min_distance, max_distance);
			theta = clamp(min_theta + (distance - min_distance) * degrees_per_unit, min_theta, max_theta);
		}

		// if middle button is held down, update mouse_to_intersection_point_vector to move the camera
		if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_MIDDLE) && is_previous_hooked)
		{
			current_mouse_to_intersection_point_vector = Vec3(camera->getDirectionFromMainWindow(Input::getMousePosition().x, Input::getMousePosition().y));
			current_mouse_to_intersection_point_vector *= previous_mouse_to_intersection_point_vector.z / current_mouse_to_intersection_point_vector.z;

			Vec3 displacement = current_mouse_to_intersection_point_vector - previous_mouse_to_intersection_point_vector;

			camera_pivot_position -= displacement;
			current_camera_pivot_position -= displacement;
			previous_mouse_to_intersection_point_vector = current_mouse_to_intersection_point_vector;
		}
		// only when middle button is up, the player can control the camera by moving cursor to the edges of window
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

	// smooth movement and rotation of the camera to the required coordinates and angles
	current_phi = lerp(current_phi, phi, interpolation_factor * Game::getIFps());
	current_theta = lerp(current_theta, theta, interpolation_factor * Game::getIFps());
	current_distance = lerp(current_distance, distance, interpolation_factor * Game::getIFps());
	current_camera_pivot_position = lerp(current_camera_pivot_position, camera_pivot_position, interpolation_factor * Game::getIFps());
}

void TopDownController::postUpdate()
{
	// after all calculations we can move camera to the right position
	vec3 cameraViewDirection = quat(vec3_up, current_phi) * vec3_forward;
	cameraViewDirection = quat(cross(vec3_up, cameraViewDirection), -current_theta) * cameraViewDirection * -1;
	cameraViewDirection.normalize();

	camera->setViewDirection(cameraViewDirection);
	camera->setPosition(current_camera_pivot_position - Vec3(cameraViewDirection) * current_distance);
}

void TopDownController::shutdown()
{
	// letting the mouse to be grabbed
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	Visualizer::setEnabled(false);
}

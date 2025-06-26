#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

class Selection;

class TopDownController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TopDownController, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(postUpdate);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, phiProp, 180.0f, "Phi", "The angle by which the camera will be rotated around the Z axis at the beginning", "Angles");
	PROP_PARAM(Float, thetaProp, 40.0f, "Theta", "The angle by which the camera will be rotated around the Y axis at the beginning", "Angles", "min=0.0f;max=89.9f");

	PROP_PARAM(Vec2, thetaMinMax, Unigine::Math::vec2(10.0f, 70.0f), nullptr, "Lower and upper angle limits", "Angles");

	PROP_PARAM(Float, Distance, 25.0f, nullptr, "Distance at which the camera will be at the beginning", "Distance", "min=0.1f;max=300.0f");
	PROP_PARAM(Vec2, distanceMinMax, Unigine::Math::vec2(5.0f, 45.0f), nullptr, "Lower and upper distance limits", "Distance");

	PROP_PARAM(Float, zoomSpeed, 5.0f, nullptr, "The speed at which transformations will occur", "Speed", "min=0.5f;max=10.0f");

	void setPosition(const Unigine::Math::Vec3& pos);
	void setTargetPosition(const Unigine::Math::Vec3& pos);

	void setDistance(float dist);
	void setTargetDistance(float dist);

protected:
	void init();
	void update();
	void postUpdate();
	void shutdown();

private:

	Unigine::PlayerPtr camera;

	// information about intersection
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
	// vector from the camera to the intersection point of a line cast from the camera to the mouse position
	Unigine::Math::Vec3 previous_mouse_to_intersection_point_vector;
	// vector from the camera to the intersection point of a line cast from the camera to the mouse position, after moving the mouse
	Unigine::Math::Vec3 current_mouse_to_intersection_point_vector;
	bool is_previous_hooked = false;

	float current_phi = 180.0f;
	float phi = 180.0f;

	float current_theta = 40.0f;
	float theta = 40.0f;
	float max_theta = 70.0f;
	float min_theta = 10.0f;

	float current_distance = 25.0f;
	float distance = 25.0f;
	float max_distance = 45.0f;
	float min_distance = 5.0f;

	float interpolation_factor = 1.0f;

	// the camera moves relative to the pivot
	Unigine::Math::Vec3 current_camera_pivot_position;
	Unigine::Math::Vec3 camera_pivot_position;

	// how many degrees to rotate when shifting by one unit
	float degrees_per_unit = 1.0f;

	Selection *selection = nullptr;
};
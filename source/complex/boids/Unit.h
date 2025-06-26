#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineNode.h>
#include <UniginePhysics.h>

class Flock;

class Unit
{
public:
	explicit Unit(const Unigine::NodePtr &node);

	void update(Flock *flock);
	UNIGINE_INLINE const Unigine::NodePtr &getNode() const { return unit_node; }

	UNIGINE_INLINE void setMaxSpeed(float speed) { max_speed = speed; }
	UNIGINE_INLINE float getMaxSpeed() const { return max_speed; }

	UNIGINE_INLINE void setRotationSpeed(float speed) { rotation_speed = speed; }
	UNIGINE_INLINE float getRotationSpeed() const { return rotation_speed; }

	UNIGINE_INLINE void setMaxForce(float force) { max_force = force; }
	UNIGINE_INLINE float getMaxForce() const { return max_force; }

	UNIGINE_INLINE float getDesiredSeparationRange() const { return desired_separation_range; }
	UNIGINE_INLINE void setDesiredSeparationRange(float range) { desired_separation_range = range; }

	UNIGINE_INLINE float getSpotRadius() const { return spot_radius; }
	UNIGINE_INLINE void setSpotRadius(float radius) { spot_radius = radius; }

	UNIGINE_INLINE const Unigine::Math::Vec3 &getCurrentVelocity() const { return velocity; }
	UNIGINE_INLINE const Unigine::Math::Vec3 &getCurrentAcceleration() const
	{
		return acceleration;
	}

	UNIGINE_INLINE void setLimitByHeightEnabled(bool enabled) { is_limit_by_height = enabled; }
	UNIGINE_INLINE bool getLimitByHeightEnabled() const { return is_limit_by_height; }

	UNIGINE_INLINE void setMinHeight(Unigine::Math::Scalar height) { min_height = height; }
	UNIGINE_INLINE Unigine::Math::Scalar getMinHeight() const { return min_height; }

	UNIGINE_INLINE void setMaxHeight(Unigine::Math::Scalar height) { max_height = height; }
	UNIGINE_INLINE Unigine::Math::Scalar getMaxHeight() const { return max_height; }

	UNIGINE_INLINE void setHeightPadding(Unigine::Math::Scalar padding)
	{
		height_padding = padding;
	}
	UNIGINE_INLINE Unigine::Math::Scalar getHeightPadding() const { return height_padding; }

private:
	struct Data
	{
		int count{0};
		Unigine::Math::Vec3 sum{};
	};

private:
	void flocking(Flock *flock);
	void limit_by_height();
	void debug_visualize() const;
	void limit_rotation();
	void apply_movement();

	void separation_loop(Data &separation_data, Unit &unit) const;
	void align_loop(Data &align_data, Unit &unit);
	void group_up_loop(Data &group_data, Unit &unit);
	void separation_total(Flock *flock, Data &separation_data);
	void align_total(Flock *flock, Data &align_data);
	void group_up_total(Flock *flock, Data &group_data);

	Unigine::Math::Vec3 chase_target(const Flock *flock) const;

private:
	Unigine::NodePtr unit_node;
	Unigine::BodyRigidPtr unit_body;

	Unigine::Math::Scalar max_speed{1.f};
	Unigine::Math::Scalar max_force{1.f};

	float desired_separation_range{1.f};
	float spot_radius{1.f};

	Unigine::Math::Vec3 acceleration{};
	Unigine::Math::Vec3 velocity{};

	float rotation_speed{50.f};

	bool is_limit_by_height{false};
	Unigine::Math::Scalar min_height{0.f};
	Unigine::Math::Scalar max_height{0.f};
	Unigine::Math::Scalar height_padding{0.f};
};
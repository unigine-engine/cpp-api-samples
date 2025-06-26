#pragma once
#include <UnigineObjects.h>
#include <UnigineComponentSystem.h>
#include <UnigineDecals.h>

class ExtinguishingPuddles: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExtinguishingPuddles, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_POST_UPDATE(post_update)

	PROP_PARAM(File, decal_ref)
	PROP_PARAM(Node, spark_emitter_node)
	PROP_PARAM(Float, spark_linear_damping, 0.1f)
	PROP_PARAM(Float, life_time, 5000.0f)
	PROP_PARAM(Float, vertical_speed, 1.0f)
	PROP_PARAM(Float, min_spawn_angle, 0.0f)
	PROP_PARAM(Float, max_spawn_angle, 180.0f)
	PROP_PARAM(Float, min_spawn_scale, 1.0f)
	PROP_PARAM(Float, max_spawn_scale, 1.0f)
	PROP_PARAM(Float, grow_speed, 0.1f)
	PROP_PARAM(Float, grow_damping, 1.0f)
	PROP_PARAM(Float, ground_speed, 0.0f)
	PROP_PARAM(Float, ground_speed_spread, 0.0f)
	PROP_PARAM(Float, ground_damping, 1.0f)
	PROP_PARAM(Float, ground_damping_spread, 1.0f)
	PROP_PARAM(Int, max_count, 15)
	PROP_PARAM(Float, dead_time, 5.0f)
	
protected:
	void init();
	void update();
	void post_update();
	void shutdown();

	struct DecalTraceData
	{
		Unigine::NodePtr node;
		Unigine::MaterialPtr material;
		float life{};
		float life_time = 2.0f;
		float scale_velocity{};
		float ground_velocity{};
		float ground_damping{};
		Unigine::Math::vec3 gravity_drag;
		Unigine::Math::vec3 ground_drag;

		void clear() { node.deleteLater(); }
	};

	void update_decal_traces();

	float cos_min_spawn_angle = 0.0f;
	float cos_max_spawn_angle = 0.0f;

	Unigine::ObjectParticlesPtr emitter;
	Unigine::DecalPtr decal_prefab;
	Unigine::ObjectParticlesPtr spark_emitter;

	Unigine::Vector<DecalTraceData> decal_traces;
	int decal_albedo_id = -1;
};

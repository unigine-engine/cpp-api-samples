#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineDecals.h>

class BoatFoam : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BoatFoam, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, decal_foam, "Decal Foam", "", "", "filter=DecalOrtho");
	PROP_PARAM(Float, decal_padding, 50.0f);
	PROP_PARAM(Float, max_ship_velocity_for_spawn_scale, 10.0f);
	PROP_PARAM(Float, max_ship_velocity_for_life_scale, 10.0f);
	PROP_PARAM(Float, max_ship_velocity_for_velocity_scale, 10.0f);

private:
	void init();
	void update();
	void shutdown();

	void update_foam_decal_size();
	void update_particles_params();
	struct FoamParticles
	{
		Unigine::ObjectParticlesPtr particles;
		float life_mean = 0.0f;
		float life_spread = 0.0f;
		float spaw_rate = 0;
		float velocity_min = 0.0f;
		float velocity_max = 0.0f;
	};

	float velocity_scalar = 0.0f;
	Unigine::Vector<FoamParticles> foam_particles;
	Unigine::DecalOrthoPtr foam;

	Unigine::Math::Vec3 last_position;

};
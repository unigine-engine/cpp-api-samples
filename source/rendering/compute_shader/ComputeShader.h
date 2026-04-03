// GPU particle system using compute shaders. 1M particles are simulated on
// GPU with position/velocity stored in textures. Particles morph between
// target mesh shapes using UV-mapped world positions and ping-pong buffers.

#pragma once

#include <UnigineComponentSystem.h>

// GPU-accelerated particle simulation with mesh morphing targets.
class ComputeShader : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ComputeShader, ComponentBase)

	// Material with compute shader for particle physics simulation
	PROP_PARAM(Material, particles_solver_material)
	// Material to convert mesh UV to world positions
	PROP_PARAM(Material, uv_to_position_material)
	// Material for particle rendering (point sprites)
	PROP_PARAM(Material, gpu_particles_material)

	// Mesh assets that particles will morph between
	PROP_ARRAY(File, meshes)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

	void create_resources();		// Creates GPU textures if not yet allocated
	void init_particles_data();		// Fills initial random positions into textures
	void destroy_resources();		// Releases GPU resources on shutdown

private:
	float time{0.f};
	bool particles_initialized{false};

	// ObjectDynamic renders particles as point primitives
	Unigine::ObjectDynamicPtr dynamic;

	// Meshes used as morph targets for particle positions
	Unigine::Vector<Unigine::ObjectMeshStaticPtr> static_meshes;

	// Ping-pong buffers: [0] = write target, [1] = read source
	Unigine::VectorStack<Unigine::TexturePtr, 2> velocity_texture;
	Unigine::VectorStack<Unigine::TexturePtr, 2> position_texture;

	// Stores world-space positions from mesh UV mapping
	Unigine::TexturePtr uv_position_texture;

	Unigine::RenderTargetPtr render_target;
};

#pragma once

#include <UnigineComponentSystem.h>

/// В этом семле можно посмотреть,
/// как можно запустить compute shader,
/// записать данные в текстуру и потом использовать эту текстуру
class ComputeShader : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ComputeShader, ComponentBase)

	PROP_PARAM(Material, particles_solver_material)
	PROP_PARAM(Material, uv_to_position_material)
	PROP_PARAM(Material, gpu_particles_material)

	PROP_ARRAY(File, meshes)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

	void create_resources();
	void init_particles_data();
	void destroy_resources();

private:
	float time{0.f};
	bool particles_initialized{false};

	// Dynamic object for particles visualization
	Unigine::ObjectDynamicPtr dynamic;

	Unigine::Vector<Unigine::ObjectMeshStaticPtr> static_meshes;

	Unigine::VectorStack<Unigine::TexturePtr, 2> velocity_texture;
	Unigine::VectorStack<Unigine::TexturePtr, 2> position_texture;

	Unigine::TexturePtr uv_position_texture;

	Unigine::RenderTargetPtr render_target;
};

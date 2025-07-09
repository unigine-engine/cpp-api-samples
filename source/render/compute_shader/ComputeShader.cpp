#include "ComputeShader.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ComputeShader)

using namespace Unigine;
using namespace Math;

int PARTICLES_SIZE_X = 1024;
int PARTICLES_SIZE_Y = 1024;

void ComputeShader::init()
{
	Render::setMotionBlur(false);
	Render::setScreenSpaceEffects(false);

	dynamic = ObjectDynamic::create();

	dynamic->setInstancing(false);
	dynamic->addSurface("particles");

	// set primitive topology to points
	dynamic->setSurfaceMode(ObjectDynamic::MODE_POINTS, 0);
	dynamic->setSurfaceBegin(0, 0);
	dynamic->setSurfaceEnd(PARTICLES_SIZE_X * PARTICLES_SIZE_Y, 0);
	dynamic->setTransform(translate(Vec3(0.f, 0.f, 0.f)));
	dynamic->setMaterial(gpu_particles_material, "*");

	const VectorStack<ObjectDynamic::Attribute, 1> attributes = {
		{0, ObjectDynamic::TYPE_FLOAT, 1}
	};

	dynamic->setVertexFormat(attributes.get(), attributes.size());
	dynamic->clearVertex();
	dynamic->clearIndices();

	dynamic->addPoints(PARTICLES_SIZE_X * PARTICLES_SIZE_Y);

	for (int i = 0; i < PARTICLES_SIZE_X * PARTICLES_SIZE_Y; ++i)
	{
		float index = static_cast<float>(i);

		// Write particle ID (also for handle particles we can use VERTEX_ID in shaders w/o any vertex/index buffer)
		dynamic->addVertexFloat(0, &index, 1);
	}

	dynamic->flushVertex();
	dynamic->flushIndices();
	dynamic->setBoundBox(BoundBox(-vec3_inf, vec3_inf));

	for (int i = 0; i < meshes.size(); ++i)
	{
		const auto& mesh_path = meshes[i];
		ObjectMeshStaticPtr object = ObjectMeshStatic::create(mesh_path);
		object->setTransform(translate(Vec3(0.f, 0.f, 0.f)));
		object->setMaterial(uv_to_position_material, "*");
		object->setEnabled(false);
		static_meshes.push_back(std::move(object));
	}
}

void ComputeShader::update()
{
	// create resources if invalid
	create_resources();

	float ifps = Game::getIFps();

	int object_index = static_cast<int>(time / 6.f) % static_meshes.size() + 1;

	// Render uv mapped position for object
	if (object_index < static_meshes.size())
	{
		static_meshes[object_index]->setEnabled(true);
	}

	render_target->bindColorTexture(0, uv_position_texture);
	render_target->enable();
	RenderState::clearBuffer(RenderState::BUFFER_COLOR, vec4_zero);
	if (object_index < static_meshes.size())
	{
		static_meshes[object_index]->render(Render::PASS_AMBIENT, 0);
	}

	render_target->disable();
	render_target->unbindColorTextures();

	if (object_index < static_meshes.size())
	{
		static_meshes[object_index]->setEnabled(false);
	}

	// Send ifps to compute material so we can use it in our shader code
	int id = particles_solver_material->findParameter("ifps");
	particles_solver_material->setParameterFloat(id, ifps);

	time += ifps;


	// move attractor by some trajectory
	float attractor_sin = 0.f;
	float attractor_cos = 0.f;
	Math::sincos(time * 0.5f, attractor_sin, attractor_cos);

	vec4 attractor = vec4(attractor_cos, attractor_cos * attractor_sin, attractor_sin, 0.f);
	attractor *= 5.f;

	// set attractor to the material so we can use it in our shader code
	id = particles_solver_material->findParameter("attractor");
	particles_solver_material->setParameterFloat4(id, attractor);


	RenderState::setTexture(RenderState::BIND_ALL, 0, position_texture[1]);
	RenderState::setTexture(RenderState::BIND_ALL, 1, velocity_texture[1]);
	RenderState::setTexture(RenderState::BIND_ALL, 2, uv_position_texture);

	render_target->bindUnorderedAccessTexture(0, position_texture[0], true);
	render_target->bindUnorderedAccessTexture(1, velocity_texture[0], true);

	// Dispatch 32x32 (1024) group threads (each group has 32x32 (1024) threads, this value defines
	// in compute shader file)
	int group_threads_x = PARTICLES_SIZE_X / 32;
	int group_threads_y = PARTICLES_SIZE_Y / 32;

	// if incomplete group exist
	if (PARTICLES_SIZE_X % 32 > 0)
		group_threads_x++;

	if (PARTICLES_SIZE_Y % 32 > 0)
		group_threads_y++;

	render_target->enable();
	particles_solver_material->renderCompute(Render::PASS_POST, group_threads_x, group_threads_y);
	render_target->disable();
	render_target->unbindUnorderedAccessTextures();

	// Swap ping-pong textures
	TexturePtr swap = position_texture[0];
	position_texture[0] = position_texture[1];
	position_texture[1] = swap;

	swap = velocity_texture[0];
	velocity_texture[0] = velocity_texture[1];
	velocity_texture[1] = swap;

	// Set textures for ObjectDynamic (particles renderer)
	MaterialPtr material = dynamic->getMaterial(0);

	if (position_texture[1])
	{
		id = material->findTexture("particles_positions");
		material->setTexture(id, position_texture[1]);
	}

	if (velocity_texture[1])
	{
		id = material->findTexture("particles_velocities");
		material->setTexture(id, velocity_texture[1]);
	}

	if (uv_position_texture)
	{
		id = material->findTexture("particles_mapped_positions");
		material->setTexture(id, uv_position_texture);
	}
}

void ComputeShader::create_resources()
{
	const int flags = Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::FORMAT_USAGE_RENDER;

	// Init velocity texture
	for (auto &texture : velocity_texture)
	{
		if (texture == nullptr)
		{
			texture = Texture::create();
			texture->create2D(PARTICLES_SIZE_X, PARTICLES_SIZE_Y, Texture::FORMAT_RGBA32F, flags);
		}
	}

	for (auto &texture : position_texture)
	{
		if (texture == nullptr)
		{
			texture = Texture::create();
			texture->create2D(PARTICLES_SIZE_X, PARTICLES_SIZE_Y, Texture::FORMAT_RGBA32F, flags);
		}
	}

	if (!render_target)
	{
		render_target = RenderTarget::create();
	}

	if (!uv_position_texture)
	{
		uv_position_texture = Texture::create();
		uv_position_texture->create2D(PARTICLES_SIZE_X, PARTICLES_SIZE_Y, Texture::FORMAT_RGBA32F,
			flags);
	}

	if (!particles_initialized)
	{
		init_particles_data();
	}
}

void ComputeShader::init_particles_data()
{
	ImagePtr initial_position = Image::create();
	initial_position->create2D(PARTICLES_SIZE_X, PARTICLES_SIZE_Y, Image::FORMAT_RGBA32F);

	position_texture.resize(2);
	velocity_texture.resize(2);

	for (int i = 0; i < PARTICLES_SIZE_X; i++)
	{
		for (int j = 0; j < PARTICLES_SIZE_Y; j++)
		{
			// initial data for particles simulation
			vec3 position = vec3_zero;
			position.x = 10.f * (Game::getRandomFloat(-10.f, 10.f));
			position.y = 10.f * (Game::getRandomFloat(-10.f, 10.f));
			position.z = 10.f * (Game::getRandomFloat(-10.f, 10.f));

			initial_position->set2D(i, j, Image::Pixel(position.x, position.y, position.z, 1.f));
		}
	}

	if (position_texture[1])
	{
		position_texture[1]->setImage(initial_position);
	}

	initial_position.clear();

	for (int i = 0; i < 2; i++)
	{
		render_target->bindColorTexture(0, velocity_texture[i]);
		render_target->enable();
		RenderState::setBufferMask(0, RenderState::BUFFER_ALL);
		RenderState::clearBuffer(RenderState::BUFFER_ALL, vec4_zero, 0.f);
		render_target->disable();
		render_target->unbindColorTextures();
	}

	particles_initialized = true;
}

void ComputeShader::destroy_resources()
{
	for (int i = 0; i < 2; ++i)
	{
		if (position_texture[i])
		{
			position_texture[i]->destroy();
			position_texture[i].clear();
		}

		if (velocity_texture[i])
		{
			velocity_texture[i]->destroy();
			velocity_texture[i].clear();
		}
	}

	uv_position_texture->destroy();
	uv_position_texture.clear();

	if (render_target)
	{
		render_target->destroy();
		render_target.clear();
	}

	particles_initialized = false;
}

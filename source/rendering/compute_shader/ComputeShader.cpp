// GPU particle system using compute shaders. 1M particles are simulated on
// GPU with position/velocity stored in textures. Particles morph between
// target mesh shapes using UV-mapped world positions. Uses ping-pong buffers
// for read/write separation during compute dispatch.

#include "ComputeShader.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ComputeShader)

using namespace Unigine;
using namespace Math;

int PARTICLES_SIZE_X = 1024;	// 1024x1024 = 1M particles
int PARTICLES_SIZE_Y = 1024;

// ObjectDynamic with point primitives is created; meshes are loaded as morph targets.
void ComputeShader::init()
{
	Render::setMotionBlur(false);
	Render::setScreenSpaceEffects(false);

	dynamic = ObjectDynamic::create();

	dynamic->setInstancing(false);
	dynamic->addSurface("particles");

	// Points are rendered; actual positions come from texture lookup in vertex shader
	dynamic->setSurfaceMode(ObjectDynamic::MODE_POINTS, 0);
	dynamic->setSurfaceBegin(0, 0);
	dynamic->setSurfaceEnd(PARTICLES_SIZE_X * PARTICLES_SIZE_Y, 0);
	dynamic->setTransform(translate(Vec3(0.f, 0.f, 0.f)));
	dynamic->setMaterial(gpu_particles_material, "*");

	// Single float attribute stores particle index for texture UV calculation
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
		// Particle ID used as texture coordinate to fetch position from compute output
		dynamic->addVertexFloat(0, &index, 1);
	}

	dynamic->flushVertex();
	dynamic->flushIndices();
	// Infinite bounds since particles can move anywhere
	dynamic->setBoundBox(BoundBox(-vec3_inf, vec3_inf));

	// Morph target meshes are loaded but kept disabled until needed
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

// Compute shader dispatches particle physics; morph target cycles every 6 seconds.
void ComputeShader::update()
{
	// Create resources if data is invalid
	create_resources();

	float ifps = Game::getIFps();

	if (static_meshes.empty())
		return;

	// Morph target changes every 6 seconds
	int object_index = static_cast<int>(time / 6.f) % static_meshes.size() + 1;

	// Current mesh is rendered to texture, outputting world positions via UV coordinates
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

	// Send IFps to compute material so we can use it in our shader code
	int id = particles_solver_material->findParameter("ifps");
	particles_solver_material->setParameterFloat(id, ifps);

	time += ifps;

	// Attractor moves in a Lissajous-like pattern to create interesting motion
	float attractor_sin = 0.f;
	float attractor_cos = 0.f;
	Math::sincos(time * 0.5f, attractor_sin, attractor_cos);

	vec4 attractor = vec4(attractor_cos, attractor_cos * attractor_sin, attractor_sin, 0.f);
	attractor *= 5.f;

	// Set attractor to the material so we can use it in our shader code
	id = particles_solver_material->findParameter("attractor");
	particles_solver_material->setParameterFloat4(id, attractor);


	// Read textures (previous frame's output)
	RenderState::setTexture(RenderState::BIND_ALL, 0, position_texture[1]);
	RenderState::setTexture(RenderState::BIND_ALL, 1, velocity_texture[1]);
	RenderState::setTexture(RenderState::BIND_ALL, 2, uv_position_texture);

	// Write textures (UAV for compute shader output)
	render_target->bindUnorderedAccessTexture(0, position_texture[0], true);
	render_target->bindUnorderedAccessTexture(1, velocity_texture[0], true);

	// Thread group count: shader uses 32x32 threads per group
	int group_threads_x = PARTICLES_SIZE_X / 32;
	int group_threads_y = PARTICLES_SIZE_Y / 32;

	// In case if incomplete group exist
	if (PARTICLES_SIZE_X % 32 > 0)
		group_threads_x++;

	if (PARTICLES_SIZE_Y % 32 > 0)
		group_threads_y++;

	render_target->enable();
	particles_solver_material->renderCompute(Render::PASS_POST, group_threads_x, group_threads_y);
	render_target->disable();
	render_target->unbindUnorderedAccessTextures();

	// Ping-pong swap: next frame reads what this frame wrote
	TexturePtr swap = position_texture[0];
	position_texture[0] = position_texture[1];
	position_texture[1] = swap;

	swap = velocity_texture[0];
	velocity_texture[0] = velocity_texture[1];
	velocity_texture[1] = swap;

	// Updated textures are bound to particle rendering material
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

// RGBA32F textures are created with UAV access for compute shader read/write.
void ComputeShader::create_resources()
{
	const int flags = Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::FORMAT_USAGE_RENDER;

	// Initial velocity texture
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

// Particles start at random positions within 100 units; velocities are zeroed.
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
			// Initial data for particles simulation
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

	// Velocity textures are cleared to zero
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

// Releases all GPU textures and render target.
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

// Renders a mesh silhouette to a texture using either Renderer.RenderMesh or
// MeshRender.Render. Demonstrates two approaches: using Camera-based rendering
// and manual shader parameter setup with orthographic projection.

#include "MeshToMaskTextureSample.h"

#include <UnigineGame.h>
#include <UnigineImport.h>
#include <UnigineTextures.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(MeshToMaskTextureSample);

using namespace Unigine;
using namespace Math;

// Mesh, texture, and manipulator are set up; display plane receives the output texture.
void MeshToMaskTextureSample::init()
{
	if (!view_point.get())
	{
		Log::error("MeshToMaskTextureSample::init The camera point is unspecified");
		return;
	}

	if (!material.get())
	{
		Log::error("MeshToMaskTextureSample::init The render material is unspecified");
		return;
	}

	auto object_mesh_static_node = checked_ptr_cast<ObjectMeshStatic>(mesh_node.get());
	if (!object_mesh_static_node)
	{
		Log::error("MeshToMaskTextureSample::init Mesh to render node is unspecified");
		return;
	}

	player = checked_ptr_cast<PlayerDummy>(player_node.get());
	if (!player)
	{
		Log::error("MeshToMaskTextureSample::init Player node is unspecified");
		return;
	}

	mesh_to_render = Mesh::create(object_mesh_static_node->getMeshPath());
	if (!mesh_to_render)
	{
		Log::error("MeshToMaskTextureSample::init Mesh for rendering is invalid ");
		return;
	}

	init_gui();

	// Clearing texture for future rendering
	texture = Texture::create();
	texture->create2D(256, 256, Texture::FORMAT_RGBA8,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
	texture->clearBuffer(vec4_black);

	// Set up display plane
	auto obj = checked_ptr_cast<ObjectMeshStatic>(object_view.get());
	auto mat = obj->getMaterial(0);
	mat = mat->inherit();
	mat->setTexture(mat->findTexture("albedo"), texture);
	mat->setParameterFloat4("uv_transform", vec4(-1.f, Render::isFlipped() ? -1.f : 1.f, 0, 0));
	obj->setMaterial(mat, 0);

	// Set up manipulator
	auto gui = Gui::getCurrent();
	manipulator = WidgetManipulatorTranslator::create(gui);
	manipulator->setTransform(view_point.get()->getWorldTransform());
	gui->addChild(manipulator);
	manipulator->getEventChanged().connect(ev_conns, [this]() {
		view_point->setWorldPosition(manipulator->getTransform().getTranslate());
		player->setWorldTransform(manipulator->getTransform());
		player->worldLookAt(mesh_node->getWorldPosition());
	});

	Visualizer::setEnabled(true);
}

// Dropdown menu is created to switch between rendering modes.
void MeshToMaskTextureSample::init_gui()
{
	window.createWindow();
	WidgetGroupBoxPtr params = window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	params->addChild(hbox, Gui::ALIGN_LEFT);
	auto evening_label = WidgetLabel::create("Rendering class:");
	hbox->addChild(evening_label, Gui::ALIGN_LEFT);
	WidgetComboBoxPtr mode = WidgetComboBox::create();
	hbox->addChild(mode, Gui::ALIGN_LEFT);
	mode->addItem("Renderer.RenderMesh");
	mode->addItem("MeshRender.Render");
	mode->getEventChanged().connect(*this, [this, mode]() {
		int item = mode->getCurrentItem();
		if (item)
		{
			is_manual = true;
		}
		else
		{
			is_manual = false;
		}
	});
}

// Manipulator is synchronized with camera; mesh is rendered based on selected mode.
void MeshToMaskTextureSample::update()
{
	auto p = Game::getPlayer();
	manipulator->setProjection(p->getProjection());
	manipulator->setModelview(p->getCamera()->getModelview());
	if (is_manual)
	{
		RenderData data = RenderData();
		data.mesh_transform = mesh_node->getWorldTransform();
		data.camera_position = view_point->getWorldPosition();
		data.material = material;
		data.pass_name = "mesh";
		data.camera_size = ivec2(10, 10);
		auto halfSize = 10 * 0.5f;

		auto projection = Math::ortho(-halfSize, halfSize, -halfSize, halfSize, data.zNear,
			data.zFar);
		auto camera_transform = Math::setTo(Vec3(data.camera_position),
			data.mesh_transform.getTranslate(), vec3_up, Math::AXIS_NZ);
		Unigine::Visualizer::renderFrustum(projection, camera_transform, Unigine::Math::vec4_red);

		renderMesh(mesh_to_render, texture, data);
	}
	else
	{
		Unigine::Visualizer::renderFrustum(player->getProjection(), player->getWorldTransform(),
			Unigine::Math::vec4_red);
		renderMesh(mesh_to_render, texture, material, "mesh", mesh_node->getWorldTransform(),
			player->getCamera());
	}
}

// Manipulator widget and UI window are cleaned up.
void MeshToMaskTextureSample::shutdown()
{
	manipulator.deleteLater();
	window.shutdown();
}

// Manual mode: orthographic projection and shader transforms are calculated explicitly.
void MeshToMaskTextureSample::renderMesh(const Unigine::MeshPtr mesh, Unigine::TexturePtr texture,
	const RenderData &data)
{
	if (!texture)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: texture in nullptr \n");
		return;
	}
	if (!mesh)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: mesh in nullptr \n");
		return;
	}
	if (!data.material)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: material is nullptr\n");
		return;
	}
	if (!data.pass_name)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: pass name is nullptr\n");
		return;
	}
	texture->clearBuffer(vec4_black);

	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(mesh);

	Mat4 mesh_transform = data.mesh_transform;
	double half_width = data.camera_size.x * 0.5f;
	double half_height = data.camera_size.y * 0.5f;


	mat4 projection = Math::ortho(
		-half_width, half_width,
		-half_height, half_height,
		data.zNear, data.zFar
	);
	Mat4 camera_transform = Math::setTo(Vec3(data.camera_position), mesh_transform.getTranslate(),
		vec3_up, Math::AXIS_NZ);

	auto renderTarget = Render::getTemporaryRenderTarget();
	// Setting up default RenderState and blending
	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setBlendFunc(RenderState::BLEND_NONE, RenderState::BLEND_NONE);
	RenderState::clearBuffer(RenderState::BUFFER_COLOR, vec4_black);

	// Binding color texture to draw on
	renderTarget->bindColorTexture(0, texture);
	renderTarget->enable();

	Renderer::setModelview(inverse((Mat4)camera_transform));
	Renderer::setOldModelview(inverse((Mat4)camera_transform));
	Renderer::setProjection(projection);
	Renderer::setOldProjection(projection);

	// Converting world space to view space
	vec4 transforms[3];
	Mat4 local_transform = Mat4(mesh_transform);
	Mat4 *transform_array = &local_transform;
	Simd::mulMat4Mat4(transforms, inverse(camera_transform), (const Mat4 **)&transform_array, 1);

	ShaderPtr shader = data.material->getShaderForce(data.pass_name);

	// Using default vertex shader and handwritten fragment shader. Check out
	// simple_material.basemat setting up required transform data for vertex shader.
	shader->setParameterArrayFloat4("s_transform", transforms, 3);
	// You could set any shader parameter you want


	// Must be after shader parameter setting so that render uses relevant values
	auto pass = data.material->getRenderPass(data.pass_name);
	Renderer::setMaterial(pass, data.material);
	Renderer::setShaderParameters(pass, data.material, 0);

	meshRender->render(MeshRender::MODE_TRIANGLES);

	renderTarget->disable();
	renderTarget->unbindAll();
	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(renderTarget);
}

// Camera mode: Renderer.RenderMesh handles projection and transforms automatically.
void MeshToMaskTextureSample::renderMesh(const Unigine::MeshPtr mesh, Unigine::TexturePtr texture,
	const Unigine::MaterialPtr material, const char *pass_name,
	const Unigine::Math::Mat4 mesh_transform, const Unigine::CameraPtr &camera)
{
	if (!texture)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: texture in nullptr \n");
		return;
	}
	if (!mesh)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: mesh in nullptr \n");
		return;
	}
	if (!material)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: material is nullptr\n");
		return;
	}
	if (!pass_name)
	{
		Log::error("MeshToMaskTextureSample::renderMesh: pass name is nullptr\n");
		return;
	}
	texture->clearBuffer(vec4_black);

	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(mesh);

	auto renderTarget = Render::getTemporaryRenderTarget();
	// Setting up default RenderState and blending
	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setBlendFunc(RenderState::BLEND_NONE, RenderState::BLEND_NONE);
	RenderState::clearBuffer(RenderState::BUFFER_COLOR, vec4_black);

	// Binding color texture to draw on
	renderTarget->bindColorTexture(0, texture);
	renderTarget->enable();
	Renderer::renderMesh(meshRender, material, pass_name, mesh_transform, camera);

	renderTarget->disable();
	renderTarget->unbindAll();
	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(renderTarget);
}

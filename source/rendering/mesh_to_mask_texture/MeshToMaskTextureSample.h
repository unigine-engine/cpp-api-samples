// Renders mesh silhouette to a texture for masking or decal purposes.
// Demonstrates two rendering approaches: Renderer.RenderMesh (camera-based)
// and MeshRender.Render (manual shader parameter setup).

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

// Renders a mesh to texture using either high-level or low-level API.
class MeshToMaskTextureSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MeshToMaskTextureSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Source mesh to render as silhouette
	PROP_PARAM(Node, mesh_node);
	// Display plane showing the rendered texture
	PROP_PARAM(Node, object_view, "", "", "", "filter=Object");
	// Camera position for rendering (moveable via manipulator)
	PROP_PARAM(Node, view_point);
	// Player camera used in Renderer.RenderMesh mode
	PROP_PARAM(Node, player_node);
	// Material with custom shader for silhouette rendering
	PROP_PARAM(Material, material);

private:
	void init();
	void init_gui();
	void update();
	void shutdown();

	// Parameters for manual rendering mode (MeshRender.Render)
	struct RenderData
	{
		// Mesh world transform
		Unigine::Math::Mat4 mesh_transform = Unigine::Math::Mat4_identity;
		// Camera position for orthographic projection
		Unigine::Math::Vec3 camera_position = Unigine::Math::Vec3_zero;
		// Orthographic camera bounds in world units
		Unigine::Math::ivec2 camera_size = Unigine::Math::ivec2(10, 10);
		// Near/far clip planes
		float zNear = 0.01f;
		float zFar = 25.0f;
		// Rendering material and pass
		Unigine::MaterialPtr material = nullptr;
		const char *pass_name;
	};

	// Manual mode: projection and shader parameters are set up explicitly
	void renderMesh(const Unigine::MeshPtr mesh, Unigine::TexturePtr texture,
		const RenderData &data);
	// Camera mode: uses Renderer.RenderMesh for simpler setup
	void renderMesh(const Unigine::MeshPtr mesh, Unigine::TexturePtr texture,
		const Unigine::MaterialPtr material, const char *pass_name,
		const Unigine::Math::Mat4 mesh_transform, const Unigine::CameraPtr &camera);

private:
	// Loaded mesh data for rendering
	Unigine::MeshPtr mesh_to_render;
	// Output texture displayed on the view plane
	Unigine::TexturePtr texture;
	// Camera for Renderer.RenderMesh mode
	Unigine::PlayerDummyPtr player;
	Unigine::EventConnections ev_conns;
	// 3D gizmo for moving the camera position
	Unigine::WidgetManipulatorTranslatorPtr manipulator;
	SampleDescriptionWindow window;
	// True = manual mode (MeshRender.Render), False = camera mode (Renderer.RenderMesh)
	bool is_manual = false;
};

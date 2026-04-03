// Renders an object's depth buffer from an orthographic top-down view. The depth
// texture is used by the excavation system to determine how deep the object cuts
// into the terrain at each texel position.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include <UnigineViewport.h>

class ObjectDepth
{
public:
	ObjectDepth();
	// Non-copyable: viewport and render target resources are unique
	ObjectDepth(const ObjectDepth&) = delete;
	ObjectDepth& operator=(const ObjectDepth&) = delete;

	// Renders depth of the target node from above into the provided texture
	// pos/size define the world-space bounding box, resolution is texture size
	void renderDepthTexture(
		Unigine::TexturePtr &tex,
		const Unigine::NodePtr &target,
		const Unigine::Math::Vec3 &pos,
		const Unigine::Math::Vec3 &size,
		const Unigine::Math::ivec2 &resolution
	);

private:
	// Callback invoked at end of viewport render pass
	void on_end_screen();

private:
	Unigine::RenderTargetPtr render_target;
	// Dedicated viewport with depth-only rendering mode
	Unigine::ViewportPtr viewport;
	// Orthographic camera looking down at the object
	Unigine::CameraPtr camera;

	// Bounding box height for depth normalization in shader
	float bb_height{ 0.0f };
	// Minimum Z of bounding box for world-space reconstruction
	float bb_min_z{0.0f};

	Unigine::EventConnection on_end_screen_connection;
	// Material containing the depth extraction shader
	Unigine::MaterialPtr digging_tool_mat;
};

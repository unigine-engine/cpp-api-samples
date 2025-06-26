#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include <UnigineViewport.h>

class ObjectDepth
{
public:
	ObjectDepth();
	ObjectDepth(const ObjectDepth&) = delete;
	ObjectDepth& operator=(const ObjectDepth&) = delete;

	void renderDepthTexture(
		Unigine::TexturePtr &tex,
		const Unigine::NodePtr &target,
		const Unigine::Math::Vec3 &pos,
		const Unigine::Math::Vec3 &size,
		const Unigine::Math::ivec2 &resolution
	);

private:
	void on_end_screen();

private:
	Unigine::RenderTargetPtr render_target;
	Unigine::ViewportPtr viewport;
	Unigine::CameraPtr camera;

	float bb_height{ 0.0f };
	float bb_min_z{0.0f};

	Unigine::EventConnection on_end_screen_connection;
	Unigine::MaterialPtr digging_tool_mat;
};

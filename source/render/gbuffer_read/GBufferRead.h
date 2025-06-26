#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>

class GBufferRead final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferRead, ComponentBase)

	PROP_PARAM(Material, fetch_buffers_post_material)

	COMPONENT_INIT(init)

	enum class Mode
	{
		RenderNode,
		RenderMainViewport
	};

	void renderNode(const Unigine::Math::Mat4 &model_view, const Unigine::Math::Mat4 &projection,
		const Unigine::NodePtr &to_render) const;

	void setMode(Mode in_mode);
	UNIGINE_INLINE Mode getMode() const { return mode; }

	UNIGINE_INLINE Unigine::TexturePtr getDepthTexture() const { return depth_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getAlbedoTexture() const { return albedo_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getNormalTexture() const { return normal_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getRoughnessTexture() const { return roughness_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getMetalnessTexture() const { return metalness_texture; }

private:
	void init();

	void g_buffers_ready_callback();

private:
	Mode mode{Mode::RenderNode};

	Unigine::TexturePtr depth_texture;
	Unigine::TexturePtr albedo_texture;
	Unigine::TexturePtr normal_texture;
	Unigine::TexturePtr metalness_texture;
	Unigine::TexturePtr roughness_texture;

	Unigine::EventConnection on_viewport_g_buffer_ready_connection;
	Unigine::EventConnection on_render_g_buffer_ready_connection;
	Unigine::ViewportPtr viewport;
};

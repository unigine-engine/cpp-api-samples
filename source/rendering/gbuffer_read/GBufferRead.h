// Extracts G-buffer components (albedo, normal, metalness, roughness, depth)
// into separate textures after the deferred pass. Hooks into the end of
// G-buffer rendering to copy data before lighting calculations.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>

// Extracts G-buffer data (albedo, normals, depth, metalness, roughness) into
// separate textures. Supports extraction from main viewport or custom node render.
class GBufferRead final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferRead, ComponentBase)

	// Material with fullscreen shader to extract G-buffer data
	PROP_PARAM(Material, fetch_buffers_post_material)

	COMPONENT_INIT(init)

	// Extraction source: single node or entire main viewport
	enum class Mode
	{
		RenderNode,
		RenderMainViewport
	};

	// Renders a single node and extracts its G-buffer data
	void renderNode(const Unigine::Math::Mat4 &model_view, const Unigine::Math::Mat4 &projection,
		const Unigine::NodePtr &to_render) const;

	void setMode(Mode in_mode);
	UNIGINE_INLINE Mode getMode() const { return mode; }

	// Getters for extracted texture data
	UNIGINE_INLINE Unigine::TexturePtr getDepthTexture() const { return depth_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getAlbedoTexture() const { return albedo_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getNormalTexture() const { return normal_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getRoughnessTexture() const { return roughness_texture; }
	UNIGINE_INLINE Unigine::TexturePtr getMetalnessTexture() const { return metalness_texture; }

private:
	void init();

	// Called when G-buffer is complete, performs texture extraction
	void g_buffers_ready_callback();

private:
	Mode mode{Mode::RenderNode};

	// Extracted G-buffer component textures
	Unigine::TexturePtr depth_texture;
	Unigine::TexturePtr albedo_texture;
	Unigine::TexturePtr normal_texture;
	Unigine::TexturePtr metalness_texture;
	Unigine::TexturePtr roughness_texture;

	// Event connections for both render modes
	Unigine::EventConnection on_viewport_g_buffer_ready_connection;
	Unigine::EventConnection on_render_g_buffer_ready_connection;
	// Dedicated viewport for RenderNode mode
	Unigine::ViewportPtr viewport;
};

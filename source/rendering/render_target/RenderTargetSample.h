// Demonstrates RenderTarget usage for compositing sprites onto a texture.
// Periodically draws random sprites on a background texture, which is
// then applied to a wall object in the scene.

#pragma once

#include <UnigineComponentSystem.h>

// Composites sprites onto a wall texture via render target operations.
class RenderTargetSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(RenderTargetSample, Unigine::ComponentBase)

	// Material with shader for drawing sprites
	PROP_PARAM(Material, draw_sprite_material)
	// Array of sprite image files
	PROP_ARRAY(File, sprites)
	// Wall object to display the composited texture
	PROP_PARAM(Node, wall_param, "Wall Node")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void update();
	void shutdown();

	// Copies source texture to destination using render target
	void copy_texture(const Unigine::TexturePtr &dest, const Unigine::TexturePtr &src) const;
	// Draws a sprite at specified UV coordinates
	void draw_sprite(float uv_x, float uv_y);

private:
	Unigine::RenderTargetPtr render_target;
	// Loaded sprite textures for random selection
	Unigine::Vector<Unigine::TexturePtr> sprite_textures;
	// Base texture that sprites are composited onto
	Unigine::TexturePtr background_texture;
	// Final composited texture applied to wall
	Unigine::TexturePtr result_texture;

	Unigine::ObjectPtr wall;
	Unigine::MaterialPtr wall_material;
	// Original wall albedo for background reset
	Unigine::ImagePtr wall_albedo;

	// Timer for periodic sprite spawning
	float countdown{0.f};
};

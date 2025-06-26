#pragma once

#include <UnigineComponentSystem.h>

class RenderTargetSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(RenderTargetSample, Unigine::ComponentBase)

	PROP_PARAM(Material, draw_sprite_material)
	PROP_ARRAY(File, sprites)
	PROP_PARAM(Node, wall_param, "Wall Node")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void update();
	void shutdown();

	void copy_texture(const Unigine::TexturePtr &dest, const Unigine::TexturePtr &src) const;
	void draw_sprite(float uv_x, float uv_y);

private:
	Unigine::RenderTargetPtr render_target;
	Unigine::Vector<Unigine::TexturePtr> sprite_textures;
	Unigine::TexturePtr background_texture;
	Unigine::TexturePtr result_texture;

	Unigine::ObjectPtr wall;
	Unigine::MaterialPtr wall_material;
	Unigine::ImagePtr wall_albedo;

	float countdown{0.f};
};

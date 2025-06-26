#pragma once

#include <UnigineComponentSystem.h>

/// This sample demonstrates how to render gui onto a custom texture
/// In GuiToTexture component we pass the texture into material's texture slot
class GuiToTexture final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GuiToTexture, Unigine::ComponentBase);

	/// Name of surface to which the material is applied
	PROP_PARAM(String, surface_name);
	/// Names of texture slots in material
	PROP_ARRAY(String, texture_slot_names);
	/// Resolution of texture that will be created and assigned to material
	PROP_PARAM(IVec2, texture_resolution, Unigine::Math::ivec2(2048, 2048));

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

public:
	UNIGINE_INLINE Unigine::GuiPtr getGui() const { return gui; }
	UNIGINE_INLINE void setAutoUpdateEnabled(bool e) { auto_update_enabled = e; }
	void renderToTexture() const;
	UNIGINE_INLINE Unigine::Math::ivec2 getTextureResolution() const { return texture_resolution; }

private:
	void init();
	void update() const;

private:
	/// Texture in which gui will be rendered
	Unigine::TexturePtr gui_texture;
	/// Gui that is rendered
	Unigine::GuiPtr gui;
	Unigine::WidgetCanvasPtr canvas;

	/// Render target that will be used to render gui on texture
	Unigine::RenderTargetPtr render_target;

	/// If this flag is enabled, gui texture be updated each frame
	bool auto_update_enabled{true};
};

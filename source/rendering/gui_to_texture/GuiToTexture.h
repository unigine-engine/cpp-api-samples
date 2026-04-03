// Renders GUI widgets to a texture for in-world display surfaces.
// Runs the GUI update loop manually and captures output to a texture
// that can be assigned to any material slot (albedo, emission, etc.).

#pragma once

#include <UnigineComponentSystem.h>

// Captures GUI rendering to texture and assigns it to material's texture slots.
class GuiToTexture final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GuiToTexture, Unigine::ComponentBase);

	// Surface that receives the GUI texture
	PROP_PARAM(String, surface_name);
	// Material texture slots to assign the rendered GUI texture
	PROP_ARRAY(String, texture_slot_names);
	// Output texture dimensions
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
	// Output texture containing rendered GUI
	Unigine::TexturePtr gui_texture;
	// Standalone GUI instance (separate from main screen GUI)
	Unigine::GuiPtr gui;
	Unigine::WidgetCanvasPtr canvas;

	// Render target for capturing GUI output
	Unigine::RenderTargetPtr render_target;

	// When enabled, GUI is re-rendered to texture each frame
	bool auto_update_enabled{true};
};

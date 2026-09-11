// Renders a GUI to a texture and applies it to an object surface material or a decal material.
// Useful for in-world displays, screens, and interactive surfaces. The GUI
// update loop runs manually outside the main render to capture to texture.

#include "GuiToTexture.h"

#include <UnigineGame.h>
#include <UnigineDecals.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(GuiToTexture);

using namespace Unigine;

// Executes the full GUI render loop and captures output to texture with mipmaps.
void GuiToTexture::renderToTexture() const
{

	// Save render state and put it at the top of the stack
	// To pop current settings, we will need to call RenderState.RestoreState() at the and of this method
	RenderState::saveState();

	// Now we clear state, so that our rendered texture won't be affected by other render activities
	RenderState::clearStates();

	// Set viewport size matching texture resolution
	RenderState::setViewport(0, 0, texture_resolution.get().x, texture_resolution.get().y);

	// Now we bind GUI texture to slot 0, because GUI renders in slot 0
	render_target->bindColorTexture(0, gui_texture);
	// Enable render target
	render_target->enable();
	// Clear texture and fill it with black color
	RenderState::clearBuffer(RenderState::BUFFER_COLOR, Math::vec4_black);

	// Now we need to perform the whole gui render loop

	// Enable gui so that it will be updated and rendered
	gui->enable();

	// Update all widgets
	gui->update();

	// Render gui
	gui->preRender();
	gui->render();

	// Disable gui
	gui->disable();

	// Now we need to free render target and unbind texture
	render_target->disable();
	render_target->unbindColorTexture(0);

	// Create texture mipmaps (set of textures of different resolutions to ensure correct rendering at longer distances)
	gui_texture->createMipmaps();

	// Pop render state from top of the stack to let render pipeline continue as usual
	RenderState::restoreState();
}

// GUI and render target are created; texture is assigned to specified material slots.
void GuiToTexture::init()
{
	// The GUI texture is applied either to a surface material of an object or to a decal material
	MaterialPtr material;
	if (const auto object = checked_ptr_cast<Object>(node))
	{
		// Find the required surface
		const int surface = object->findSurface(surface_name);
		if (surface == -1)
		{
			Log::error("GuiToTexture::init(): surface with name %s not found\n", surface_name.get());
			return;
		}

		// We need to inherit material, because there might be other objects that are using this material
		// and we don't want all objects in the scene to get gui from this component
		material = object->getMaterialInherit(surface);
	}
	else if (const auto decal = checked_ptr_cast<Decal>(node))
	{
		// A decal has a single material and no surfaces, so surface_name is not used here.
		// Material is inherited for the same reason as above: other decals may share it
		material = decal->getMaterialInherit();
	}
	else
	{
		Log::error("GuiToTexture::init(): the component must be assigned to an Object or a Decal node\n");
		return;
	}

	render_target = RenderTarget::create();

	gui = Gui::create();
	gui->setSize(texture_resolution.get());
	gui->setDefaultColor(Math::vec4_red);

	// Here we need to specify format of texture: RGBA8
	// and set the flag FORMAT_USAGE_RENDER to be able to render into the texture
	// we also need to specify the sampler by setting another flag (bilinear sampler in our case)
	gui_texture = Texture::create();
	gui_texture->create2D(texture_resolution.get().x, texture_resolution.get().y,
		Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_RENDER | Texture::SAMPLER_FILTER_BILINEAR);

	for (int texture_index = 0, num_textures = texture_slot_names.size(); texture_index < num_textures; ++texture_index)
	{
		const ComponentVariableString &texture_name = texture_slot_names[texture_index];
		material->setTexture(texture_name, gui_texture);
	}
}

// Texture is re-rendered each frame when auto-update is enabled.
void GuiToTexture::update() const
{
	if (auto_update_enabled)
	{
		renderToTexture();
	}
}

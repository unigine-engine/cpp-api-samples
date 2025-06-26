#include "GuiToTexture.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(GuiToTexture);

using namespace Unigine;

void GuiToTexture::renderToTexture() const
{
	// Render gui onto texture

	// Save render state and put it at the top of the stack
	// To pop current settings, we will need to call RenderState.RestoreState() at the and of this method
	RenderState::saveState();

	// Now we clear state, so that our rendered texture won't be affected by other render activities
	RenderState::clearStates();

	// Set viewport size matching texture resolution
	RenderState::setViewport(0, 0, texture_resolution.get().x, texture_resolution.get().y);

	// Now we bind gui texture to slot 0, because gui renders in slot 0
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

void GuiToTexture::init()
{
	// Obtain object from the node this component is attached to
	const auto object = checked_ptr_cast<Object>(node);
	if (!object)
	{
		Log::error("GuiToTexture::init: component must be assigned to object");
	}

	// Find the required surface
	const int surface = object->findSurface(surface_name);
	if (surface == -1)
	{
		Log::error("GuiToTexture::init: surface with name %s not found", surface_name.get());
	}

	render_target = RenderTarget::create();

	// We need to inherit material, because there might be other objects that are using this material
	// and we don't want all objects in the scene to get gui from this component
	const MaterialPtr material = object->getMaterialInherit(surface);

	gui = Gui::create();
	gui->setSize(texture_resolution.get());
	gui->setDefaultColor(Math::vec4_red);

	// here we need to specify format of texture: rgba8
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

void GuiToTexture::update() const
{
	// Update gui if flag is set
	if (auto_update_enabled)
	{
		renderToTexture();
	}
}

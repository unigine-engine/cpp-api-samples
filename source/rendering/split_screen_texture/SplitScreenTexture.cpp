// Split-screen multiplayer rendering. Two cameras render to separate textures
// which are displayed stacked vertically using sprite widgets. Also maps the
// textures to in-world TV display objects for picture-in-picture effect.

#include "SplitScreenTexture.h"

#include <UnigineGame.h>
#include <UnigineGui.h>
#include <UniginePlayers.h>
#include <UnigineTextures.h>
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(SplitScreenTexture);

using namespace Unigine;
using namespace Math;

// Cameras are extracted from player nodes; textures and viewports are created for each view.
void SplitScreenTexture::init()
{
	init_gui();
	// Extract camera objects from the assigned player nodes for rendering
	cached_camera_top = checked_ptr_cast<Player>(camera_top.get())->getCamera();
	cached_camera_bottom = checked_ptr_cast<Player>(camera_bottom.get())->getCamera();

	// Create render target textures sized to half the screen height (stacked layout)
	texture_top = Texture::create();
	create_texture_2D(*texture_top);
	texture_bottom = Texture::create();
	create_texture_2D(*texture_bottom);

	// Each camera needs its own viewport for independent render state
	viewport_top = Viewport::create();
	viewport_bottom = Viewport::create();

	// Connect textures to in-world TV display objects for picture-in-picture effect
	bind_texture(display_player1.get(), texture_top);
	bind_texture(display_player2.get(), texture_bottom);
}

// GUI sprites and control hint labels are created; default rendering is disabled.
void SplitScreenTexture::init_gui()
{
	// Cache window dimensions for resize detection
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	current_width = window_size.x;
	current_height = window_size.y;
	auto gui = Gui::getCurrent();

	// Disable default scene rendering since we handle it manually via viewports
	Render::setEnabled(false);

	// Create sprite widgets that will display the rendered textures
	widget_top = WidgetSprite::create(gui);
	widget_bottom = WidgetSprite::create(gui);

	// Vertical box layout stacks the two views (top player above bottom player)
	lay = WidgetVBox::create();
	gui->addChild(lay, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	lay->addChild(widget_top);
	lay->addChild(widget_bottom);

	// Control hints for each player's input scheme
	wasd_label = setup_label(gui, "Use WASD to move");
	arrows_label = setup_label(gui, "Use arrows to move");
	update_labels_pos();
}

// Label widget is created with white text and added to the GUI overlay.
WidgetLabelPtr SplitScreenTexture::setup_label(Unigine::GuiPtr gui, const char *text)
{
	auto label = WidgetLabel::create(gui, text);
	label->setFontColor(vec4(1.f, 1.f, 1.f, 1.f));
	label->setFontSize(24);
	// Overlap alignment allows manual positioning over other widgets
	gui->addChild(label, Gui::ALIGN_OVERLAP);
	// Calculate label dimensions for proper positioning
	label->arrange();
	return label;
}

// Labels are positioned in the top-right corner of each player's viewport.
void SplitScreenTexture::update_labels_pos()
{
	// WASD label goes in top viewport (player 1)
	wasd_label->setPosition(current_width - 30 - wasd_label->getWidth(), 10);
	// Arrows label goes in bottom viewport (player 2), offset by half screen height
	arrows_label->setPosition(current_width - 30 - arrows_label->getWidth(),
	 	current_height / 2 + 10);
}

// Material is inherited so changes are local; UV transform flips texture for correct orientation.
void SplitScreenTexture::bind_texture(const NodePtr &tv_display, const TexturePtr &texture)
{
	auto obj = checked_ptr_cast<ObjectMeshStatic>(tv_display);
	auto mat = obj->getMaterial(0);
	// Inherit material so texture assignment doesn't affect other objects using same material
	mat = mat->inherit();
	mat->setTexture(mat->findTexture("albedo"), texture);
	// UV transform flips and mirrors the texture for correct TV display orientation
	mat->setParameterFloat4("uv_transform", vec4(-1.f, Render::isFlipped() ? 1.f : -1.f, 0.f, 0.f));
	obj->setMaterial(mat, 0);
}

// Texture is created at half window height for stacked split-screen layout.
void SplitScreenTexture::create_texture_2D(Texture &texture)
{
	// RG11B10F format provides HDR color range; height is halved for two-player split
	texture.create2D(current_width, current_height / 2, Texture::FORMAT_RG11B10F,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
}

// Each viewport renders its camera view to texture; sprites display the results.
void SplitScreenTexture::update()
{
	// Recreate textures if window was resized
	update_screen_size();

	// Render each player's camera view to their respective texture
	viewport_top->renderTexture2D(cached_camera_top, texture_top);
	viewport_bottom->renderTexture2D(cached_camera_bottom, texture_bottom);

	// Update sprite widgets with rendered textures; flip flag handles platform differences
	widget_top->setRender(texture_top, !Render::isFlipped());
	widget_bottom->setRender(texture_bottom, !Render::isFlipped());
}

// Window size changes trigger texture recreation to match new dimensions.
void SplitScreenTexture::update_screen_size()
{
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	int app_width = window_size.x;
	bool need_update = false;

	// Check for width change
	if (app_width != current_width)
	{
		current_width = app_width;
		need_update = true;
	}

	// Check for height change
	int app_height = window_size.y;
	if (app_height != current_height)
	{
		need_update = true;
		current_height = app_height;
	}

	// Recreate textures with new dimensions and reposition labels
	if (need_update)
	{
		create_texture_2D(*texture_top);
		create_texture_2D(*texture_bottom);
		update_labels_pos();
	}
}

// Default rendering is restored; all GUI widgets are scheduled for deletion.
void SplitScreenTexture::shutdown()
{
	// Re-enable default rendering that was disabled in init_gui()
	Render::setEnabled(true);
	// Schedule widget deletion to avoid destroying during event handling
	lay.deleteLater();
	widget_top.deleteLater();
	widget_bottom.deleteLater();
	wasd_label.deleteLater();
	arrows_label.deleteLater();
}

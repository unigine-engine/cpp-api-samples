#include "SplitScreenTexture.h"

#include <UnigineGame.h>
#include <UnigineGui.h>
#include <UniginePlayers.h>
#include <UnigineTextures.h>
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(SplitScreenTexture);

using namespace Unigine;
using namespace Math;

void SplitScreenTexture::init()
{
	init_gui();
	cached_camera_top = checked_ptr_cast<Player>(camera_top.get())->getCamera();
	cached_camera_bottom = checked_ptr_cast<Player>(camera_bottom.get())->getCamera();

	texture_top = Texture::create();
	create_texture_2D(*texture_top);
	texture_bottom = Texture::create();
	create_texture_2D(*texture_bottom);
	viewport_top = Viewport::create();
	viewport_bottom = Viewport::create();
	bind_texture(display_player1.get(), texture_top);
	bind_texture(display_player2.get(), texture_bottom);
}

void SplitScreenTexture::init_gui()
{
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	current_width = window_size.x;
	current_height = window_size.y;
	auto gui = Gui::getCurrent();
	Render::setEnabled(false);
	widget_top = WidgetSprite::create(gui);
	widget_bottom = WidgetSprite::create(gui);
	lay = WidgetVBox::create();
	gui->addChild(lay, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	lay->addChild(widget_top);
	lay->addChild(widget_bottom);
	wasd_label = setup_label(gui, "Use WASD to move");
	arrows_label = setup_label(gui, "Use arrows to move");
	update_labels_pos();
}

WidgetLabelPtr SplitScreenTexture::setup_label(Unigine::GuiPtr gui, const char *text)
{
	auto label = WidgetLabel::create(gui, text);
	label->setFontColor(vec4(1.f, 1.f, 1.f, 1.f));
	label->setFontSize(24);
	gui->addChild(label, Gui::ALIGN_OVERLAP);
	label->arrange();
	return label;
}

void SplitScreenTexture::update_labels_pos()
{
	wasd_label->setPosition(current_width - 30 - wasd_label->getWidth(), 10);
	arrows_label->setPosition(current_width - 30 - arrows_label->getWidth(),
	 	current_height / 2 + 10);
}

void SplitScreenTexture::bind_texture(const NodePtr &tv_display, const TexturePtr &texture)
{
	auto obj = checked_ptr_cast<ObjectMeshStatic>(tv_display);
	auto mat = obj->getMaterial(0);
	mat = mat->inherit();
	mat->setTexture(mat->findTexture("albedo"), texture);
	mat->setParameterFloat4("uv_transform", vec4(-1.f, Render::isFlipped() ? 1.f : -1.f, 0.f, 0.f));
	obj->setMaterial(mat, 0);
}

void SplitScreenTexture::create_texture_2D(Texture &texture)
{
	texture.create2D(current_width, current_height / 2, Texture::FORMAT_RG11B10F,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
}

void SplitScreenTexture::update()
{
	update_screen_size();
	viewport_top->renderTexture2D(cached_camera_top, texture_top);
	viewport_bottom->renderTexture2D(cached_camera_bottom, texture_bottom);
	widget_top->setRender(texture_top, !Render::isFlipped());
	widget_bottom->setRender(texture_bottom, !Render::isFlipped());
}

void SplitScreenTexture::update_screen_size()
{
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	int app_width = window_size.x;
	bool need_update = false;
	if (app_width != current_width)
	{
		current_width = app_width;
		need_update = true;
	}
	int app_height = window_size.y;
	if (app_height != current_height)
	{
		need_update = true;
		current_height = app_height;
	}
	if (need_update)
	{
		create_texture_2D(*texture_top);
		create_texture_2D(*texture_bottom);
		update_labels_pos();
	}
}

void SplitScreenTexture::shutdown()
{
	Render::setEnabled(true);
	lay.deleteLater();
	widget_top.deleteLater();
	widget_bottom.deleteLater();
	wasd_label.deleteLater();
	arrows_label.deleteLater();
}

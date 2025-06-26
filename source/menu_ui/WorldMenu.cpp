#include "WorldMenu.h"

REGISTER_COMPONENT(WorldMenu);

using namespace Unigine;
using namespace Math;

void WorldMenu::init()
{
	main_hbox = WidgetHBox::create();
	ivec4 padding = ui_configuration->common->padding.get();
	main_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	int space = ui_configuration->hint->space_x;
	hint_hbox = WidgetHBox::create(space, 0);
	hint_hbox->setBackground(1);
	hint_hbox->setBackgroundTexture(ui_configuration->hint->background.get());
	hint_hbox->setBackgroundColor(ui_configuration->hint->background_color.get());
	hint_hbox->setBackground9Sliced(true);
	vec4 hint_offsets = ui_configuration->hint->background_slice_offsets.get();
	hint_hbox->setBackground9SliceOffsets(hint_offsets.x, hint_offsets.y, hint_offsets.z, hint_offsets.w);
	hint_hbox->setBackground9SliceScale(ui_configuration->hint->background_slice_scale.get());
	ivec4 hint_padding = ui_configuration->hint->background_padding.get();
	hint_hbox->setPadding(hint_padding.x - space, hint_padding.y - space, hint_padding.z, hint_padding.w);

	auto hint_sprite = WidgetSprite::create();
	hint_sprite->addLayer();
	hint_sprite->setLayerTexture(0, ui_configuration->hint->icon.get());
	hint_sprite->setLayerColor(0, ui_configuration->hint->icon_color.get());
	hint_sprite->setWidth(ui_configuration->hint->icon_width.get());
	hint_sprite->setHeight(ui_configuration->hint->icon_height.get());
	hint_hbox->addChild(hint_sprite, Gui::ALIGN_LEFT);

	hint_label = WidgetLabel::create("Press <b>ESC</b> to interact with UI");
	hint_label->setFontRich(1);
	hint_label->setFont(ui_configuration->hint->label_font.get());
	hint_label->setFontSize(ui_configuration->hint->label_font_size.get());
	hint_label->setFontColor(ui_configuration->hint->label_font_color.get());
	hint_hbox->addChild(hint_label);

	back_hbox = WidgetHBox::create(ui_configuration->back->space_between_button_and_label.get(), 0);
	back_hbox->setBackground(1);
	back_hbox->setBackgroundTexture(ui_configuration->back->background.get());
	back_hbox->setBackgroundColor(ui_configuration->back->background_color.get());
	back_hbox->setBackground9Sliced(false);
	vec4 back_offsets = ui_configuration->back->background_slice_offsets.get();
	back_hbox->setBackground9SliceOffsets(back_offsets.x, back_offsets.y, back_offsets.z, back_offsets.w);
	back_hbox->setBackground9SliceScale(ui_configuration->back->background_slice_scale.get());
	ivec4 back_padding = ui_configuration->back->background_padding.get();
	back_hbox->setPadding(back_padding.x, back_padding.y, back_padding.z, back_padding.w);

	back_button_sprite = WidgetSprite::create();
	back_button_sprite->addLayer();
	back_button_sprite->setLayerTexture(0, ui_configuration->back->button_icon.get());
	back_button_sprite->setLayerColor(0, ui_configuration->back->button_icon_color.get());
	back_button_sprite->setWidth(ui_configuration->back->button_width.get());
	back_button_sprite->setHeight(ui_configuration->back->button_height.get());
	back_hbox->addChild(back_button_sprite, Gui::ALIGN_LEFT);

	back_label = WidgetLabel::create("<p align=center>Back to Main Menu</p>");
	back_label->setFontRich(1);
	back_label->setFont(ui_configuration->back->label_font.get());
	back_label->setFontSize(ui_configuration->back->label_font_size.get());
	back_label->setFontColor(ui_configuration->back->label_font_color.get());
	back_hbox->addChild(back_label, Gui::ALIGN_RIGHT);

	main_hbox->addChild(hint_hbox, Gui::ALIGN_BOTTOM | Gui::ALIGN_CENTER);
	main_hbox->addChild(back_hbox, Gui::ALIGN_BOTTOM | Gui::ALIGN_RIGHT);

	WindowManager::getMainWindow()->addChild(main_hbox, Gui::ALIGN_OVERLAP /*| Gui::ALIGN_EXPAND*/);
}

void WorldMenu::update()
{
	EngineWindowViewportPtr window = WindowManager::getMainWindow();
	ivec2 size = window->getClientSize();

	hint_hbox->setHidden(!Input::isMouseGrab());
	hint_hbox->setEnabled(Input::isMouseGrab());

	main_hbox->setWidth(size.x);
	main_hbox->arrange();

	main_hbox->setPositionX(size.x - ui_configuration->common->right_offset.get() - main_hbox->getWidth());
	main_hbox->setPositionY(size.y - ui_configuration->common->bottom_offset.get() - main_hbox->getHeight());

	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT);

	bool hovered = back_hbox->getMouseX() >= 0 && back_hbox->getMouseX() < back_hbox->getWidth()
		&& back_hbox->getMouseY() >= 0 && back_hbox->getMouseY() < back_hbox->getHeight();

	if (pressed && up)
	{
		pressed = false;
		Input::setMouseHandle(mouse_handle_at_click);
		if (hovered)
		{
			World::loadWorld(ui_configuration->main_menu_world_name.get(), true);
		}
	}

	if (!pressed && down && hovered)
	{
		pressed = true;
		// for samples with MOUSE_HANDLE_GRAB
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
	}

	back_hbox->setColor(pressed ? ui_configuration->back->button_tint_color : ui_configuration->back->background_color);
}

void WorldMenu::shutdown()
{
	main_hbox.deleteLater();
}

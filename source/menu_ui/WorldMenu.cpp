#include "WorldMenu.h"

#include "SamplesManager.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(WorldMenu);


using namespace Unigine;
using namespace Math;

bool WorldMenu::is_selection_active = false;

void WorldMenu::init()
{
	auto sample = SamplesManager::get()->getSampleByWorldPath(World::getPath());
	if (sample)
	{
		current_sample.title = sample->title;
		current_sample.world_name = sample->world_name;
	}
	else
	{
		current_sample.title = String::filename(World::getPath());
		current_sample.world_name = current_sample.title;
	}

	main_hbox = WidgetHBox::create();
	ivec4 padding = ui_configuration->common->padding.get();
	main_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	// create hint box
	init_hint_box();

	// create back button
	init_back_button();

	init_navigation_bar();
	nav_select->setButtonPressed(is_selection_active);

	auto navigation = WidgetVBox::create();
	navigation->addChild(selector_hbox);
	navigation->addChild(navigation_hbox);

	int space_y = ui_configuration->common->space_between_hint_and_navigation;
	auto aux_vbox = WidgetVBox::create(0, space_y);
	aux_vbox->setPadding(0, 0, -space_y, -space_y);
	aux_vbox->addChild(hint_hbox);
	aux_vbox->addChild(navigation);
	//	main_hbox->addChild(navigation_hbox, Gui::ALIGN_BOTTOM | Gui::ALIGN_CENTER);
	main_hbox->addChild(aux_vbox, Gui::ALIGN_BOTTOM | Gui::ALIGN_CENTER);

	aux_vbox = WidgetVBox::create();
	aux_vbox->addChild(back_hbox, Gui::ALIGN_BOTTOM /*, Gui::ALIGN_OVERLAP*/);

	main_hbox->addChild(aux_vbox, Gui::ALIGN_BOTTOM | Gui::ALIGN_RIGHT);
	WindowManager::getMainWindow()->addChild(main_hbox, Gui::ALIGN_OVERLAP /*| Gui::ALIGN_EXPAND*/);
}

void WorldMenu::update()
{
	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT) || Input::isTouchDown(0);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT) || Input::isTouchUp(0);

	// update hint state
	hint_hbox->setHidden(!Input::isMouseGrab());
	hint_hbox->setEnabled(Input::isMouseGrab());

	update_back_button(up, down);
	update_navigation(up, down);

	// adjust UI position in case if window size changed
	EngineWindowViewportPtr window = WindowManager::getMainWindow();
	ivec2 size = window->getClientSize();

	main_hbox->setWidth(size.x);
	main_hbox->arrange();

	main_hbox->setPositionX(
		size.x - ui_configuration->common->right_offset.get() - main_hbox->getWidth());
	main_hbox->setPositionY(
		size.y - ui_configuration->common->bottom_offset.get() - main_hbox->getHeight());
}

void WorldMenu::shutdown()
{
	main_hbox.deleteLater();
	hint_hbox.deleteLater();
	back_hbox.deleteLater();

	selector_hbox.deleteLater();
	for (int i = 0; i < select_buttons.size(); i++)
	{
		delete select_buttons[i];
	}
	select_buttons.clear();

	delete nav_prev;
	delete nav_next;
	delete nav_select;
}

void WorldMenu::init_back_button()
{
	auto &config = ui_configuration->back;
	int space = config->space_between_icon_and_label.get();
	back_hbox = WidgetHBox::create(space, 0);
	back_hbox->setBackground(1);
	back_hbox->setBackgroundTexture(config->background.get());
	back_hbox->setBackgroundColor(config->background_color.get());
	back_hbox->setBackground9Sliced(true);
	vec4 back_offsets = config->background_slice_offsets.get();
	back_hbox->setBackground9SliceOffsets(back_offsets.x, back_offsets.y, back_offsets.z,
		back_offsets.w);
	back_hbox->setBackground9SliceScale(config->background_slice_scale.get());
	ivec4 back_padding = config->background_padding.get();
	back_hbox->setPadding(back_padding.x - space, back_padding.y - space, back_padding.z,
		back_padding.w);

	auto back_button_sprite = WidgetSprite::create();
	back_button_sprite->addLayer();
	back_button_sprite->setLayerTexture(0, config->icon->icon.get());
	back_button_sprite->setLayerColor(0, config->icon->icon_color.get());
	back_button_sprite->setWidth(config->icon->icon_size.get().x);
	back_button_sprite->setHeight(config->icon->icon_size.get().y);
	back_hbox->addChild(back_button_sprite, Gui::ALIGN_LEFT);

	auto back_label = WidgetLabel::create("<p align=center>Back to Main Menu</p>");
	back_label->setFontRich(1);
	back_label->setFont(config->label->font.get());
	back_label->setFontSize(config->label->font_size.get());
	back_label->setFontColor(config->label->font_color.get());
	back_hbox->addChild(back_label, Gui::ALIGN_RIGHT);
}

void WorldMenu::init_hint_box()
{
	auto &config = ui_configuration->hint;
	int space = config->space_x;
	hint_hbox = WidgetHBox::create(space, 0);
	hint_hbox->setBackground(1);
	hint_hbox->setHeight(config->height);
	hint_hbox->setBackgroundTexture(config->background.get());
	hint_hbox->setBackgroundColor(config->background_color.get());
	hint_hbox->setBackground9Sliced(true);
	vec4 hint_offsets = config->background_slice_offsets.get();
	hint_hbox->setBackground9SliceOffsets(hint_offsets.x, hint_offsets.y, hint_offsets.z,
		hint_offsets.w);
	hint_hbox->setBackground9SliceScale(config->background_slice_scale.get());
	ivec4 hint_padding = config->background_padding.get();
	hint_hbox->setPadding(hint_padding.x - space, hint_padding.y - space, hint_padding.z,
		hint_padding.w);

	auto hint_sprite = WidgetSprite::create();
	hint_sprite->addLayer();
	hint_sprite->setLayerTexture(0, config->icon->icon.get());
	hint_sprite->setLayerColor(0, config->icon->icon_color.get());
	hint_sprite->setWidth(config->icon->icon_size.get().x);
	hint_sprite->setHeight(config->icon->icon_size.get().y);
	hint_hbox->addChild(hint_sprite, Gui::ALIGN_LEFT);

	auto hint_label = WidgetLabel::create("Press <b>ESC</b> to interact with UI");
	hint_label->setFontRich(1);
	hint_label->setFont(config->label->font.get());
	hint_label->setFontSize(config->label->font_size.get());
	hint_label->setFontColor(config->label->font_color.get());
	hint_hbox->addChild(hint_label);
}

void WorldMenu::init_navigation_bar()
{
	// create selector box
	init_selector();

	auto &config = ui_configuration->navigation;
	config->button_prev->button_height = config->height;
	config->button_next->button_height = config->height;

	// create selector button
	nav_select = new SelectorButton(current_sample.title, config.get(),
		[this](bool val) { selector_hbox->setHidden(!val); });

	String prev_world, next_world;
	SamplesManager::get()->getPrevNextSamplesID(current_sample.world_name, prev_world, next_world);

	// create button to the prev sample
	const Sample *s = SamplesManager::get()->getSampleByID(prev_world);
	nav_prev = new NavigationButton(prev_world, s ? s->title : prev_world,
		config->button_prev.get(), config->tooltip.get());
	// create button to the next sample
	s = SamplesManager::get()->getSampleByID(next_world);
	nav_next = new NavigationButton(next_world, s ? s->title : next_world,
		config->button_next.get(), config->tooltip.get());

	navigation_hbox = WidgetHBox::create();
	navigation_hbox->addChild(nav_prev->getWidget(), Gui::ALIGN_LEFT);
	navigation_hbox->addChild(nav_select->getWidget(), Gui::ALIGN_LEFT);
	navigation_hbox->addChild(nav_next->getWidget(), Gui::ALIGN_LEFT);
}

void WorldMenu::init_selector()
{
	auto category = SamplesManager::get()->getCategoryBySampleID(current_sample.world_name);
	String title = category ? category->title : "No Category";

	auto &config = ui_configuration->selector;
	int width = ui_configuration->navigation->width;
	config->button->button_width = width;

	// create main box
	selector_hbox = WidgetHBox::create();
	selector_vbox = WidgetVBox::create();
	selector_vbox->setBackground(1);
	selector_vbox->setBackgroundTexture(config->background.get());
	selector_vbox->setBackground9Sliced(true);
	vec4 nav_offsets = config->background_slice_offsets.get();
	selector_vbox->setBackground9SliceOffsets(nav_offsets.x, nav_offsets.y, nav_offsets.z,
		nav_offsets.w);
	selector_vbox->setBackground9SliceScale(config->background_slice_scale.get());
	selector_vbox->setHeight(config->height);
	selector_vbox->setWidth(width);

	// create header with category name
	auto header = WidgetHBox::create();
	ivec4 padding = config->header_padding.get();
	header->setPadding(padding.x, padding.y, padding.z, padding.w);

	String text = "Samples C++ - " + title;
	auto selector_label = WidgetLabel::create(text);
	selector_label->setFont(config->header_font->font.get());
	selector_label->setFontSize(config->header_font->font_size.get());
	selector_label->setFontColor(config->header_font->font_color.get());
	header->addChild(selector_label);

	auto spacer = WidgetHBox::create();
	spacer->setBackground(1);
	spacer->setBackgroundTexture(config->spacer_background.get());
	spacer->setHeight(config->spacer_height);
	spacer->setWidth(width - 2);

	// create samples scrollbox
	int space = config->space_between_buttons;
	selector_scroll = WidgetScrollBox::create(0, space);
	selector_scroll->setBorder(0);
	padding = config->list_padding.get();
	selector_scroll->setPadding(padding.x, padding.y, padding.z, padding.w);
	selector_scroll->setHScrollEnabled(false);
	selector_scroll->setVScrollHidden(WidgetScrollBox::SCROLL_RENDER_MODE::ALWAYS_HIDE_NO_BOUNDS);
	selector_scroll->arrange();

	selector_vbox->addChild(header);
	selector_vbox->addChild(spacer);
	selector_vbox->addChild(selector_scroll, Gui::ALIGN_EXPAND);

	selector_hbox->addChild(selector_vbox);

	if (category)
	{
		for (auto &s : category->samples)
		{
			WorldButton *button = new WorldButton(s.title, s.world_name, config->button.get(),
				s.world_name == current_sample.world_name);
			select_buttons.append(button);
			selector_scroll->addChild(button->getWidget());
		}
	}
	else
	{
		WorldButton *button = new WorldButton(current_sample.title, current_sample.world_name,
			config->button.get(), true);
		select_buttons.append(button);
		selector_scroll->addChild(button->getWidget());
	}
}

void WorldMenu::update_navigation(bool up, bool down)
{
	nav_prev->update(up, down);
	nav_next->update(up, down);

	nav_select->update(up, down);

	if (!selector_hbox->isHidden())
	{
		for (auto &b : select_buttons)
			b->update(up, down);

		update_selector();
	}
}

void WorldMenu::update_selector()
{
	bool hovered = is_hovered(selector_vbox);
	if (hovered && !is_selector_hovered)
	{
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
		is_selector_hovered = hovered;
	}
	if (!hovered && is_selector_hovered)
	{
		Input::setMouseHandle(mouse_handle_at_click);
		is_selector_hovered = hovered;
	}

	if (is_hovered(selector_scroll))
	{
		int wheel = Input::getMouseWheel();
		if (wheel)
		{
			int value = selector_scroll->getVScrollValue();
			int step = selector_scroll->getVScrollStepSize();
			selector_scroll->setVScrollValue(value - wheel * step * 4);
		}
	}
	else
		selector_scroll->removeFocus();
}

void WorldMenu::update_back_button(bool up, bool down)
{
	bool hovered = is_hovered(back_hbox);

	if (pressed && up)
	{
		pressed = false;
		Input::setMouseHandle(mouse_handle_at_click);
		if (hovered)
		{
			World::loadWorld(ui_configuration->main_menu_world_name.get(), true);
			is_selection_active = false;
		}
	}

	if (!pressed && down && hovered)
	{
		pressed = true;
		// for samples with MOUSE_HANDLE_GRAB
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
	}

	if (back_prev_hovered != hovered)
	{
		back_hbox->setBackgroundTexture(hovered ? ui_configuration->back->background_hover.get()
												: ui_configuration->back->background.get());
		back_prev_hovered = hovered;
	}
	if (back_prev_pressed != pressed)
	{
		back_hbox->setBackgroundColor(pressed ? ui_configuration->back->button_tint_color
											  : ui_configuration->back->background_color);
		back_prev_pressed = pressed;
	}
}

bool WorldMenu::is_hovered(const Unigine::WidgetPtr &widget)
{
	ivec2 gui_pos = ivec2(widget->getGui()->getMouseX(), widget->getGui()->getMouseY());

	int x = widget->getScreenPositionX();
	int y = widget->getScreenPositionY();

	return gui_pos.x >= x && gui_pos.x < x + widget->getWidth()
		&& gui_pos.y >= y && gui_pos.y < y + widget->getHeight();
}

bool WorldMenu::Button::isHovered() const
{
	if (!button_hbox)
		return false;

	ivec2 gui_pos = ivec2(button_hbox->getGui()->getMouseX(), button_hbox->getGui()->getMouseY());

	int x = button_hbox->getScreenPositionX();
	int y = button_hbox->getScreenPositionY();

	return gui_pos.x >= x && gui_pos.x < x + button_hbox->getWidth()
		&& gui_pos.y >= y && gui_pos.y < y + button_hbox->getHeight();
}

WorldMenu::NavigationButton::NavigationButton(const Unigine::String &world_path,
	const Unigine::String &title, UINavigationButton &ui, UITooltip &tooltip)
	: world_path(world_path)
{
	button_hbox = WidgetHBox::create(0, 0);
	button_hbox->setWidth(ui.button_width);
	button_hbox->setHeight(ui.button_height);
	button_hbox->setBackground(1);
	button_hbox->setBackgroundTexture(ui.background.get());
	button_hbox->setBackground9Sliced(true);
	vec4 nav_offsets = ui.background_slice_offsets.get();
	button_hbox->setBackground9SliceOffsets(nav_offsets.x, nav_offsets.y, nav_offsets.z,
		nav_offsets.w);
	button_hbox->setBackground9SliceScale(ui.background_slice_scale.get());

	auto sprite = WidgetSprite::create();
	sprite->addLayer();
	sprite->setLayerTexture(0, ui.icon->icon.get());
	sprite->setWidth(ui.icon->icon_size.get().x);
	sprite->setHeight(ui.icon->icon_size.get().y);
	button_hbox->addChild(sprite, Gui::ALIGN_CENTER);
	button_hbox->arrange();

	background = ui.background.get();
	background_hover = ui.background_hover.get();

	background_color = ui.background_color.get();
	background_tint_color = ui.background_tint_color.get();

	tooltip_hbox = WidgetHBox::create(0, 0);
	tooltip_hbox->setBackground(1);
	tooltip_hbox->setBackgroundTexture(tooltip.background.get());
	tooltip_hbox->setBackgroundColor(tooltip.background_color.get());
	tooltip_hbox->setHeight(tooltip.height);
	ivec4 padding = tooltip.padding.get();
	tooltip_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	String text = ui.tooltip_text.get() + title;
	auto label = WidgetLabel::create(text);
	label->setFont(tooltip.font->font.get());
	label->setFontSize(tooltip.font->font_size.get());
	label->setFontColor(tooltip.font->font_color.get());
	label->setTextAlign(Gui::ALIGN_CENTER);

	tooltip_hbox->addChild(label);
	tooltip_hbox->setHidden(true);
	tooltip_hbox->setOrder(125);

	Gui::getCurrent()->addChild(tooltip_hbox, Gui::ALIGN_OVERLAP);

	time_delay = tooltip.time_delay < 0.f ? 0.1f : tooltip.time_delay;
	pos = tooltip.pos.get();
}

void WorldMenu::NavigationButton::update(bool up, bool down)
{
	if (!button_hbox)
		return;

	bool hovered = isHovered();

	// update tooltip time
	if (prev_hovered && hovered && !(prev_pressed || pressed))
		current_time += Game::getIFps();
	else
		current_time = 0.f;

	// update tooltip state
	if (current_time >= time_delay && tooltip_hbox->isHidden())
	{
		ivec2 mouse_pos = Input::getMousePosition() - Gui::getCurrent()->getPosition();
		tooltip_hbox->setHidden(false);
		tooltip_hbox->setPositionX(mouse_pos.x + pos.x);
		tooltip_hbox->setPositionY(mouse_pos.y - tooltip_hbox->getHeight() + pos.y);
	}

	if (current_time < time_delay && !tooltip_hbox->isHidden())
	{
		tooltip_hbox->setHidden(true);
	}

	// update button state
	if (pressed && up)
	{
		pressed = false;
		Input::setMouseHandle(mouse_handle_at_click);
		if (hovered)
			World::loadWorld(world_path, true);
	}

	if (!pressed && down && hovered)
	{
		pressed = true;
		// for samples with MOUSE_HANDLE_GRAB
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
	}

	if (prev_hovered != hovered)
	{
		button_hbox->setBackgroundTexture(hovered ? background_hover : background);
		prev_hovered = hovered;
	}
	if (prev_pressed != pressed)
	{
		button_hbox->setBackgroundColor(pressed ? background_tint_color : background_color);
		prev_pressed = pressed;
	}
}

WorldMenu::WorldButton::WorldButton(const Unigine::String &text, const Unigine::String &world_path,
	UIWorldButton &config, bool is_current)
	: world_path(world_path)
{
	background_color = is_current ? config.background_selected_color.get()
								  : config.background_color.get();
	background_tint_color = config.background_tint_color.get();
	background_selected_color = config.background_selected_color.get();

	button_hbox = WidgetHBox::create();
	button_hbox->setHeight(config.button_height);
	button_hbox->setWidth(config.button_width);
	button_hbox->setBackground(1);
	button_hbox->setBackgroundTexture(config.background.get());
	button_hbox->setBackgroundColor(background_color);

	auto label = WidgetLabel::create(text);
	label->setFont(config.font->font.get());
	label->setFontSize(config.font->font_size.get());
	label->setFontColor(config.font->font_color.get());
	button_hbox->addChild(label, Gui::ALIGN_CENTER);
	button_hbox->arrange();
}

void WorldMenu::WorldButton::update(bool up, bool down)
{
	if (!button_hbox)
		return;

	bool hovered = isHovered();

	if (pressed && up)
	{
		pressed = false;
		Input::setMouseHandle(mouse_handle_at_click);
		if (hovered)
		{
			World::loadWorld(world_path, true);
		}
	}

	if (!pressed && down && hovered)
	{
		pressed = true;
		// for samples with MOUSE_HANDLE_GRAB
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
	}

	if (prev_hovered != hovered)
	{
		button_hbox->setBackgroundColor(hovered ? background_tint_color : background_color);
		prev_hovered = hovered;
	}
	if (prev_pressed != pressed)
	{
		button_hbox->setBackgroundColor(pressed ? background_selected_color : background_color);
		prev_pressed = pressed;
	}
}

WorldMenu::SelectorButton::SelectorButton(const String &sample_name, UINavigationPanel &config,
	std::function<void(bool)> on_clicked)
	: on_clicked(on_clicked)
{
	button_hbox = WidgetHBox::create(0, 0);
	button_hbox->setBackground(1);
	button_hbox->setBackgroundTexture(config.background.get());
	button_hbox->setBackground9Sliced(false);
	button_hbox->setWidth(config.width);
	button_hbox->setHeight(config.height);

	auto sprite = WidgetSprite::create();
	sprite->addLayer();
	sprite->setLayerTexture(0, config.icon->icon.get());
	sprite->setWidth(config.icon->icon_size.get().x);
	sprite->setHeight(config.icon->icon_size.get().y);

	auto label = WidgetLabel::create(sample_name);
	label->setFont(config.label->font.get());
	label->setFontSize(config.label->font_size.get());
	label->setFontColor(config.label->font_color.get());

	auto aux_hbox = WidgetHBox::create(config.space_between_icon_and_label, 0);
	aux_hbox->addChild(sprite, Gui::ALIGN_LEFT);
	aux_hbox->addChild(label, Gui::ALIGN_LEFT);
	button_hbox->addChild(aux_hbox, Gui::ALIGN_CENTER);

	background = config.background.get();
	background_hover = config.background_hover.get();

	background_color = config.background_color.get();
	background_tint_color = config.background_tint_color.get();
}

void WorldMenu::SelectorButton::update(bool up, bool down)
{
	if (!button_hbox)
		return;

	bool hovered = isHovered();

	if (pressed && up)
	{
		pressed = false;
		Input::setMouseHandle(mouse_handle_at_click);
		if (hovered)
		{
			enabled = !enabled;
			on_clicked(enabled);
			is_selection_active = enabled;
		}
	}

	if (!pressed && down && hovered)
	{
		pressed = true;
		mouse_handle_at_click = Input::getMouseHandle();
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
	}

	if (prev_hovered != hovered && !enabled)
	{
		button_hbox->setBackgroundTexture(hovered ? background_hover : background);
		prev_hovered = hovered;
	}
	if (prev_pressed != pressed && !enabled)
	{
		button_hbox->setBackgroundColor(pressed ? background_tint_color : background_color);
		prev_pressed = pressed;
	}
}

void WorldMenu::SelectorButton::setButtonPressed(bool is_pressed)
{
	button_hbox->setBackgroundTexture(is_pressed ? background_hover : background);
	button_hbox->setBackgroundColor(is_pressed ? background_tint_color : background_color);
	enabled = is_pressed;
	is_selection_active = enabled;
	pressed = false;
	prev_pressed = true;

	on_clicked(enabled);
}

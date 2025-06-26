#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class WorldMenu : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldMenu, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);


	struct UICommon : public::Unigine::ComponentStruct
	{
		PROP_TOOLTIP("");
		PROP_PARAM(IVec4, padding);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, bottom_offset);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, right_offset);
	};

	struct UIHintPanel : public::Unigine::ComponentStruct
	{
		PROP_PARAM(Int, space_x, 10);
		PROP_TOOLTIP("");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("");
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_TOOLTIP("");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("");
		PROP_PARAM(IVec4, background_padding);
		PROP_TOOLTIP("");
		PROP_PARAM(File, label_font);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, label_font_size);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, label_font_color);
		PROP_TOOLTIP("");
		PROP_PARAM(File, icon);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, icon_color);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, icon_width);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, icon_height);
	};

	struct UIBackPanel : public::Unigine::ComponentStruct
	{
		PROP_TOOLTIP("");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("");
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_TOOLTIP("");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("");
		PROP_PARAM(IVec4, background_padding);
		PROP_TOOLTIP("");
		PROP_PARAM(File, button_background);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, button_background_color);
		PROP_TOOLTIP("");
		PROP_PARAM(File, button_icon);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, button_icon_color);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, button_width);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, button_height);
		PROP_TOOLTIP("");
		PROP_PARAM(File, label_font);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, label_font_size);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, label_font_color);
		PROP_TOOLTIP("");
		PROP_PARAM(Int, space_between_button_and_label);
		PROP_TOOLTIP("");
		PROP_PARAM(Color, button_tint_color);
	};

	struct UIConfiguration : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Main menu world name");
		PROP_PARAM(String, main_menu_world_name);
		PROP_STRUCT(UICommon, common);
		PROP_STRUCT(UIHintPanel, hint);
		PROP_STRUCT(UIBackPanel, back);
	};

	PROP_STRUCT(UIConfiguration, ui_configuration);

private:
	void init();
	void update();
	void shutdown();

	Unigine::WidgetHBoxPtr main_hbox;

	Unigine::WidgetHBoxPtr hint_hbox;
	Unigine::WidgetLabelPtr hint_label;

	Unigine::WidgetHBoxPtr back_hbox;
	Unigine::WidgetSpritePtr back_button_sprite;
	Unigine::WidgetLabelPtr back_label;

	bool pressed = false;
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_click = Unigine::Input::MOUSE_HANDLE_USER;
};

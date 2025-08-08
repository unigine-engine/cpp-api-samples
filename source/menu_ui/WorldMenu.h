#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

struct Sample;

class WorldMenu : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WorldMenu, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	struct UIFont : public Unigine::ComponentStruct
	{
		PROP_PARAM(File, font);
		PROP_PARAM(Int, font_size);
		PROP_PARAM(Color, font_color, Unigine::Math::vec4_white);
	};

	struct UIIcon : public Unigine::ComponentStruct
	{
		PROP_PARAM(File, icon);
		PROP_PARAM(Color, icon_color, Unigine::Math::vec4_white);
		PROP_PARAM(IVec2, icon_size);
	};

	struct UITooltip : public Unigine::ComponentStruct
	{
		PROP_PARAM(File, background);
		PROP_PARAM(Color, background_color);
		PROP_PARAM(Int, height);
		PROP_PARAM(IVec4, padding);
		PROP_PARAM(Float, time_delay);
		PROP_PARAM(IVec2, pos);
		PROP_STRUCT(UIFont, font);
	};

	struct UICommon : public Unigine::ComponentStruct
	{
		PROP_PARAM(IVec4, padding);
		PROP_PARAM(Int, bottom_offset);
		PROP_PARAM(Int, right_offset);
		PROP_PARAM(Int, space_between_hint_and_navigation);
	};

	struct UIHintPanel : public Unigine::ComponentStruct
	{
		PROP_PARAM(Int, space_x, 10);
		PROP_PARAM(File, background);
		PROP_PARAM(Color, background_color);
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_PARAM(Float, background_slice_scale);
		PROP_PARAM(IVec4, background_padding);
		PROP_PARAM(Int, width);
		PROP_PARAM(Int, height);

		PROP_STRUCT(UIIcon, icon);
		PROP_STRUCT(UIFont, label);
	};

	struct UIBackPanel : public Unigine::ComponentStruct
	{
		PROP_PARAM(File, background);
		PROP_PARAM(File, background_hover);
		PROP_PARAM(Color, background_color);
		PROP_PARAM(Color, button_tint_color);
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_PARAM(Float, background_slice_scale);
		PROP_PARAM(IVec4, background_padding);

		PROP_STRUCT(UIIcon, icon);
		PROP_STRUCT(UIFont, label);

		PROP_PARAM(Int, space_between_icon_and_label);
	};

	struct UIButton : public Unigine::ComponentStruct
	{
		PROP_PARAM(File, background);
		PROP_PARAM(File, background_hover);
		PROP_PARAM(Color, background_color, Unigine::Math::vec4_white);
		PROP_PARAM(Color, background_tint_color, Unigine::Math::vec4(0.68f, 0.68f, 0.68f, 1.f));

		PROP_PARAM(Int, button_width);
		PROP_PARAM(Int, button_height);
	};

	struct UINavigationButton : public UIButton
	{
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_PARAM(Float, background_slice_scale);

		PROP_STRUCT(UIIcon, icon);
		PROP_PARAM(String, tooltip_text);
	};

	struct UIWorldButton : public UIButton
	{
		PROP_PARAM(Color, background_selected_color);

		PROP_STRUCT(UIFont, font);
	};

	struct UINavigationPanel : public Unigine::ComponentStruct
	{
		PROP_GROUP("Selector Button");
		PROP_PARAM(File, background);
		PROP_PARAM(File, background_hover);
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_PARAM(Float, background_slice_scale);
		PROP_PARAM(IVec4, background_padding);
		PROP_PARAM(Int, width);
		PROP_PARAM(Int, height);
		PROP_PARAM(Color, background_color, Unigine::Math::vec4_white);
		PROP_PARAM(Color, background_tint_color, Unigine::Math::vec4(0.68f, 0.68f, 0.68f, 1.f));

		PROP_PARAM(Int, space_between_icon_and_label);
		PROP_STRUCT(UIIcon, icon);
		PROP_STRUCT(UIFont, label);
		PROP_GROUP(nullptr);

		PROP_STRUCT(UITooltip, tooltip);

		PROP_STRUCT(UINavigationButton, button_prev);
		PROP_STRUCT(UINavigationButton, button_next);
	};

	struct UISelectorList : public Unigine::ComponentStruct
	{
		PROP_GROUP("Main Widget");
		PROP_PARAM(File, background);
		PROP_PARAM(Vec4, background_slice_offsets);
		PROP_PARAM(Float, background_slice_scale);
		PROP_PARAM(IVec4, background_padding);
		PROP_PARAM(Int, height);
		PROP_GROUP(nullptr);

		PROP_GROUP("Header");
		PROP_PARAM(Int, header_height);
		PROP_PARAM(IVec4, header_padding);
		PROP_STRUCT(UIFont, header_font);
		PROP_GROUP(nullptr);

		PROP_GROUP("Spacer");
		PROP_PARAM(File, spacer_background);
		PROP_PARAM(Int, spacer_height);
		PROP_GROUP(nullptr);

		PROP_GROUP("List");
		PROP_PARAM(Int, space_between_buttons);
		PROP_PARAM(IVec4, list_padding);
		PROP_GROUP(nullptr);
		PROP_STRUCT(UIWorldButton, button);
	};

	struct UIConfiguration : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Main menu world name");
		PROP_PARAM(String, main_menu_world_name);
		PROP_STRUCT(UICommon, common);
		PROP_STRUCT(UIHintPanel, hint);
		PROP_STRUCT(UIBackPanel, back);
		PROP_STRUCT(UINavigationPanel, navigation);
		PROP_STRUCT(UISelectorList, selector);
	};

	PROP_STRUCT(UIConfiguration, ui_configuration);

private:
	void init();
	void update();
	void shutdown();

	void init_back_button();
	void init_hint_box();
	void init_navigation_bar();
	void init_selector();

	void update_back_button(bool up, bool down);
	void update_navigation(bool up, bool down);
	void update_selector();

	bool is_hovered(const Unigine::WidgetPtr &widget);

	class Button
	{
	public:
		Button() {};
		virtual ~Button() {};

		virtual void update(bool up, bool down) = 0;
		bool isHovered() const;
		Unigine::WidgetHBoxPtr &getWidget() { return button_hbox; }

	protected:
		Unigine::WidgetHBoxPtr button_hbox = nullptr;

		Unigine::Math::vec4 background_color;
		Unigine::Math::vec4 background_tint_color;

		bool pressed = false;
		bool prev_hovered = false;
		bool prev_pressed = false;

		Unigine::Input::MOUSE_HANDLE mouse_handle_at_click;
	};

	class NavigationButton : public Button
	{
	public:
		NavigationButton(const Unigine::String &world_path, const Unigine::String &title,
			UINavigationButton &config, UITooltip &tooltip);
		void update(bool up, bool down) override;

	private:
		Unigine::String world_path = "";

		Unigine::String background = "";
		Unigine::String background_hover = "";

		Unigine::WidgetHBoxPtr tooltip_hbox;
		float current_time = 0.0f;
		float time_delay;
		Unigine::Math::ivec2 pos;
	};
	friend class NavigationButton;

	class SelectorButton : public Button
	{
	public:
		SelectorButton(const Unigine::String &sample_name, UINavigationPanel &config,
			std::function<void(bool)> on_clicked);
		void update(bool up, bool down) override;
		void setButtonPressed(bool is_pressed);

	private:
		Unigine::String background = "";
		Unigine::String background_hover = "";

		std::function<void(bool)> on_clicked;
		bool enabled = false;
	};

	class WorldButton : public Button
	{
	public:
		WorldButton(const Unigine::String &text, const Unigine::String &world_path,
			UIWorldButton &config, bool is_current = false);
		void update(bool up, bool down) override;

	private:
		Unigine::String world_path = "";

		Unigine::Math::vec4 background_selected_color;
	};

private:
	struct ActiveSample
	{
		Unigine::String world_name;
		Unigine::String title;
	} current_sample;

	Unigine::WidgetHBoxPtr main_hbox;

	Unigine::WidgetHBoxPtr hint_hbox;

	Unigine::WidgetHBoxPtr back_hbox;
	bool back_prev_pressed = false;
	bool back_prev_hovered = false;
	bool pressed = false;

	Unigine::WidgetHBoxPtr navigation_hbox;
	NavigationButton *nav_prev{nullptr};
	NavigationButton *nav_next{nullptr};
	SelectorButton *nav_select{nullptr};

	Unigine::WidgetHBoxPtr selector_hbox;
	Unigine::WidgetVBoxPtr selector_vbox;
	Unigine::WidgetScrollBoxPtr selector_scroll;
	Unigine::Vector<WorldButton *> select_buttons;
	bool is_selector_hovered = false;

	Unigine::Input::MOUSE_HANDLE mouse_handle_at_click = Unigine::Input::MOUSE_HANDLE_USER;

	static bool is_selection_active;
};

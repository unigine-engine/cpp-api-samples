#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include <memory>

class WidgetSearchField;
class WidgetTagCloud;

struct Sample;
struct Category;
class WidgetSampleList;
class Tag;

class MainMenu: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MainMenu, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	struct UICommon : public Unigine::ComponentStruct
	{
		PROP_GROUP("Main");
		PROP_TOOLTIP("Default white image, to use it as background texture");
		PROP_PARAM(File, white_image);
		PROP_TOOLTIP("Horizontal spacing between widgets inside panel");
		PROP_PARAM(Int, min_window_height, 400); 
		PROP_GROUP(nullptr);
	};

	struct UIControlPanel : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Space between buttons");
		PROP_PARAM(Int, control_panel_space_x, 10);
		PROP_GROUP("Close Button");
		PROP_TOOLTIP("Width of the close button");
		PROP_PARAM(Int, close_button_width);
		PROP_TOOLTIP("Height of the close button");
		PROP_PARAM(Int, close_button_height);
		PROP_TOOLTIP("Texture for the background of the close button");
		PROP_PARAM(File, close_button_background);
		PROP_TOOLTIP("Color of the background of the close button");
		PROP_PARAM(Color, close_button_background_color);
		PROP_TOOLTIP("Texture for the icon of the close button (must be the same size as the background texture)");
		PROP_PARAM(String, close_button_text);
		PROP_TOOLTIP("Color for the tint effect when button pressed");
		PROP_PARAM(Color, close_button_tint_color);
		PROP_TOOLTIP("Font for close button");
		PROP_PARAM(File, close_font);
		PROP_TOOLTIP("Font size for close button label");
		PROP_PARAM(Int, close_font_size);
		PROP_TOOLTIP("Color of close label");
		PROP_PARAM(Color, close_font_color);

		PROP_GROUP("Collapse All Button");
		PROP_TOOLTIP("Size of the Collaps button (button shape square)");
		PROP_PARAM(Int, collaps_button_size);
		PROP_TOOLTIP("Texture for the icon of the Collapse button (must be the same size as the background texture)");
		PROP_PARAM(File, collaps_button_icon);
		PROP_TOOLTIP("Texture for the icon of the Collapse button (must be the same size as the background texture)");
		PROP_PARAM(File, collaps_button_hovered_icon);

		PROP_GROUP("Expand All Button");
		PROP_TOOLTIP("Size of the Expand button (button shape square)");
		PROP_PARAM(Int, expand_button_size);
		PROP_TOOLTIP("Texture for the icon of the Expand button (must be the same size as the background texture)");
		PROP_PARAM(File, expand_button_icon);
		PROP_TOOLTIP("Texture for the icon of the Expand button (must be the same size as the background texture)");
		PROP_PARAM(File, expand_button_hovered_icon);
		PROP_GROUP(nullptr);

	};

	struct UISerachField : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Search field background texture");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("Color of the background of a tag");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("Tag background texture slice offsets for 9 sliced");
		PROP_PARAM(Vec4, background_offsets);
		PROP_TOOLTIP("Tag background texture slice scale");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("Color of the selection of the search field");
		PROP_PARAM(Color, selection_color);
		PROP_TOOLTIP("Height of the search field");
		PROP_PARAM(Int, height, 50);
		PROP_TOOLTIP("Padding from text field of the search field (l, r, t, b)");
		PROP_PARAM(IVec4, padding);
		PROP_TOOLTIP("Texture for the search icon");
		PROP_PARAM(File, icon);
		PROP_TOOLTIP("Size of the search icon (icon shape square)");
		PROP_PARAM(Int, icon_size);
		PROP_TOOLTIP("Color of the search icon");
		PROP_PARAM(Color, icon_color);
		PROP_TOOLTIP("Font for the search field");
		PROP_PARAM(File, font);
		PROP_TOOLTIP("Color of the search field font");
		PROP_PARAM(Color, font_color);
		PROP_TOOLTIP("Size  of the search field font");
		PROP_PARAM(Int, font_size,10);
		PROP_TOOLTIP("Horizontal space between tags in row");
		PROP_PARAM(Int, space_between_tags_in_row);
		PROP_TOOLTIP("Vertical space between rows of tags");
		PROP_PARAM(Int, space_between_rows_of_tags);
	};

	struct UISampleListCategory : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Vertical spacing between samples in block");
		PROP_PARAM(Int, vertical_spacing);
		PROP_TOOLTIP("Samples list category background texture");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("Color of the background of the sample list category");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("Samples list category background texture slice offsets for 9 sliced");
		PROP_PARAM(Vec4, background_offsets);
		PROP_TOOLTIP("Samples list category background texture slice scale");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("Samples list category background padding from its content (l, r, t, b)");
		PROP_PARAM(IVec4, background_padding);
		PROP_TOOLTIP("Samples list category icon size (icon shape square)");
		PROP_PARAM(IVec2, icon_size);
		PROP_TOOLTIP("Samples list category arrow icon size");
		PROP_PARAM(Int, arrow_icon_size);
		PROP_TOOLTIP("Samples list category arrow icon texture");
		PROP_PARAM(File, arrow_icon);
		PROP_TOOLTIP("Color of the arrow icon of the sample lit category");
		PROP_PARAM(Color, arrow_icon_color);
		PROP_TOOLTIP("Arrow Icon padding (l, r, t, b)");
		PROP_PARAM(Vec4, arrow_offset);
		PROP_TOOLTIP("Font for sample list category title");
		PROP_PARAM(File, title_font);
		PROP_TOOLTIP("Font size for sample list category title");
		PROP_PARAM(Int, title_font_size);
		PROP_TOOLTIP("Color of the sample list category title");
		PROP_PARAM(Color, title_font_color);
		PROP_TOOLTIP("Offset from left side for sample inside category block");
		PROP_PARAM(Float, title_offset);
		PROP_TOOLTIP("Category title left offset");
	};

	struct UISampleListSample : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Offset from left side for sample in block");
		PROP_PARAM(Int, side_offset);
		PROP_TOOLTIP("Vertical spacing between block in sample content");
		PROP_PARAM(Int, content_vertical_spacing);
		PROP_TOOLTIP("Sample list sample background texture");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("Color of the background of the sample list sample");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("Sample list sample background texture slice offsets for 9 sliced");
		PROP_PARAM(Vec4, background_offsets);
		PROP_TOOLTIP("Sample list sample background texture slice scale");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("Sample list sample background padding from its content (l, r, t, b)");
		PROP_PARAM(IVec4, background_padding);
		PROP_TOOLTIP("Horizontal space between tags in row");
		PROP_PARAM(Int, space_between_tags_in_row);
		PROP_TOOLTIP("Vertical space between rows of tags");
		PROP_PARAM(Int, space_between_rows_of_tags);
		PROP_TOOLTIP("Font for sample list sample title");
		PROP_PARAM(File, title_font);
		PROP_TOOLTIP("Font size for sample list sample title");
		PROP_PARAM(Int, title_font_size);
		PROP_TOOLTIP("Color of the sample list sample title");
		PROP_PARAM(Color, title_font_color);
		PROP_TOOLTIP("Font for sample list sample description");
		PROP_PARAM(File, description_font);
		PROP_TOOLTIP("Font size for sample list sample description");
		PROP_PARAM(Int, description_font_size);
		PROP_TOOLTIP("Color of the sample list category description");
		PROP_PARAM(Color, description_font_color);
		PROP_TOOLTIP("Font VSpacing for sample list sample description");
		PROP_PARAM(Int, description_font_vspacing);
	};

	struct UISampleList : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Color of the bottom left panel with list of sample");
		PROP_PARAM(Color, sample_list_background_color);
		PROP_TOOLTIP("Minimal width for the bottom panel");
		PROP_PARAM(Int, sample_list_min_width, 800, nullptr, "");
		PROP_TOOLTIP("Vertical spacing between categories in sample list");
		PROP_PARAM(Int, vertical_spacing);
		PROP_TOOLTIP("Samples list padding from its content (l, r, t, b)");
		PROP_PARAM(IVec4, padding);
		PROP_TOOLTIP("Color for the tint effect when category or sample panel pressed");
		PROP_PARAM(Color, tint_color);
		
		PROP_STRUCT(UISampleListCategory, category);
		PROP_STRUCT(UISampleListSample, sample);
	};

	struct UITagCloud : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Minimal horizontal space between tags in row");
		PROP_PARAM(Int, space_between_tags_in_row);
		PROP_TOOLTIP("Vertical space between rows of tags in tag cloud");
		PROP_PARAM(Int, space_between_rows);
		PROP_TOOLTIP("Font for tag title");
		PROP_PARAM(File, font);
		PROP_TOOLTIP("Font size for tag title");
		PROP_PARAM(Int, font_size);
		PROP_TOOLTIP("Color of tag label");
		PROP_PARAM(Color, font_color);
	};
	
	struct UISearchSection : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Search Section background texture");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("Search Section background texture slice offsets for 9 sliced");
		PROP_PARAM(Vec4, background_offsets);
		PROP_TOOLTIP("Search Section background texture slice scale");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("Color of the Search Section");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("Padding from the Search Section content (l, r, t, b)");
		PROP_PARAM(IVec4, padding);
		PROP_TOOLTIP("Search Section width");
		PROP_PARAM(Int, width, 350);
		PROP_TOOLTIP("Search field size relative to full size of Search Section");
		PROP_PARAM(Float, search_field_releative_size, 0.3f);
		PROP_TOOLTIP("Space between serch field and tag cloud");
		PROP_PARAM(Int, space_between_child_sections, 30);

		PROP_STRUCT(UIControlPanel, control_panel);
		PROP_STRUCT(UISerachField, search_field);
		PROP_STRUCT(UITagCloud, tag_cloud);
	};

	struct UITagStyle : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Tag background texture");
		PROP_PARAM(File, background);
		PROP_TOOLTIP("Selected tag background texture");
		PROP_PARAM(File, selected_background);
		PROP_TOOLTIP("Color of the background of a tag");
		PROP_PARAM(Color, background_color);
		PROP_TOOLTIP("Tag background texture slice offsets for 9 sliced");
		PROP_PARAM(Vec4, background_offsets);
		PROP_TOOLTIP("Tag background texture slice scale");
		PROP_PARAM(Float, background_slice_scale);
		PROP_TOOLTIP("Tag background padding from its content (l, r, t, b)");
		PROP_PARAM(IVec4, background_padding);
		PROP_TOOLTIP("Font for tag label");
		PROP_PARAM(File, font);
		PROP_TOOLTIP("Font size for tag label");
		PROP_PARAM(File, selected_font);
		PROP_TOOLTIP("Font size for selected tag label");
		PROP_PARAM(Int, font_size);
		PROP_TOOLTIP("Color of tag label");
		PROP_PARAM(Color, font_color);
		PROP_TOOLTIP("Color of selected tag label");
		PROP_PARAM(Color, selected_font_color);
		PROP_TOOLTIP("Color for the tint effect when tag pressed");
		PROP_PARAM(Color, tint_color);
		PROP_TOOLTIP("Size of tag remove button (button shape square)");
		PROP_PARAM(Int, remove_button_size);
		PROP_TOOLTIP("Background texture for remove button in tag");
		PROP_PARAM(File, remove_button_background);
		PROP_TOOLTIP("Color of the background of the remove button in tag");
		PROP_PARAM(Color, remove_button_background_color);
		PROP_TOOLTIP("Texture for the icon of the remove button (must be the same size as the background texture)");
		PROP_PARAM(File, remove_button_icon);
		PROP_TOOLTIP("Color of the icon of the remove button in tag");
		PROP_PARAM(Color, remove_button_icon_color);
		PROP_TOOLTIP("Color for the tint effect when remove button pressed");
		PROP_PARAM(Color, remove_button_tint_color);
		PROP_TOOLTIP("Scale of tag in search field");
		PROP_PARAM(Float, search_field_scale);
		PROP_TOOLTIP("Scale of tag in sample list");
		PROP_PARAM(Float, sample_list_scale);
		PROP_TOOLTIP("Scale of tag in tag cloud");
		PROP_PARAM(Float, tag_cloud_scale);
	};

	struct UIConfiguration : public Unigine::ComponentStruct
	{
		PROP_TOOLTIP("Path to meta file(.sample) relative to data");
		PROP_PARAM(String, path_to_meta);
		PROP_STRUCT(UICommon, common);
		PROP_STRUCT(UISearchSection, search_section);
		PROP_STRUCT(UISampleList, sample_list);
		PROP_STRUCT(UITagStyle, tag_style);
	};

	PROP_STRUCT(UIConfiguration, ui_configuration);

private:
	void init();
	void update();
	void shutdown();
	void save(const Unigine::BlobPtr &blob);
	void restore(const Unigine::BlobPtr &blob);

	void update_close_button();
	void parse_meta_xml(Unigine::String path_relative_to_data, Unigine::Vector<Category> &categories, Unigine::Vector<Unigine::String> &tags);

	void create_side_panel();
	void create_sample_list(const Unigine::Vector<Category> &categories);
	void create_tag_cloud(const Unigine::Vector<Unigine::String> &tags);

private:
	Unigine::WidgetVBoxPtr main_vbox;
	Unigine::WidgetHBoxPtr samples_and_tags_hbox;

	// search section side panel
	Unigine::WidgetVBoxPtr side_panel_vbox;
	std::unique_ptr<WidgetSearchField> search_field = nullptr;
	std::unique_ptr<WidgetTagCloud> tag_cloud = nullptr;

	// side panel buttons
	Unigine::WidgetSpritePtr collaps_button_sprite;
	Unigine::WidgetSpritePtr expand_button_sprite;
	Unigine::WidgetHBoxPtr close_button;
	Unigine::Math::vec4 close_pressed_color;
	Unigine::Math::vec4 close_color;
	bool close_button_pressed = false;

	bool search_editline_focused = false;

	// sample list
	std::unique_ptr<WidgetSampleList> sample_list = nullptr;

	// saved state
	static Unigine::BlobPtr saved_state;
};


class WidgetSearchField
{
public:
	WidgetSearchField(MainMenu::UIConfiguration &config);
	~WidgetSearchField();

	Unigine::WidgetPtr getWidget() const { return main_vbox; }

	void changeTagState(const Unigine::String &str, MainMenu::UITagStyle &config);
	void addTagToSerach(const Unigine::String &str, MainMenu::UITagStyle &config);
	void removeTagFromSearch(const Unigine::String &str);

	Unigine::Event<const Unigine::Vector<Unigine::String> &, const Unigine::Vector<Unigine::String> &> &getEventSearchRequest() { return event_search_request; }
	Unigine::Event<const Unigine::String &> &getEventTagRemoved() { return event_tag_removed; }
	Unigine::Event<const Unigine::String &> &getEventTagAdded() { return event_tag_added; }
	Unigine::Event<bool> &getEventEditlineFocused() { return event_editline_focus; }

	void update();

	void save(const Unigine::BlobPtr &blob);
	void restore(const Unigine::BlobPtr &blob);

private:
	void arrange_tags();
	void run_event_search_request();

	MainMenu::UITagStyle tag_config;
	Unigine::WidgetVBoxPtr main_vbox;
	Unigine::WidgetEditLinePtr editline;
	Unigine::WidgetSpritePtr icon_sprite;

	Unigine::WidgetVBoxPtr tags_vbox;
	Unigine::WidgetScrollBoxPtr tags_scrollbox;
	Unigine::Vector<Unigine::WidgetPtr> tag_widgets_ordered;
	Unigine::HashMap<Unigine::String, Unigine::WidgetPtr> tag_widgets;
	int space_between_tags_in_row = 0;

	Unigine::EventConnections editline_callback_connections;
	Unigine::EventConnections tag_remove_button_connections;

	// lower case words and tags parsed from edit text
	Unigine::EventInvoker<const Unigine::Vector<Unigine::String> &, const Unigine::Vector<Unigine::String> &> event_search_request;
	Unigine::EventInvoker<const Unigine::String &> event_tag_removed;
	Unigine::EventInvoker<const Unigine::String &> event_tag_added;
	Unigine::EventInvoker<bool> event_editline_focus;
	bool pressed = false;
};

class WidgetTagCloud
{
public:
	WidgetTagCloud(const Unigine::Vector<Unigine::String> &tags, MainMenu::UIConfiguration &config);
	~WidgetTagCloud();

	Unigine::WidgetPtr getWidget() const { return main_vbox; }

	void update(bool enable_focus);
	void setTagSelected(const Unigine::String &str, bool selected = true);

	Unigine::Event<const Unigine::String &> &getEventTagClicked() { return event_tag_clicked; }

private:
	Unigine::WidgetScrollBoxPtr main_scrollbox;
	Unigine::WidgetVBoxPtr main_vbox;

	Unigine::HashMap<Unigine::String, bool> pressed_tags;
	Unigine::HashMap<Unigine::String, Tag*> tag_widgets;

	// tag name
	Unigine::EventInvoker<const Unigine::String &> event_tag_clicked;
};

struct Sample
{
	Unigine::String title;
	Unigine::String description;
	Unigine::Vector<Unigine::String> tags;
	Unigine::String world_name;
};

struct Category
{
	Unigine::ImagePtr icon;
	Unigine::String title;

	Unigine::Vector<Sample> samples;
};

class WidgetSampleListNode
{
public:
	WidgetSampleListNode(MainMenu::UIConfiguration &config);
	virtual ~WidgetSampleListNode();

	void addChild(WidgetSampleListNode *node);
	int getNumChildren() const { return children.size(); }
	WidgetSampleListNode *getChild(int index) const { return children[index]; }

	Unigine::WidgetPtr getWidget() const { return main_vbox; }
	virtual const Unigine::String& getTitle() const = 0;

	virtual void getSearchInfo(Unigine::String &main_info, Unigine::Vector<Unigine::String> &tags) const = 0;

	void setHidden(bool hide);
	bool isHidden() const { return is_hidden; }

	virtual void setChildrenHidden(bool hide);
	bool isChildrenHidden() const { return is_children_hidden; }

	virtual void update();
	virtual void setTagSelected(const Unigine::String &str, bool selected = true) {};

protected:
	virtual void on_clicked() = 0;

	void show_press_effect(bool show);

	Unigine::WidgetVBoxPtr main_vbox;
	Unigine::WidgetHBoxPtr header_hbox;
	Unigine::WidgetVBoxPtr children_container_vbox;

	bool is_hidden = false;
	bool is_children_hidden = false;

	WidgetSampleListNode *parent = nullptr;
	Unigine::Vector<WidgetSampleListNode *> children;

	bool pressed = false;
	Unigine::Math::vec4 tint_color;
};

class WidgetSample : public WidgetSampleListNode
{
public:
	WidgetSample(const Sample &sample, MainMenu::UIConfiguration &config);
	~WidgetSample() override;

	const Unigine::String &getTitle() const override { return sample_info.title; }
	void getSearchInfo(Unigine::String &main_info, Unigine::Vector<Unigine::String> &tags) const override;
	void setTagSelected(const Unigine::String &str, bool selected = true) override;

	void update() override;

private:
	void on_clicked() override;

	Unigine::WidgetVBoxPtr header_vbox;

	Unigine::WidgetLabelPtr title_label;
	Unigine::WidgetLabelPtr description_label;

	Unigine::WidgetVBoxPtr tags_vbox;
	Unigine::HashMap<Unigine::String, Tag *> tag_widgets;

	Sample sample_info;
	Unigine::String search_info;
};

class WidgetCategory : public WidgetSampleListNode
{
public:
	WidgetCategory(const Category &category, MainMenu::UIConfiguration &config);
	~WidgetCategory() override;

	const Unigine::String& getTitle() const override { return category_info.title; }
	void getSearchInfo(Unigine::String &main_info, Unigine::Vector<Unigine::String> &tags) const override;
	void setTagSelected(const Unigine::String &str, bool selected = true) override;

	void setChildrenHidden(bool hide) override;

private:
	void on_clicked() override;

	bool was_pressed = false;

	Unigine::WidgetSpritePtr icon_sprite;
	Unigine::WidgetLabelPtr title_label;
	Unigine::WidgetSpritePtr arrow_sprite;

	Category category_info;
};

class WidgetSampleList
{
public:
	WidgetSampleList(const Unigine::Vector<Category> &categories, MainMenu::UIConfiguration &config);
	~WidgetSampleList();

	Unigine::WidgetPtr getWidget() const { return main_scrollbox; }
	void setTagSelected(const Unigine::String &str, bool selected = true);

	void filter(const Unigine::Vector<Unigine::String> &search_words, const Unigine::Vector<Unigine::String> &search_tags);

	void update(bool enable_focus);

	void setCollapseAll(bool collapse = true);

	void save(const Unigine::BlobPtr &blob);
	void restore(const Unigine::BlobPtr &blob);

private:
	void filter_node(WidgetSampleListNode *node, const Unigine::Vector<Unigine::String> &search_words, const Unigine::Vector<Unigine::String> &search_tags);

	Unigine::Vector<WidgetSampleListNode *> roots;

	Unigine::WidgetScrollBoxPtr main_scrollbox;
};

class MenuUtils
{
public:
	enum TAG_PLACE
	{
		SEARCH_FIELD,
		SAMPLES_LIST,
		TAGS_CLOUD
	};

	static Unigine::WidgetHBoxPtr createTagWidget(const Unigine::String &text, const MainMenu::UITagStyle &config, TAG_PLACE place);

	static bool isWordSuitable(const Unigine::String &text, const Unigine::String &word);

	static bool isHovered(const Unigine::WidgetPtr widget);
};

class Tag
{
public:
	Tag(const Unigine::String &text, MenuUtils::TAG_PLACE place);
	~Tag() { tag_widget.deleteLater(); }

	Unigine::WidgetHBoxPtr getWidget() const { return tag_widget; }
	Unigine::String getText() const { return text; }

	void setTagSelected(bool selected = true);
	void update(bool up, bool down);

	static Unigine::Event<const Unigine::String &> &getEventClicked() { return event_clicked; }
	static void setTagConfig(const MainMenu::UITagStyle &tag_config) { config = tag_config; }

private:
	const Unigine::String text;
	Unigine::WidgetHBoxPtr tag_widget;
	Unigine::WidgetLabelPtr label = nullptr;
	bool is_selected = false;
	bool is_pressed = false;

	static MainMenu::UITagStyle config;
	static Unigine::EventInvoker<const Unigine::String &> event_clicked;
};

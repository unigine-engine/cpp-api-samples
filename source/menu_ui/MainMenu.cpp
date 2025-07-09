#include "MainMenu.h"

#include <UnigineXml.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(MainMenu);

using namespace Unigine;
using namespace Math;


BlobPtr MainMenu::saved_state = Blob::create();

void MainMenu::init()
{
	// parse meta
	Vector<Category> categories;
	Vector<String> tags;
	parse_meta_xml(ui_configuration->path_to_meta.get(), categories, tags);

	Tag::setTagConfig(ui_configuration->tag_style.get());

	// samples list
	samples_and_tags_hbox = WidgetHBox::create();

	create_sample_list(categories);
	samples_and_tags_hbox->addChild(sample_list->getWidget(), Gui::ALIGN_LEFT | Gui::ALIGN_EXPAND);

	create_side_panel();
	samples_and_tags_hbox->addChild(side_panel_vbox, Gui::ALIGN_LEFT);

	// create search field
	search_field = std::make_unique<WidgetSearchField>(ui_configuration.get());
	side_panel_vbox->addChild(search_field->getWidget(), Gui::ALIGN_TOP);

	// tags area
	create_tag_cloud(tags);
	side_panel_vbox->addChild(tag_cloud->getWidget(), Gui::ALIGN_EXPAND);

	// setup main window
	auto main_window = WindowManager::getMainWindow();
	main_window->addChild(samples_and_tags_hbox, Gui::ALIGN_EXPAND);
	main_window->setMinSize(ivec2(ui_configuration->sample_list->sample_list_min_width.get()
			+ ui_configuration->search_section->width,
		ui_configuration->common->min_window_height));

	// setup callbacks
	search_field->getEventSearchRequest().connect(*this, sample_list.get(), &WidgetSampleList::filter);
	search_field->getEventTagRemoved().connect(*this, [this](const String &str) {
		tag_cloud->setTagSelected(str, false);
		sample_list->setTagSelected(str, false);
	});
	search_field->getEventTagAdded().connect(*this, [this](const String &str) {
		tag_cloud->setTagSelected(str, true);
		sample_list->setTagSelected(str, true);
	});
	search_field->getEventEditlineFocused().connect(*this,
		[this](bool focused) { search_editline_focused = focused; });
	Tag::getEventClicked().connect(*this, [this](const String &str) {
		search_field->changeTagState(str, ui_configuration->tag_style.get());
	});
	expand_button_sprite->getEventClicked().connect(*this, [this]() { sample_list->setCollapseAll(false); });
	collaps_button_sprite->getEventClicked().connect(*this, [this]() { sample_list->setCollapseAll(true); });

	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	if (saved_state.isValid())
		restore(saved_state);
}

void MainMenu::update()
{
	EngineWindowViewportPtr window = WindowManager::getMainWindow();
	int window_height = window->getClientSize().y;

	tag_cloud->update(!search_editline_focused);
	sample_list->update(!search_editline_focused);
	search_field->update();

	side_panel_vbox->setHeight(window_height);
	search_field->getWidget()->setHeight(side_panel_vbox->getHeight()
		* ui_configuration->search_section->search_field_releative_size.get());

	if (close_button->isHidden() == window->isFullscreen())
	{
		close_button->setHidden(!window->isFullscreen());
		close_button->setEnabled(!close_button->isHidden());
	}
	if (!close_button->isHidden())
		update_close_button();
}

void MainMenu::shutdown()
{
	save(saved_state);
	samples_and_tags_hbox.deleteLater();
}

void MainMenu::save(const Unigine::BlobPtr &blob)
{
	blob->clear();
	search_field->save(blob);
	sample_list->save(blob);
}

void MainMenu::restore(const Unigine::BlobPtr &blob)
{
	if (blob->getSize() <= 0)
		return;

	blob->seekSet(0);
	search_field->restore(blob);
	sample_list->restore(blob);
}

void MainMenu::update_close_button()
{
	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT);

	bool hovered = MenuUtils::isHovered(close_button);

	if (!hovered && !up)
		return;

	if (close_button_pressed && up)
	{
		close_button_pressed = false;
		if (hovered)
		{
			Engine::get()->quit();
		}
	}

	if (!close_button_pressed && down && hovered)
	{
		close_button_pressed = true;
	}

	close_button->setBackgroundColor(close_button_pressed ? close_pressed_color : close_color);
}

void MainMenu::parse_meta_xml(String path_relative_to_data, Vector<Category> &categories, Vector<String> &tags)
{
	String cpp_samples_xml_path = FileSystem::getAbsolutePath(String::joinPaths(Engine::get()->getDataPath(), path_relative_to_data));

	XmlPtr cpp_samples_xml = Xml::create();
	if (!cpp_samples_xml->load(cpp_samples_xml_path))
	{
		Unigine::Log::warning("MainMenu::parse_meta_xml(): cannot open %s file\n", cpp_samples_xml_path.get());
		return;
	}

	XmlPtr cpp_samples_samples_pack = cpp_samples_xml->getChild("samples_pack");
	XmlPtr categories_xml = cpp_samples_samples_pack->getChild("categories");
	XmlPtr samples_xml = cpp_samples_samples_pack->getChild("samples");

	HashMap<String, Category> categories_map;
	Vector<String> categories_id;
	HashSet<String> tags_set;

	for (int i = 0; i < categories_xml->getNumChildren(); ++i)
	{
		XmlPtr category_xml = categories_xml->getChild(i);

		String icon_path = category_xml->getArg("img");
		icon_path = icon_path.trimFirst("data/");

		Category c;
		c.icon = Image::create(icon_path.get());
		c.title = category_xml->getArg("name");

		String category_id = category_xml->getArg("id");

		categories_id.append(category_id);
		categories_map[category_id] = c;
	}

	for (int i = 0; i < samples_xml->getNumChildren(); ++i)
	{
		XmlPtr sample_xml = samples_xml->getChild(i);

		Sample s;
		s.title = sample_xml->getArg("title");
		s.description = sample_xml->getChild("sdk_desc")->getData();
		s.world_name = sample_xml->getArg("id");

		XmlPtr tags_xml = sample_xml->getChild("tags");
		for (int j = 0; j < tags_xml->getNumChildren(); ++j)
		{
			String tag = tags_xml->getChild(j)->getData();

			if (!tags_set.contains(tag))
				tags_set.insert(tag);

			s.tags.push_back(tag);
		}

		String category_id = sample_xml->getArg("category_id");

		if (categories_map.contains(category_id))
			categories_map[category_id].samples.push_back(s);
		else
			Unigine::Log::error("Category with id %s don't exists in .sample file\n", category_id.get());
	}

	categories.clear();

	for (const auto &id : categories_id)
	{
		if (categories_map.contains(id) && categories_map.value(id).samples.size() > 0)
			categories.push_back(categories_map.value(id));
	}
	categories_id.clear();

	tags.clear();
	for (const auto &t : tags_set)
	{
		tags.push_back(t.key);
	}

}

void MainMenu::create_side_panel()
{
	int space_y = ui_configuration->search_section->space_between_child_sections;
	side_panel_vbox = WidgetVBox::create(0, space_y);
	side_panel_vbox->setBackground(1);
	side_panel_vbox->setBackgroundTexture(ui_configuration->search_section->background.get());
	side_panel_vbox->setBackgroundColor(ui_configuration->search_section->background_color.get());
	side_panel_vbox->setBackground9Sliced(true);
	vec4 offsets = ui_configuration->search_section->background_offsets.get();
	side_panel_vbox->setBackground9SliceOffsets(offsets.x, offsets.y, offsets.z, offsets.w);
	side_panel_vbox->setBackground9SliceScale(ui_configuration->search_section->background_slice_scale.get());
	side_panel_vbox->setWidth(ui_configuration->search_section->width);
	ivec4 padding = ui_configuration->search_section->padding.get();
	side_panel_vbox->setPadding(padding.x, padding.y, padding.z - space_y, padding.w - space_y);

	// create control buttons box
	auto& control_config = ui_configuration->search_section->control_panel;
	int space_x = control_config->control_panel_space_x;
	auto control_hbox = WidgetHBox::create(space_x, 0);
	control_hbox->setPadding(-space_x, -space_x, 0, 0);
	control_hbox->setWidth(ui_configuration->search_section->width - padding.x - padding.y);
	control_hbox->setHeight(control_config->close_button_height.get());
	side_panel_vbox->addChild(control_hbox, Gui::ALIGN_TOP);

	// create expand button
	expand_button_sprite = WidgetSprite::create();
	expand_button_sprite->setTexture(control_config->expand_button_icon.get());
	expand_button_sprite->setWidth(control_config->expand_button_size.get());
	expand_button_sprite->setHeight(control_config->expand_button_size.get());
	expand_button_sprite->setToolTip("Expand all categories");

	expand_button_sprite->getEventEnter().connect(*this, [this]() { expand_button_sprite->setTexture(ui_configuration->search_section->control_panel->expand_button_hovered_icon.get()); });
	expand_button_sprite->getEventLeave().connect(*this, [this]() { expand_button_sprite->setTexture(ui_configuration->search_section->control_panel->expand_button_icon.get()); });
	control_hbox->addChild(expand_button_sprite, Gui::ALIGN_LEFT);

	// create collapse button
	collaps_button_sprite = WidgetSprite::create();
	collaps_button_sprite->setTexture(control_config->collaps_button_icon.get());
	collaps_button_sprite->setWidth(control_config->collaps_button_size.get());
	collaps_button_sprite->setHeight(control_config->collaps_button_size.get());
	collaps_button_sprite->setToolTip("Collapse all categories");

	collaps_button_sprite->getEventEnter().connect(*this, [this]() { collaps_button_sprite->setTexture(ui_configuration->search_section->control_panel->collaps_button_hovered_icon.get()); });
	collaps_button_sprite->getEventLeave().connect(*this, [this]() { collaps_button_sprite->setTexture(ui_configuration->search_section->control_panel->collaps_button_icon.get()); });
	control_hbox->addChild(collaps_button_sprite, Gui::ALIGN_LEFT);

	// create close button
	close_color = control_config->close_button_background_color.get();
	close_pressed_color = close_color * control_config->close_button_tint_color.get();

	auto close_label = WidgetLabel::create(control_config->close_button_text);
	close_label->setFont(control_config->close_font.get());
	close_label->setFontSize(control_config->close_font_size);
	close_label->setFontColor(control_config->close_font_color);

	close_button = WidgetHBox::create();
	close_button->setBackground(1);
	close_button->setBackgroundTexture(control_config->close_button_background.get());
	close_button->setBackgroundColor(control_config->close_button_background_color.get());
	close_button->setWidth(control_config->close_button_width.get());
	close_button->setHeight(control_config->close_button_height.get());

	close_button->addChild(close_label, Gui::ALIGN_CENTER);
	control_hbox->addChild(close_button, Gui::ALIGN_RIGHT | Gui::ALIGN_TOP);
}

void MainMenu::create_sample_list(const Vector<Category> &categories)
{
	sample_list = std::make_unique<WidgetSampleList>(categories, ui_configuration.get());
	sample_list->getWidget()->arrange();
}

void MainMenu::create_tag_cloud(const Vector<String> &tags)
{
	tag_cloud = std::make_unique<WidgetTagCloud>(tags, ui_configuration.get());
	tag_cloud->getWidget()->arrange();
}

WidgetSearchField::WidgetSearchField(MainMenu::UIConfiguration &config)
{
	tag_config = config.tag_style.get();

	auto &search_field_params = config.search_section->search_field;
	space_between_tags_in_row = search_field_params->space_between_tags_in_row.get();

	auto auxiliary_hbox = WidgetHBox::create(0, 0);
	auxiliary_hbox->setBackground(1);
	auxiliary_hbox->setBackgroundTexture(search_field_params->background.get());
	auxiliary_hbox->setBackgroundColor(search_field_params->background_color.get());
	auxiliary_hbox->setBackground9Sliced(true);
	vec4 offsets = search_field_params->background_offsets.get();
	auxiliary_hbox->setBackground9SliceOffsets(offsets.x, offsets.y, offsets.z, offsets.w);
	auxiliary_hbox->setBackground9SliceScale(search_field_params->background_slice_scale.get());

	ivec4 padding = search_field_params->padding.get();
	auxiliary_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	editline = WidgetEditLine::create();
	editline->setHeight(search_field_params->height.get() - padding.z - padding.w - 5);
	editline->setBorderColor(vec4(0.0f, 0.0f, 0.0f, 0.0f));
	editline->setBackground(0);
	editline->setSelectionColor(search_field_params->selection_color.get());
	editline->setFont(search_field_params->font.get());
	editline->setFontSize(search_field_params->font_size.get());
	editline->setFontColor(search_field_params->font_color.get());

	auxiliary_hbox->addChild(editline, Gui::ALIGN_EXPAND);

	icon_sprite = WidgetSprite::create();
	icon_sprite->setTexture(search_field_params->icon.get());
	icon_sprite->setWidth(search_field_params->icon_size.get());
	icon_sprite->setHeight(search_field_params->icon_size.get());
	icon_sprite->setColor(search_field_params->icon_color.get());

	auxiliary_hbox->addChild(icon_sprite, Gui::ALIGN_RIGHT);
	auxiliary_hbox->setHeight(search_field_params->height.get());
	padding = config.search_section->padding.get();
	auxiliary_hbox->setWidth(config.search_section->width - padding.x - padding.y);

	int space_y = search_field_params->space_between_rows_of_tags.get();
	main_vbox = WidgetVBox::create(0, space_y);
	main_vbox->addChild(auxiliary_hbox, Gui::ALIGN_TOP);
	main_vbox->setPadding(0, 0, -space_y, -space_y);

	tags_scrollbox = WidgetScrollBox::create(0,
		search_field_params->space_between_rows_of_tags.get());
	tags_scrollbox->setHScrollEnabled(false);
	tags_scrollbox->setVScrollHidden(WidgetScrollBox::SCROLL_RENDER_MODE::ALWAYS_HIDE_NO_BOUNDS);
	tags_scrollbox->setBorder(0);
	tags_scrollbox->setPadding(0, 0, -space_y, -space_y);
	main_vbox->addChild(tags_scrollbox, Gui::ALIGN_EXPAND);
	main_vbox->arrange();
	tags_scrollbox->setWidth(main_vbox->getWidth());

	editline->getEventFocusOut().connect(editline_callback_connections, this, &WidgetSearchField::run_event_search_request);
	editline->getEventFocusOut().connect(editline_callback_connections,
		[this]() { event_editline_focus.run(false); });
	editline->getEventFocusIn().connect(editline_callback_connections,
		[this]() { event_editline_focus.run(true); });
	editline->getEventChanged().connect(editline_callback_connections, this, &WidgetSearchField::run_event_search_request);
}

WidgetSearchField::~WidgetSearchField()
{
	main_vbox.deleteLater();
	editline_callback_connections.disconnectAll();
	tag_remove_button_connections.disconnectAll();
}

void WidgetSearchField::changeTagState(const Unigine::String &str, MainMenu::UITagStyle &config)
{
	if (tag_widgets.contains(str))
		removeTagFromSearch(str);
	else
		addTagToSerach(str, config);
}

void WidgetSearchField::addTagToSerach(const Unigine::String &str, MainMenu::UITagStyle &config)
{
	if (tag_widgets.contains(str))
		return;

	WidgetPtr tag = MenuUtils::createTagWidget(str, config, MenuUtils::SEARCH_FIELD);

	tag_widgets_ordered.push_back(tag);
	tag_widgets[str] = tag;

	for (int i = 0; i < tag->getNumChildren(); ++i)
	{
		WidgetSpritePtr sprite = checked_ptr_cast<WidgetSprite>(tag->getChild(i));

		if (sprite)
		{
			sprite->getEventPressed().connect(tag_remove_button_connections, [this, sprite, &config]() {
				pressed = true;
				sprite->setLayerColor(1,
					config.remove_button_icon_color.get() * config.remove_button_tint_color.get());
			});
			sprite->getEventReleased().connect(tag_remove_button_connections, [this, sprite, &config]() {
				pressed = false;
				sprite->setLayerColor(1, config.remove_button_icon_color.get());
			});
			sprite->getEventClicked().connect(tag_remove_button_connections,
				[this, str]() { removeTagFromSearch(str); });

			break;
		}
	}

	event_tag_added.run(str);
	arrange_tags();
	run_event_search_request();
}

void WidgetSearchField::removeTagFromSearch(const Unigine::String &str)
{
	auto it = tag_widgets.find(str);
	if (it == tag_widgets.end())
		return;

	auto it_ordered = tag_widgets_ordered.find((*it).data);
	tag_widgets_ordered.remove(it_ordered);
	tag_widgets.remove(it);

	event_tag_removed.run(str);
	arrange_tags();
	run_event_search_request();
}

void WidgetSearchField::update()
{
	if (pressed)
		return;

	if (editline->isFocused()
		&& (Input::isKeyDown(Input::KEY_ESC) || Input::isKeyDown(Input::KEY_ENTER)))
		editline->removeFocus();

	if (!editline->isFocused() && MenuUtils::isHovered(tags_scrollbox))
		tags_scrollbox->setFocus();
}

void WidgetSearchField::save(const Unigine::BlobPtr &blob)
{
	blob->writeInt(tag_widgets.size());
	for (const auto &it : tag_widgets)
		blob->writeString(it.key);

	blob->writeString(editline->getText());
}

void WidgetSearchField::restore(const Unigine::BlobPtr &blob)
{
	event_search_request.setEnabled(false);

	int tags_size = blob->readInt();
	for (int i = 0; i < tags_size; i++)
		addTagToSerach(blob->readString(), tag_config);

	editline->getEventChanged().setEnabled(false);
	editline->setText(blob->readString());
	editline->getEventChanged().setEnabled(true);

	event_search_request.setEnabled(true);
	run_event_search_request();
}

void WidgetSearchField::arrange_tags()
{
	for (int i = tags_scrollbox->getNumChildren(); i > 0; --i)
	{
		auto child = tags_scrollbox->getChild(i - 1);
		tags_scrollbox->removeChild(child);
		child.deleteLater();
	}

	auto current_tags_hbox = WidgetHBox::create(space_between_tags_in_row);
	current_tags_hbox->setPadding(-space_between_tags_in_row, -space_between_tags_in_row, 0, 0);
	tags_scrollbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);

	for (int i = 0; i < tag_widgets_ordered.size(); ++i)
	{
		WidgetPtr tag = tag_widgets_ordered[i];

		current_tags_hbox->arrange();
		tag->arrange();
		int current_width = current_tags_hbox->getWidth() + tag->getWidth()
			+ space_between_tags_in_row;
		if (current_width < tags_scrollbox->getWidth())
		{
			current_tags_hbox->addChild(tag);
		}
		else
		{
			current_tags_hbox = WidgetHBox::create(space_between_tags_in_row);
			current_tags_hbox->setPadding(-space_between_tags_in_row, -space_between_tags_in_row, 0,
				0);
			tags_scrollbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);
			current_tags_hbox->addChild(tag);
		}
	}
	current_tags_hbox->arrange();
}

void WidgetSearchField::run_event_search_request()
{
	String str = editline->getText();

	Vector<String> words;
	String buffer = "";
	for (int i = 0; i < str.size(); ++i)
	{
		char c = String::toLower(str[i]);
		if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
		{
			buffer += c;
		}
		else
		{
			if (!buffer.empty())
				words.push_back(buffer);
			buffer.clear();
		}
	}

	if (!buffer.empty())
		words.push_back(buffer);


	Vector<String> tags;
	for (const auto &it : tag_widgets)
	{
		String lower_tag = it.key;
		tags.push_back(lower_tag.lower());
	}

	event_search_request.run(words, tags);
}

WidgetTagCloud::WidgetTagCloud(const Vector<String> &tags, MainMenu::UIConfiguration &config)
{
	auto &tag_cloud_params = config.search_section->tag_cloud;
	main_vbox = WidgetVBox::create();

	auto title = WidgetLabel::create("TAGS");
	title->setFont(tag_cloud_params->font);
	title->setFontSize(tag_cloud_params->font_size);
	title->setFontColor(tag_cloud_params->font_color.get());
	main_vbox->addChild(title, Gui::ALIGN_LEFT);

	main_scrollbox = WidgetScrollBox::create(0, tag_cloud_params->space_between_rows.get());
	main_scrollbox->setHScrollEnabled(false);
	main_scrollbox->setVScrollHidden(WidgetScrollBox::SCROLL_RENDER_MODE::ALWAYS_HIDE_NO_BOUNDS);
	main_scrollbox->setBorder(0);
	main_vbox->addChild(main_scrollbox, Gui::ALIGN_EXPAND);

	ivec4 padding = config.search_section->padding.get();
	int max_tags_hbox_size = config.search_section->width.get() - padding.x - padding.y;
	
	int x_space = tag_cloud_params->space_between_tags_in_row.get();
	auto current_tags_hbox = WidgetHBox::create(x_space);
	current_tags_hbox->setPadding(-x_space, -x_space, 0, 0);
	main_scrollbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);

	for (int i = 0; i < tags.size(); ++i)
	{
		auto tag = new Tag(tags[i], MenuUtils::TAGS_CLOUD);
		tag_widgets[tags[i]] = tag;
		pressed_tags[tags[i]] = false;

		main_scrollbox->arrange();
		current_tags_hbox->arrange();
		auto tag_widget = tag->getWidget();
		tag_widget->arrange();
		int current_width = current_tags_hbox->getWidth() + tag_widget->getWidth() + x_space;
		if (current_width < max_tags_hbox_size)
		{
			current_tags_hbox->addChild(tag_widget, Gui::ALIGN_LEFT);
		}
		else
		{
			// create new
			current_tags_hbox = WidgetHBox::create(x_space);
			current_tags_hbox->setPadding(-x_space, -x_space, 0, 0);
			main_scrollbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);
			current_tags_hbox->addChild(tag_widget);
		}
	}
}

WidgetTagCloud::~WidgetTagCloud()
{
	for (auto &tag : tag_widgets)
		delete tag.data;
	tag_widgets.clear();

	main_scrollbox.deleteLater();
}

void WidgetTagCloud::update(bool enable_focus)
{
	if (Console::isActive())
		return;

	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT);

	for (const auto &tag_widget : tag_widgets)
	{
		tag_widget.data->update(up, down);
	}

	if (enable_focus && MenuUtils::isHovered(main_scrollbox))
		main_scrollbox->setFocus();
}

void WidgetTagCloud::setTagSelected(const Unigine::String &str, bool selected)
{
	if (!tag_widgets.contains(str))
		return;

	auto &widget = tag_widgets[str];
	widget->setTagSelected(selected);
}

WidgetSampleListNode::WidgetSampleListNode(MainMenu::UIConfiguration &config)
{
	main_vbox = WidgetVBox::create();
	header_hbox = WidgetHBox::create();
	children_container_vbox = WidgetVBox::create();

	main_vbox->addChild(header_hbox, Gui::ALIGN_EXPAND);
	main_vbox->addChild(children_container_vbox, Gui::ALIGN_EXPAND);

	tint_color = config.sample_list->tint_color.get();
}

WidgetSampleListNode::~WidgetSampleListNode()
{
	for (int i = 0; i < children.size(); ++i)
		delete children[i];

	main_vbox.deleteLater();
}

void WidgetSampleListNode::addChild(WidgetSampleListNode *node)
{
	children_container_vbox->addChild(node->getWidget(), Gui::ALIGN_EXPAND);
	children.push_back(node);
	node->parent = this;
}

void WidgetSampleListNode::setHidden(bool hide)
{
	if (hide == is_hidden)
		return;

	is_hidden = hide;

	main_vbox->setHidden(is_hidden);
	main_vbox->setEnabled(!is_hidden);
}

void WidgetSampleListNode::setChildrenHidden(bool hide)
{
	if (hide == is_children_hidden)
		return;

	is_children_hidden = hide;

	children_container_vbox->setHidden(is_children_hidden);
	children_container_vbox->setEnabled(!is_children_hidden);
}

void WidgetSampleListNode::show_press_effect(bool show)
{
	header_hbox->setColor(show ? tint_color : vec4_white);
}

void WidgetSampleListNode::update()
{
	if (!isChildrenHidden())
		for (int i = 0; i < children.size(); ++i)
			children[i]->update();

	if (Console::isActive())
		return;

	bool hovered = MenuUtils::isHovered(header_hbox);

	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT);

	if (pressed && up)
	{
		pressed = false;
		if (hovered)
			on_clicked();
	}

	if (!pressed && down && hovered)
		pressed = true;

	show_press_effect(pressed);
}

WidgetSample::WidgetSample(const Sample &sample, MainMenu::UIConfiguration &config)
	: WidgetSampleListNode(config)
{
	auto &sample_params = config.sample_list->sample;
	sample_info = sample;

	header_hbox->setBackground(1);
	header_hbox->setBackgroundTexture(sample_params->background.get());
	header_hbox->setBackgroundColor(sample_params->background_color.get());
	header_hbox->setBackground9Sliced(true);
	vec4 offsets = sample_params->background_offsets.get();
	header_hbox->setBackground9SliceOffsets(offsets.x, offsets.y, offsets.z, offsets.w);
	header_hbox->setBackground9SliceScale(sample_params->background_slice_scale.get());

	ivec4 padding = sample_params->background_padding.get();
	header_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	header_vbox = WidgetVBox::create();
	header_vbox->setSpace(0, sample_params->content_vertical_spacing.get());
	header_hbox->addChild(header_vbox, Gui::ALIGN_EXPAND);

	auto title_hbox = WidgetHBox::create();

	title_label = WidgetLabel::create(sample_info.title);
	title_label->setFont(sample_params->title_font.get());
	title_label->setFontSize(sample_params->title_font_size.get());
	title_label->setFontColor(sample_params->title_font_color.get());
	title_hbox->addChild(title_label, Gui::ALIGN_CENTER);
	header_vbox->addChild(title_hbox, Gui::ALIGN_LEFT);

	auto description_hbox = WidgetHBox::create();
	description_label = WidgetLabel::create(sample_info.description);
	description_label->setFont(sample_params->description_font.get());
	description_label->setFontSize(sample_params->description_font_size.get());
	description_label->setFontColor(sample_params->description_font_color.get());
	description_label->setFontRich(1);
	description_label->setFontWrap(1);
	description_label->setFontVSpacing(sample_params->description_font_vspacing);
	description_hbox->addChild(description_label, Gui::ALIGN_EXPAND);
	header_vbox->addChild(description_hbox, Gui::ALIGN_EXPAND);

	Unigine::String tag_info;
	auto tags_vbox = WidgetVBox::create(0, sample_params->space_between_rows_of_tags.get());
	int x_space = sample_params->space_between_tags_in_row.get();
	auto current_tags_hbox = WidgetHBox::create(x_space);
	current_tags_hbox->setPadding(-x_space, -x_space, 0, 0);
	tags_vbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);
	int max_tags_hbox_width = config.sample_list->sample_list_min_width.get()
		- config.sample_list->padding.get().x - config.sample_list->padding.get().y
		- sample_params->side_offset.get() - sample_params->background_padding.get().x
		- sample_params->background_padding.get().y;
	for (int i = 0; i < sample.tags.size(); ++i)
	{
		tag_info += " " + sample.tags[i];
		auto tag = new Tag(sample.tags[i], MenuUtils::SAMPLES_LIST);
		auto tag_widget = tag->getWidget();
		tag_widgets[sample.tags[i]] = tag;
		tag_widget->arrange();
		current_tags_hbox->arrange();

		if (tag_widget->getWidth() + current_tags_hbox->getWidth() < max_tags_hbox_width)
		{
			current_tags_hbox->addChild(tag_widget, Gui::ALIGN_LEFT);
		}
		else
		{
			current_tags_hbox = WidgetHBox::create(x_space);
			current_tags_hbox->setPadding(-x_space, -x_space, 0, 0);
			tags_vbox->addChild(current_tags_hbox, Gui::ALIGN_LEFT);
			current_tags_hbox->addChild(tag_widget, Gui::ALIGN_LEFT);
		}
	}
	header_vbox->addChild(tags_vbox, Gui::ALIGN_LEFT);

	search_info += sample_info.title.lower();
	search_info += " ";
	search_info += sample_info.description.lower();
	search_info += " ";
	search_info += tag_info.lower();
}

WidgetSample::~WidgetSample()
{
	for (auto &tag : tag_widgets)
		delete tag.data;
	tag_widgets.clear();
}

void WidgetSample::getSearchInfo(Unigine::String &main_info,
	Unigine::Vector<Unigine::String> &tags) const
{
	String category_search_info;
	Vector<String> empty;
	parent->getSearchInfo(category_search_info, empty);

	main_info.clear();
	main_info += search_info;
	main_info += category_search_info.lower();

	tags.clear();
	tags = sample_info.tags;
}

void WidgetSample::update()
{
	if (isHidden())
		return;

	if (pressed)
	{
		WidgetSampleListNode::update();
		return;
	}

	bool some_hovered = false;

	bool down = Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT);
	bool up = Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT);

	for (const auto &tag_widget : tag_widgets)
	{
		some_hovered |= MenuUtils::isHovered(tag_widget.data->getWidget());
		tag_widget.data->update(up, down);
	}

	if(!some_hovered)
		WidgetSampleListNode::update();
}

void WidgetSample::setTagSelected(const Unigine::String &str, bool selected)
{
	if (!tag_widgets.contains(str))
		return;

	auto &widget = tag_widgets[str];
	widget->setTagSelected(selected);
}

void WidgetSample::on_clicked()
{
	World::loadWorld(sample_info.world_name, true);
}

WidgetCategory::WidgetCategory(const Category &category, MainMenu::UIConfiguration &config)
	: WidgetSampleListNode(config)
{
	auto &category_params = config.sample_list->category;
	category_info = category;

	header_hbox->setBackground(1);
	header_hbox->setBackgroundTexture(category_params->background.get());
	header_hbox->setBackgroundColor(category_params->background_color.get());
	header_hbox->setBackground9Sliced(true);
	vec4 offsets = category_params->background_offsets.get();
	header_hbox->setBackground9SliceOffsets(offsets.x, offsets.y, offsets.z, offsets.w);
	header_hbox->setBackground9SliceScale(category_params->background_slice_scale.get());

	ivec4 padding = category_params->background_padding.get();
	header_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	children_container_vbox->setSpace(0, category_params->vertical_spacing.get());
	children_container_vbox->setPadding(config.sample_list->sample->side_offset.get(), 0, 0, 0);

	icon_sprite = WidgetSprite::create();
	icon_sprite->setImage(category_info.icon);
	icon_sprite->setHeight(category_params->icon_size.get().y);
	icon_sprite->setWidth(category_params->icon_size.get().x);
	header_hbox->addChild(icon_sprite, Gui::ALIGN_LEFT);

	title_label = WidgetLabel::create(category_info.title);
	title_label->setFont(category_params->title_font.get());
	title_label->setFontSize(category_params->title_font_size.get());
	title_label->setFontColor(category_params->title_font_color.get());
	auto title_box = WidgetVBox::create();
	title_box->setPadding(category_params->title_offset,0,0,0);
	title_box->addChild(title_label, Gui::ALIGN_LEFT);
	header_hbox->addChild(title_box, Gui::ALIGN_LEFT);

	arrow_sprite = WidgetSprite::create();
	arrow_sprite->setTexture(category_params->arrow_icon.get());
	arrow_sprite->setColor(category_params->arrow_icon_color.get());
	arrow_sprite->setWidth(category_params->arrow_icon_size.get());
	arrow_sprite->setHeight(category_params->arrow_icon_size.get());
	auto icon_padding = category_params->arrow_offset.get();
	auto arrow_box = WidgetVBox::create();
	arrow_box->setPadding(icon_padding.x, icon_padding.y, icon_padding.z, icon_padding.w);
	arrow_box->addChild(arrow_sprite, Gui::ALIGN_CENTER);
	header_hbox->addChild(arrow_box, Gui::ALIGN_RIGHT);

	const Vector<Sample> &samples = category.samples;
	for (int i = 0; i < samples.size(); ++i)
	{
		const Sample &sample = samples[i];

		WidgetSample *widget = new WidgetSample(sample, config);
		addChild(widget);
	}
}

WidgetCategory::~WidgetCategory()
{
}

void WidgetCategory::getSearchInfo(Unigine::String &main_info, Unigine::Vector<Unigine::String> &tags) const
{
	main_info.clear();
	tags.clear();

	main_info += title_label->getText();
}

void WidgetCategory::setTagSelected(const Unigine::String &str, bool selected)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->setTagSelected(str, selected);
}

void WidgetCategory::setChildrenHidden(bool hide)
{
	WidgetSampleListNode::setChildrenHidden(hide);

	if (!children_container_vbox->isHidden())
		arrow_sprite->setTexCoord(vec4(0.0f, 1.0f, 1.0f, 0.0f));
	else
		arrow_sprite->setTexCoord(vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

void WidgetCategory::on_clicked()
{
	setChildrenHidden(!isChildrenHidden());
}

WidgetSampleList::WidgetSampleList(const Vector<Category> &categories, MainMenu::UIConfiguration &config)
{
	main_scrollbox = WidgetScrollBox::create(0, config.sample_list->vertical_spacing.get());
	main_scrollbox->setHScrollEnabled(false);
	main_scrollbox->setWidth(config.sample_list->sample_list_min_width.get());
	main_scrollbox->setVScrollHidden(WidgetScrollBox::SCROLL_RENDER_MODE::ALWAYS_HIDE_NO_BOUNDS);
	main_scrollbox->setBorder(0);

	ivec4 padding = config.sample_list->padding.get();
	main_scrollbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	// temp widget to get scrollbox vbox widget
	auto temp = WidgetVBox::create();
	main_scrollbox->addChild(temp);

	auto background_vbox = checked_ptr_cast<WidgetVBox>(temp->getParent());
	background_vbox->setBackground(1);
	background_vbox->setBackgroundTexture(config.common->white_image.get());
	background_vbox->setBackgroundColor(config.sample_list->sample_list_background_color.get());

	for (int i = 0; i < categories.size(); ++i)
	{
		const Category &category = categories[i];
	
		WidgetCategory *widget = new WidgetCategory(category, config);
		widget->setChildrenHidden(true);

		background_vbox->addChild(widget->getWidget());

		roots.push_back(widget);
	}

	main_scrollbox->removeChild(temp);
	temp.deleteLater();
}

WidgetSampleList::~WidgetSampleList()
{
	for (int i = 0; i < roots.size(); ++i)
		delete roots[i];

	roots.clear();

	main_scrollbox.deleteLater();
}

void WidgetSampleList::setTagSelected(const Unigine::String &str, bool selected)
{
	for (int i = 0; i < roots.size(); ++i)
		roots[i]->setTagSelected(str, selected);
}

void WidgetSampleList::update(bool enable_focus)
{
	for (int i = 0; i < roots.size(); ++i)
		roots[i]->update();

	bool hovered = MenuUtils::isHovered(main_scrollbox);

	if (hovered && enable_focus)
		main_scrollbox->setFocus();
}

void WidgetSampleList::setCollapseAll(bool collapse)
{
	for (int i = 0; i < roots.size(); ++i)
		roots[i]->setChildrenHidden(collapse);
}

void WidgetSampleList::save(const Unigine::BlobPtr &blob)
{
	int num_roots = roots.size();
	blob->writeInt(num_roots);

	for (auto const &root : roots)
	{
		blob->writeString(root->getTitle());
		blob->writeBool(root->isChildrenHidden());
	}

	int scroll = main_scrollbox->getVScrollValue();
	blob->writeInt(scroll);
}

void WidgetSampleList::restore(const Unigine::BlobPtr &blob)
{
	int num_roots = blob->readInt();

	HashMap<String, bool> hidden_children;
	hidden_children.clear();
	hidden_children.reserve(num_roots);

	for (int i = 0; i < num_roots; i++)
	{
		String title = blob->readString();
		bool is_hidden = blob->readBool();
		hidden_children[title] = is_hidden;
	}

	for (auto &root : roots)
	{
		String title = root->getTitle();
		if (hidden_children.contains(title))
		{
			root->setChildrenHidden(hidden_children[title]);
		}
	}

	int scroll_value = blob->readInt();
	main_scrollbox->arrange();
	main_scrollbox->setVScrollValue(scroll_value);
}

void WidgetSampleList::filter(const Unigine::Vector<Unigine::String> &search_words, const Unigine::Vector<Unigine::String> &search_tags)
{
	for (int i = 0; i < roots.size(); ++i)
		filter_node(roots[i], search_words, search_tags);
}

void WidgetSampleList::filter_node(WidgetSampleListNode *node, const Unigine::Vector<Unigine::String> &search_words, const Unigine::Vector<Unigine::String> &search_tags)
{
	int num_children = node->getNumChildren();

	if (num_children > 0)
	{
		int num_enabled = 0;
		for (int i = 0; i < num_children; ++i)
		{
			filter_node(node->getChild(i), search_words, search_tags);

			if (!node->getChild(i)->isHidden())
				++num_enabled;
		}

		node->setHidden(num_enabled == 0);
		if (!node->isHidden())
			node->setChildrenHidden(false);
	}
	else
	{
		String main_info;
		Vector<String> node_tags;
		node->getSearchInfo(main_info, node_tags);

		if (search_words.empty() && search_tags.empty())
		{
			node->setHidden(false);
			return;
		}

		int num_tag_matches = 0;
		// check tags
		for (int i = 0; i < node_tags.size(); ++i)
		{
			auto node_tag = node_tags[i];
			node_tag.lower();
			for (int j = 0; j < search_tags.size(); ++j)
			{
				String tag = search_tags[j];
				tag.lower();
				if (!String::compare(node_tag, tag))
				{
					++num_tag_matches;
					break;
				}
			}
		}

		bool tags_search = num_tag_matches == search_tags.size();
		if (!tags_search)
		{
			node->setHidden(true);
			return;
		}

		bool words_search = search_words.empty();
		// check words
		for (int i = 0; i < search_words.size(); ++i)
		{
			if (MenuUtils::isWordSuitable(main_info, search_words[i]))
			{
				words_search = true;
				break;
			}
		}

		node->setHidden(!words_search);
	}
}

WidgetHBoxPtr MenuUtils::createTagWidget(const String &text, const MainMenu::UITagStyle &config, TAG_PLACE place)
{
	auto main_hbox = WidgetHBox::create();

	ivec4 padding = config.background_padding.get();
	switch (place)
	{
	case TAG_PLACE::SEARCH_FIELD:
		padding = ivec4(toInt(padding.x * config.search_field_scale.get()),
			toInt(padding.y * config.search_field_scale.get()),
			toInt(padding.z * config.search_field_scale.get()),
			toInt(padding.w * config.search_field_scale.get()));
		break;
	case TAG_PLACE::SAMPLES_LIST:
		padding = ivec4(toInt(padding.x * config.sample_list_scale.get()),
			toInt(padding.y * config.sample_list_scale.get()),
			toInt(padding.z * config.sample_list_scale.get()),
			toInt(padding.w * config.sample_list_scale.get()));
		break;
	case TAG_PLACE::TAGS_CLOUD:
		padding = ivec4(toInt(padding.x * config.tag_cloud_scale.get()),
			toInt(padding.y * config.tag_cloud_scale.get()),
			toInt(padding.z * config.tag_cloud_scale.get()),
			toInt(padding.w * config.tag_cloud_scale.get()));
		break;
	}

	main_hbox->setPadding(padding.x, padding.y, padding.z, padding.w);

	main_hbox->setBackground(1);
	main_hbox->setBackgroundColor(config.background_color.get());
	main_hbox->setBackgroundTexture(config.background.get());
	main_hbox->setBackground9Sliced(true);
	vec4 offsets = config.background_offsets.get();
	main_hbox->setBackground9SliceOffsets(offsets.x, offsets.y, offsets.z, offsets.w);
	main_hbox->setBackground9SliceScale(config.background_slice_scale.get());

	auto label = WidgetLabel::create(text.get());
	label->setFont(config.font.get());
	int font_size = config.font_size.get();
	auto font_color = config.font_color.get();
	switch (place)
	{
	case TAG_PLACE::SEARCH_FIELD:
		font_size *= config.search_field_scale.get();
		font_color = config.selected_font_color.get();
		label->setFont(config.selected_font.get());
		break;
	case TAG_PLACE::SAMPLES_LIST:
		font_size *= config.sample_list_scale.get();
		break;
	case TAG_PLACE::TAGS_CLOUD:
		font_size *= config.tag_cloud_scale.get();
		break;
	}
	label->setFontSize(font_size);
	label->setFontColor(font_color);

	main_hbox->addChild(label);

	if (place == TAG_PLACE::SEARCH_FIELD)
	{
		main_hbox->setBackgroundTexture(config.selected_background.get());
		main_hbox->setSpace(5, 0);
		main_hbox->setPadding(padding.x, padding.y - 5, padding.z, padding.w);

		auto remove_button_sprite = WidgetSprite::create();
		remove_button_sprite->setLayerTexture(0, config.remove_button_background.get());
		remove_button_sprite->setLayerColor(0, config.remove_button_background_color.get());
		remove_button_sprite->addLayer();
		remove_button_sprite->setLayerTexture(1, config.remove_button_icon.get());
		remove_button_sprite->setLayerColor(1, config.remove_button_icon_color.get());
		remove_button_sprite->setWidth(config.remove_button_size.get() * config.search_field_scale.get());
		remove_button_sprite->setHeight(config.remove_button_size.get() * config.search_field_scale.get());

		main_hbox->addChild(remove_button_sprite, Gui::ALIGN_RIGHT);
	}

	return main_hbox;
}

bool MenuUtils::isWordSuitable(const String &text, const String &word)
{
	for (int i = 0; i < text.size() - word.size() + 1; ++i)
	{
		int num_matches = 0;
		for (int j = 0; j < word.size(); ++j)
		{
			if (text[i + j] == word[j])
				++num_matches;
			else
				break;
		}

		if (num_matches == word.size())
			return true;
	}

	return false;
}

bool MenuUtils::isHovered(const Unigine::WidgetPtr widget)
{
	bool hovered = widget->getMouseX() >= 0 && widget->getMouseX() < widget->getWidth()
		&& widget->getMouseY() >= 0 && widget->getMouseY() < widget->getHeight();
	return hovered;
}


MainMenu::UITagStyle Tag::config;
Unigine::EventInvoker<const Unigine::String &> Tag::event_clicked;

Tag::Tag(const Unigine::String &text_, MenuUtils::TAG_PLACE place)
	: text(text_)
{
	tag_widget = MenuUtils::createTagWidget(text_, config, place);
	for (int i = 0; i < tag_widget->getNumChildren(); i++)
	{
		auto child = checked_ptr_cast<WidgetLabel>(tag_widget->getChild(i));
		if (child)
		{
			label = child;
			break;
		}
	}
}

void Tag::setTagSelected(bool selected)
{
	if (is_selected == selected)
		return;

	is_selected = selected;
	vec4 color = config.font_color.get();
	if (selected)
	{
		tag_widget->setBackgroundTexture(config.selected_background.get());
		color = config.selected_font_color.get();
	}
	else
	{
		tag_widget->setBackgroundTexture(config.background.get());
	}

	if (label)
		label->setFontColor(color);
}

void Tag::update(bool up, bool down)
{
	bool hovered = MenuUtils::isHovered(tag_widget);
	if (!hovered && !up)
		return;

	if (is_pressed && up)
	{
		is_pressed = false;
		if (hovered)
		{
			event_clicked.run(text);
//			setTagSelected(!is_selected);
		}
	}

	if (!is_pressed && down && hovered)
	{
		is_pressed = true;
	}

	tag_widget->setColor(is_pressed ? config.tint_color : vec4_white);
}

// Demonstrates multi-script text rendering (HarfBuzz shaping) across GUI widgets, scene
// ObjectText nodes and a world-space ObjectGui, using six languages (EN/RU/AR/ZH/TH/HI).
// RTL and complex scripts rely on the font fallback chain set with setGlobalFontFallback();
// the Noto/Hind .ttf files must be present in the data folder or glyphs render as "tofu" (□).

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineGui.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

// One language entry: ISO tag plus plain, paragraph (for wrapping) and rich (markup) variants.
struct LangSample
{
	const char *code;
	const char *plain;
	const char *paragraph;
	const char *rich;
};

class FontsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(FontsSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	
	PROP_PARAM(Node, text_en, "Text EN (ObjectText)");
	PROP_PARAM(Node, text_ru, "Text RU (ObjectText)");
	PROP_PARAM(Node, text_ar, "Text AR (ObjectText)");
	PROP_PARAM(Node, text_zh, "Text ZH (ObjectText)");
	PROP_PARAM(Node, text_th, "Text TH (ObjectText)");
	PROP_PARAM(Node, text_hi, "Text HI (ObjectText)");
	PROP_PARAM(Node, gui_object, "World GUI object (ObjectGui)");

	PROP_PARAM(File, font_latin,   "", "Latin / Cyrillic font", "Used for English and Russian.", "Fonts", "filter=.ttf");
	PROP_PARAM(File, font_arabic,  "", "Arabic font",           "Used for Arabic.",              "Fonts", "filter=.ttf");
	PROP_PARAM(File, font_chinese, "", "Chinese font",          "Used for Chinese.",             "Fonts", "filter=.ttf");
	PROP_PARAM(File, font_thai,    "", "Thai font",             "Used for Thai.",                "Fonts", "filter=.ttf");
	PROP_PARAM(File, font_hindi,   "", "Hindi font",            "Used for Hindi (Devanagari).",  "Fonts", "filter=.ttf");

private:
	void init();
	void shutdown();

	// Controls.
	void install_font_fallback();
	void set_text_direction(int idx);
	void set_cursor_mode(int idx);
	void set_font_size(int size);

	// Test panel (screen-space GUI). keep() tracks widgets so the font size can be applied to
	// all of them at once.
	template <class T>
	Unigine::Ptr<T> keep(const Unigine::Ptr<T> &w)
	{
		widgets.append(w);
		return w;
	}

	Unigine::WidgetVBoxPtr begin_tab(const Unigine::WidgetTabBoxPtr &tabs, const char *name, const char *hint);
	void add_lang_table(const Unigine::WidgetVBoxPtr &root, bool wrap, bool rich);
	void add_lang_row(const Unigine::WidgetGridBoxPtr &grid, const LangSample &s, bool wrap, bool rich);
	void add_bidi_section(const Unigine::WidgetVBoxPtr &root);
	void add_alignment_section(const Unigine::WidgetVBoxPtr &root);
	void add_outline_section(const Unigine::WidgetVBoxPtr &root);
	void add_color_section(const Unigine::WidgetVBoxPtr &root);
	void add_per_widget_font_section(const Unigine::WidgetVBoxPtr &root);
	void build_feature_panel();

	// Scene nodes (ObjectText / ObjectGui placed in the world).
	Unigine::ObjectTextPtr get_text_object(int i);
	const char *font_for_lang(int i);
	void setup_object_texts();
	void setup_object_gui();
	void build_object_gui_content();

private:
	SampleDescriptionWindow description_window;
	Unigine::WidgetWindowPtr features_window;
	// Every panel widget, kept so font size can be applied to all of them.
	Unigine::Vector<Unigine::WidgetPtr> widgets;

	// Resolved ObjectGui node and its GUI, plus the content root for rebuilds.
	Unigine::ObjectGuiPtr object_gui;
	Unigine::GuiPtr object_gui_gui;
	Unigine::WidgetVBoxPtr object_gui_root;

	int font_size = 16;
	Unigine::Gui::CursorMode cursor_mode = Unigine::Gui::CURSOR_MODE_AUTO;
};

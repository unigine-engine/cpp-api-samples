// Multi-script text rendering sample. See FontsSample.h for the overview.

#include "FontsSample.h"

#include <UnigineWindowManager.h>

REGISTER_COMPONENT(FontsSample);

using namespace Unigine;
using namespace Unigine::Math;

namespace
{
	// Order is fixed and shared with font_for_lang() below.
	const LangSample SAMPLES[] = {
		{ "EN", "Hello, world!",
		  "The quick brown fox jumps over the lazy dog near the river bank "
		  "where the trees grow tall and the wind whispers through the leaves.",
		  "Hello <b>bold</b> <i>italic</i> <font color=#ff0000>red</font> <font color=#00cc00>green</font> world!" },
		{ "RU", "Привет, мир!",
		  "Съешь же ещё этих мягких французских булок, да выпей чаю — "
		  "холодного или горячего, как тебе угодно, у самовара на старой даче.",
		  "Привет <b>жирный</b> <i>курсив</i> <font color=#ff0000>красный</font> <font color=#00cc00>зелёный</font> мир!" },
		{ "AR", "مرحبا بالعالم",
		  "في عام 2024 بدأت العديد من الشركات التقنية باستخدام أدوات "
		  "تعتمد على الذكاء الاصطناعي لتحسين تجربة المستخدم بشكل ملحوظ.",
		  "مرحبا <b>بالعالم</b> <font color=#ff0000>عربي</font> <font color=#00cc00>نص</font> RTL" },
		{ "ZH", "你好，世界!",
		  "敏捷的棕色狐狸跳过懒狗。在长江两岸的城市里，许多公司正在使用"
		  "人工智能技术来改善用户在现代应用程序中的体验。",
		  "你好 <b>粗体</b> <font color=#ff0000>红色</font> <font color=#00cc00>绿色</font> 世界!" },
		{ "TH", "สวัสดีชาวโลก",
		  "สุนัขจิ้งจอกสีน้ำตาลที่ว่องไวกระโดดข้ามสุนัขขี้เกียจ "
		  "ในสวนที่สวยงามใกล้แม่น้ำซึ่งต้นไม้สูงและลมพัดเบาๆ",
		  "สวัสดี <b>หนา</b> <font color=#ff0000>แดง</font> <font color=#00cc00>เขียว</font> ชาวโลก" },
		{ "HI", "नमस्ते दुनिया",
		  "तेज़ भूरी लोमड़ी आलसी कुत्ते के ऊपर कूदती है। नदी के पास "
		  "के पेड़ बहुत लंबे हैं और शाम भर हवा धीरे-धीरे बहती रहती है।",
		  "नमस्ते <b>मोटा</b> <font color=#ff0000>लाल</font> <font color=#00cc00>हरा</font> दुनिया" },
	};
	const int NUM_SAMPLES = 6;

	// Mixed-direction (bidirectional) strings: digits, Latin and RTL runs in one line.
	struct TextSample
	{
		const char *label;
		const char *text;
	};
	const TextSample BIDI_SAMPLES[] = {
		{ "AR+EN+Num", "المستخدم John Smith لديه 3 رسائل جديدة" },
		{ "EN+AR",     "The document title is \"تقرير سنوي\" and it has 42 pages" },
		{ "AR+ZH",     "النسخة الصينية: 你好世界 متاحة الآن" },
		{ "Nested",    "Start مرحبا Hello again عودة end" },
	};
	const int NUM_BIDI = 4;

	// Accent color for section headers in the test panel.
	const char *HEADER_COLOR = "#7fbfff";

	// Column widths for the test-panel tables.
	const int COL_TAG = 50, COL_LBL = 230, COL_BTN = 150, COL_EDIT = 230, COL_ET = 230, COL_ET_H = 32;
	const int COL_PAD = 6, COL_PADY = 3;
}

// The parameter window, the test panel and the scene nodes are set up here.
void FontsSample::init()
{
	install_font_fallback();

	description_window.createWindow(Gui::ALIGN_LEFT | Gui::ALIGN_TOP);

	// Global text direction: Auto picks LTR/RTL from the first strong character of each string,
	// while LTR/RTL force a single block direction for every widget at once.
	description_window.addSwitchParameter("Text direction",
		"Auto detects direction per string; LTR/RTL force one direction globally.",
		0, { "Auto", "LTR", "RTL" }, [this](int idx) { set_text_direction(idx); });

	// Caret movement across bidirectional text: visual follows on-screen position,
	// logical follows character order in memory.
	description_window.addSwitchParameter("Cursor mode",
		"How the caret steps through bidirectional text.",
		0, { "Auto", "Visual", "Logical" }, [this](int idx) { set_cursor_mode(idx); });

	description_window.addIntParameter("GUI font size", "Font size of the feature-panel widgets.",
		font_size, 12, 32, [this](int v) { set_font_size(v); });

	description_window.addParameterSpacer();

	description_window.addBoolParameter("Show widget font features", "Show the multi-script widget font features window.",
		true, [this](bool v) { if (features_window) features_window->setHidden(!v); });

	build_feature_panel();
	setup_object_texts();
	setup_object_gui();
}

// The test panel is released; the scene nodes belong to the world and are left untouched.
void FontsSample::shutdown()
{
	widgets.clear();
	if (features_window)
		features_window.deleteLater();
	description_window.shutdown();
}

// =========================================================================
// Controls
// =========================================================================

// Lists per-script fonts tried in order when the primary font has no glyph for a character.
void FontsSample::install_font_fallback()
{
	const char *paths[] = {
		font_hindi.get(), font_arabic.get(), font_chinese.get(), font_thai.get(), font_latin.get(),
	};
	VectorStack<String> fonts;
	for (const char *p : paths)
		if (p && *p)
			fonts.append(p);
	Gui::setGlobalFontFallback(fonts);
}

void FontsSample::set_text_direction(int idx)
{
	// Combo order matches the TextDirection enum (Auto = 0, LTR = 1, RTL = 2).
	Gui::TextDirection dir = (Gui::TextDirection)idx;
	Gui::getCurrent()->setGlobalTextDirection(dir);
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		ObjectTextPtr text = get_text_object(i);
		if (text)
			text->setTextDirection(dir);
	}
	if (object_gui_gui)
		object_gui_gui->setGlobalTextDirection(dir);
}

void FontsSample::set_cursor_mode(int idx)
{
	cursor_mode = (Gui::CursorMode)idx;
	Gui::getCurrent()->setGlobalCursorMode(cursor_mode);
	if (object_gui_gui)
		object_gui_gui->setGlobalCursorMode(cursor_mode);
}

void FontsSample::set_font_size(int size)
{
	font_size = size;
	for (int i = 0; i < widgets.size(); i++)
		if (widgets[i])
			widgets[i]->setFontSize(font_size);
}

// =========================================================================
// Test panel (screen-space GUI)
// =========================================================================

// Creates a new tab holding a content column (with a subtle hint line) and returns that
// column so a section can be built into it. The tab has a fixed width (horizontal scroll
// when content is wider) but its height grows to fit the content.
WidgetVBoxPtr FontsSample::begin_tab(const WidgetTabBoxPtr &tabs, const char *name, const char *hint)
{
	tabs->addTab(name);

	auto scroll = WidgetScrollBox::create();
	scroll->setBackground(0);
	scroll->setBorder(0);
	scroll->setWidth(min(900, Gui::getCurrent()->getWidth() - 120));
	scroll->setHeight(min(420, Gui::getCurrent()->getHeight() - 160));
	scroll->setHScrollEnabled(true);
	scroll->setVScrollEnabled(true);
	scroll->setHScrollHidden(WidgetScrollBox::AUTO_HIDE);
	scroll->setVScrollHidden(WidgetScrollBox::AUTO_HIDE);
	tabs->addChild(scroll, Gui::ALIGN_EXPAND);

	auto content = WidgetVBox::create();
	content->setSpace(0, 4);
	content->setPadding(10, 10, 10, 10);
	scroll->addChild(content, Gui::ALIGN_EXPAND);

	auto hint_label = keep(WidgetLabel::create(String::format("<font color=%s><i>%s</i></font>", HEADER_COLOR, hint).get()));
	hint_label->setFontRich(1);
	hint_label->setFontSize(font_size);
	content->addChild(hint_label);

	return content;
}

// A language table is a 5-column grid: every column is sized to its widest cell, so the
// header and all language rows line up regardless of how wide each script renders.
void FontsSample::add_lang_table(const WidgetVBoxPtr &root, bool wrap, bool rich)
{
	auto grid = keep(WidgetGridBox::create(5, COL_PAD, COL_PADY));

	auto header = [&](const char *text, int width) {
		auto label = keep(WidgetLabel::create(String::format("<b>%s</b>", text).get()));
		label->setFontRich(1);
		label->setFontSize(font_size);
		label->setWidth(width);
		grid->addChild(label, Gui::ALIGN_LEFT);
	};
	header("Lang", COL_TAG);
	header("Label", COL_LBL);
	header("Button", COL_BTN);
	header("EditLine", COL_EDIT);
	header("EditText", COL_ET);

	for (int i = 0; i < NUM_SAMPLES; i++)
		add_lang_row(grid, SAMPLES[i], wrap, rich);

	root->addChild(grid);
}

// One language as five grid cells (tag, Label, Button, EditLine, EditText). wrap enables
// word wrapping, rich enables markup parsing.
void FontsSample::add_lang_row(const WidgetGridBoxPtr &grid, const LangSample &s, bool wrap, bool rich)
{
	const char *text = rich ? s.rich : (wrap ? s.paragraph : s.plain);

	auto code = keep(WidgetLabel::create(s.code));
	code->setFontSize(font_size);
	code->setWidth(COL_TAG);
	grid->addChild(code, Gui::ALIGN_LEFT);

	auto label = keep(WidgetLabel::create(text));
	label->setFontSize(font_size);
	label->setWidth(COL_LBL);
	label->setFontRich(rich);
	label->setFontWrap(wrap);
	grid->addChild(label, Gui::ALIGN_LEFT);

	auto button = keep(WidgetButton::create(rich ? s.rich : s.plain));
	button->setFontSize(font_size);
	button->setFontRich(rich);
	button->setWidth(COL_BTN);
	grid->addChild(button, Gui::ALIGN_LEFT);

	auto edit_line = keep(WidgetEditLine::create(rich ? s.rich : s.plain));
	edit_line->setBackground(1);
	edit_line->setFontSize(font_size);
	edit_line->setWidth(COL_EDIT);
	edit_line->setFontRich(rich);
	grid->addChild(edit_line, Gui::ALIGN_LEFT);

	auto edit_text = keep(WidgetEditText::create(text));
	edit_text->setBackground(1);
	edit_text->setFontSize(font_size);
	edit_text->setWidth(COL_ET);
	edit_text->setHeight(COL_ET_H);
	edit_text->setFontRich(rich);
	grid->addChild(edit_text, Gui::ALIGN_LEFT);
}

void FontsSample::add_bidi_section(const WidgetVBoxPtr &root)
{
	auto grid = keep(WidgetGridBox::create(3, COL_PAD, COL_PADY));
	for (int i = 0; i < NUM_BIDI; i++)
	{
		auto tag = keep(WidgetLabel::create(BIDI_SAMPLES[i].label));
		tag->setFontSize(font_size);
		tag->setWidth(120);
		grid->addChild(tag, Gui::ALIGN_LEFT);
		auto label = keep(WidgetLabel::create(BIDI_SAMPLES[i].text));
		label->setFontSize(font_size);
		label->setWidth(340);
		grid->addChild(label, Gui::ALIGN_LEFT);
		auto edit = keep(WidgetEditLine::create(BIDI_SAMPLES[i].text));
		edit->setBackground(1);
		edit->setFontSize(font_size);
		edit->setWidth(340);
		grid->addChild(edit, Gui::ALIGN_LEFT);
	}
	root->addChild(grid);
}

void FontsSample::add_alignment_section(const WidgetVBoxPtr &root)
{
	// Each alignment is shown inside a fixed-width tinted box so the text container bounds
	// are visible and the left/center/right placement is obvious.
	struct AlignCase { const char *name; int align; vec4 color; };
	const AlignCase cases[] = {
		{ "Left",   Gui::ALIGN_LEFT,   vec4(1.0f, 0.4f, 0.4f, 0.14f) },
		{ "Center", Gui::ALIGN_CENTER, vec4(0.4f, 1.0f, 0.4f, 0.14f) },
		{ "Right",  Gui::ALIGN_RIGHT,  vec4(0.4f, 0.6f, 1.0f, 0.14f) },
	};
	const int cell_w = 210;

	auto grid = keep(WidgetGridBox::create(4, COL_PAD, COL_PADY));

	auto header = [&](const char *text, int width) {
		auto label = keep(WidgetLabel::create(String::format("<b>%s</b>", text).get()));
		label->setFontRich(1);
		label->setFontSize(font_size);
		label->setWidth(width);
		grid->addChild(label, Gui::ALIGN_LEFT);
	};
	header("Lang", COL_TAG);
	for (const auto &c : cases)
		header(c.name, cell_w);

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		auto tag = keep(WidgetLabel::create(SAMPLES[i].code));
		tag->setFontSize(font_size);
		tag->setWidth(COL_TAG);
		grid->addChild(tag, Gui::ALIGN_LEFT);
		for (const auto &c : cases)
		{
			auto box = WidgetVBox::create();
			box->setWidth(cell_w);
			box->setBackground(1);
			box->setBackgroundColor(c.color);
			auto label = keep(WidgetLabel::create(SAMPLES[i].plain));
			label->setFontSize(font_size);
			label->setWidth(cell_w);
			label->setTextAlign(c.align);
			box->addChild(label, Gui::ALIGN_EXPAND);
			grid->addChild(box, Gui::ALIGN_LEFT);
		}
	}
	root->addChild(grid);
}

void FontsSample::add_outline_section(const WidgetVBoxPtr &root)
{
	auto grid = keep(WidgetGridBox::create(4, COL_PAD, COL_PADY));
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		auto tag = keep(WidgetLabel::create(SAMPLES[i].code));
		tag->setFontSize(font_size);
		tag->setWidth(COL_TAG);
		grid->addChild(tag, Gui::ALIGN_LEFT);
		auto plain = keep(WidgetLabel::create(SAMPLES[i].plain));
		plain->setFontSize(font_size);
		plain->setWidth(210);
		grid->addChild(plain, Gui::ALIGN_LEFT);
		auto outline = keep(WidgetLabel::create(SAMPLES[i].plain));
		outline->setFontSize(font_size);
		outline->setFontOutline(1);
		outline->setWidth(210);
		grid->addChild(outline, Gui::ALIGN_LEFT);
		auto big = keep(WidgetLabel::create(SAMPLES[i].plain));
		big->setFontSize(font_size + 8);
		big->setFontOutline(1);
		big->setWidth(240);
		grid->addChild(big, Gui::ALIGN_LEFT);
	}
	root->addChild(grid);
}

void FontsSample::add_color_section(const WidgetVBoxPtr &root)
{
	struct ColorCase { const char *tag; const char *text; vec4 color; };
	const ColorCase cases[] = {
		{ "AR", "مرحبا بالعالم",   vec4(1.0f, 0.2f, 0.2f, 1.0f) },
		{ "ZH", "你好，世界！",      vec4(0.1f, 0.8f, 0.1f, 1.0f) },
		{ "HI", "नमस्ते दुनिया",   vec4(0.2f, 0.5f, 1.0f, 1.0f) },
		{ "TH", "สวัสดีชาวโลก",    vec4(1.0f, 0.8f, 0.0f, 1.0f) },
		{ "EN", "Hello, world!",    vec4(0.0f, 0.8f, 0.8f, 1.0f) },
		{ "RU", "Привет, мир!",     vec4(1.0f, 0.5f, 0.0f, 1.0f) },
	};
	auto grid = keep(WidgetGridBox::create(3, COL_PAD, COL_PADY));
	for (const auto &c : cases)
	{
		auto tag = keep(WidgetLabel::create(c.tag));
		tag->setFontSize(font_size);
		tag->setWidth(COL_TAG);
		grid->addChild(tag, Gui::ALIGN_LEFT);
		auto label = keep(WidgetLabel::create(c.text));
		label->setFontSize(font_size);
		label->setWidth(250);
		label->setFontColor(c.color);
		grid->addChild(label, Gui::ALIGN_LEFT);
		auto edit = keep(WidgetEditLine::create(c.text));
		edit->setBackground(1);
		edit->setFontSize(font_size);
		edit->setWidth(250);
		edit->setFontColor(c.color);
		grid->addChild(edit, Gui::ALIGN_LEFT);
	}
	root->addChild(grid);
}

void FontsSample::add_per_widget_font_section(const WidgetVBoxPtr &root)
{
	auto grid = keep(WidgetGridBox::create(3, COL_PAD, COL_PADY));
	auto add = [&](const char *label_text, const char *text, const char *font) {
		auto tag = keep(WidgetLabel::create(label_text));
		tag->setFontSize(font_size);
		tag->setWidth(200);
		grid->addChild(tag, Gui::ALIGN_LEFT);
		auto label = keep(WidgetLabel::create(text));
		label->setFontSize(font_size);
		label->setWidth(280);
		label->setFont(font);
		grid->addChild(label, Gui::ALIGN_LEFT);
		auto edit = keep(WidgetEditLine::create(text));
		edit->setBackground(1);
		edit->setFontSize(font_size);
		edit->setWidth(280);
		edit->setFont(font);
		grid->addChild(edit, Gui::ALIGN_LEFT);
	};
	add("Arabic font",  "مرحبا بالعالم",   font_arabic.get());
	add("Chinese font", "你好，世界！测试", font_chinese.get());
	add("Hindi font",   "नमस्ते दुनिया",   font_hindi.get());
	add("Thai font",    "สวัสดีชาวโลก",    font_thai.get());
	add("Latin on AR",  "مرحبا بالعالم",   font_latin.get());
	root->addChild(grid);
}

// Builds the floating, resizable window with one feature per tab.
void FontsSample::build_feature_panel()
{
	features_window = WidgetWindow::create("Multi-script font features");
	features_window->setSizeable(1);

	auto tabs = WidgetTabBox::create(4, 4);
	features_window->addChild(tabs, Gui::ALIGN_EXPAND);

	add_lang_table(begin_tab(tabs, "Plain", "Plain text in Label, Button, EditLine and EditText."), false, false);
	add_lang_table(begin_tab(tabs, "Wrapped", "Word-wrapped paragraphs."), true, false);
	add_lang_table(begin_tab(tabs, "Rich", "Rich markup: bold, italic and color."), false, true);
	add_bidi_section(begin_tab(tabs, "Bidirectional", "Mixed LTR/RTL runs with digits and punctuation."));
	add_alignment_section(begin_tab(tabs, "Alignment", "Left / center / right alignment."));
	add_outline_section(begin_tab(tabs, "Outline", "No outline / outline / large outline."));
	add_color_section(begin_tab(tabs, "Color", "Per-widget setFontColor."));
	add_per_widget_font_section(begin_tab(tabs, "Per-widget font", "setFont overrides the global fallback chain."));

	tabs->setCurrentTab(0);
	features_window->arrange();
	WindowManager::getMainWindow()->addChild(features_window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
}

// =========================================================================
// Scene nodes (ObjectText / ObjectGui placed in the world)
// =========================================================================

// Returns the ObjectText assigned for language i (SAMPLES order), or null if unassigned.
ObjectTextPtr FontsSample::get_text_object(int i)
{
	NodePtr nodes[] = {
		text_en.get(), text_ru.get(), text_ar.get(),
		text_zh.get(), text_th.get(), text_hi.get(),
	};
	if (i < 0 || i >= NUM_SAMPLES || !nodes[i])
		return ObjectTextPtr();
	ObjectTextPtr text = checked_ptr_cast<ObjectText>(nodes[i]);
	if (!text)
		Log::warning("FontsSample: text node for %s is not an ObjectText.\n", SAMPLES[i].code);
	return text;
}

// Preferred font path for language i (SAMPLES order), taken from the File parameters.
const char *FontsSample::font_for_lang(int i)
{
	switch (i)
	{
		case 2: return font_arabic.get();
		case 3: return font_chinese.get();
		case 4: return font_thai.get();
		case 5: return font_hindi.get();
		default: return font_latin.get();  // EN, RU
	}
}

// Fills each assigned ObjectText node with its language sample and matching font.
void FontsSample::setup_object_texts()
{
	Gui::TextDirection dir = Gui::getCurrent()->getGlobalTextDirection();
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		ObjectTextPtr text = get_text_object(i);
		if (!text)
			continue;
		text->setFontName(font_for_lang(i));
		text->setTextDirection(dir);
		text->setText(SAMPLES[i].paragraph);
	}
}

// Resolves the ObjectGui node and populates it.
void FontsSample::setup_object_gui()
{
	if (!gui_object.get())
		return;
	object_gui = checked_ptr_cast<ObjectGui>(gui_object.get());
	if (!object_gui)
	{
		Log::warning("FontsSample: \"World GUI object\" (%s) is not an ObjectGui.\n", gui_object->getName());
		return;
	}
	object_gui_gui = object_gui->getGui();
	build_object_gui_content();
}

// (Re)builds the widget tree shown on the ObjectGui surface.
void FontsSample::build_object_gui_content()
{
	if (!object_gui_gui)
		return;

	// Replace any previously built content so font-size changes take effect.
	if (object_gui_root)
	{
		object_gui_gui->removeChild(object_gui_root);
		object_gui_root.deleteLater();
	}

	// Scale the layout to the ObjectGui surface resolution (designed for 1024 wide) so the
	// text fills the physical panel instead of rendering tiny.
	float sc = object_gui_gui->getWidth() > 0 ? object_gui_gui->getWidth() / 1024.0f : 1.0f;
	int title_fs = (int)(54 * sc);
	int row_fs = (int)(44 * sc);
	int pad = (int)(20 * sc);

	object_gui_root = WidgetVBox::create();
	object_gui_root->setSpace(0, (int)(10 * sc));
	object_gui_root->setPadding(pad, pad, pad, pad);

	auto title = WidgetLabel::create(String::format("<font color=%s><b>World GUI — multi-script</b></font>", HEADER_COLOR).get());
	title->setFontRich(1);
	title->setFontSize(title_fs);
	object_gui_root->addChild(title);

	auto grid = WidgetGridBox::create(3, (int)(12 * sc), (int)(8 * sc));
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		auto code = WidgetLabel::create(SAMPLES[i].code);
		code->setFontSize(row_fs);
		code->setWidth((int)(90 * sc));
		grid->addChild(code, Gui::ALIGN_LEFT);
		auto label = WidgetLabel::create(SAMPLES[i].plain);
		label->setFontSize(row_fs);
		label->setWidth((int)(400 * sc));
		grid->addChild(label, Gui::ALIGN_LEFT);
		auto edit = WidgetEditLine::create(SAMPLES[i].plain);
		edit->setBackground(1);
		edit->setFontSize(row_fs);
		edit->setWidth((int)(400 * sc));
		grid->addChild(edit, Gui::ALIGN_LEFT);
	}
	object_gui_root->addChild(grid);

	object_gui_gui->addChild(object_gui_root, Gui::ALIGN_EXPAND);
	object_gui_gui->setGlobalTextDirection(Gui::getCurrent()->getGlobalTextDirection());
	object_gui_gui->setGlobalCursorMode(cursor_mode);
}

// Demonstrates ObjectText for rendering 3D text in the world. Three text objects are
// created: user-editable text with adjustable font properties, static rich text with
// HTML-like formatting, and dynamic text that cycles through color values each frame.

#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Unigine::Math;

// Creates and manages three ObjectText instances demonstrating different use cases.
class ObjectTextSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(ObjectTextSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Default text shown when user input is empty
	PROP_PARAM(String, user_text_if_empty, "Here could be your text...");

	// Speed of color component change per second for dynamic text
	PROP_PARAM(Float, color_step, 1.0f);

private:
	// Three ObjectText instances are created with UI for editing first one.
	void init()
	{
		color = min_color;

		description_window.createWindow();

		auto &params = description_window.getParameterGridBox();

		// Default parameters for user-editable text object
		text_user_pos = Vec3(0, 12, 2);
		vec4 user_color = vec4_white;
		float user_wrap = 32;
		bool user_depth_test = false;
		int user_outline = 0;
		int user_vspacing = 0;
		int user_hspacing = 0;
		int user_resolution = 512;
		int user_size = 128;
		bool user_rich = false;

		// UI controls for adjusting text properties with live preview
		description_window.addFloatParameter("Wrap width", nullptr, user_wrap, 4, 64, [this](float v) {
			text_user->setTextWrapWidth(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addBoolParameter("Depth test", nullptr, user_depth_test, [this](bool v) {
			text_user->setDepthTest(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addIntParameter("Font outline", nullptr, user_outline, 0, 32, [this](int v) {
			text_user->setFontOutline(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addIntParameter("Vertical spacing", nullptr, user_vspacing, 0, 32, [this](int v) {
			text_user->setFontVSpacing(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addIntParameter("Horizontal spacing", nullptr, user_hspacing, 0, 32, [this](int v) {
			text_user->setFontHSpacing(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addIntParameter("Font resolution", nullptr, user_resolution, 128, 1024, [this](int v) {
			text_user->setFontResolution(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addIntParameter("Font size", nullptr, user_size, 32, 256, [this](int v) {
			text_user->setFontSize(v);
			center_text(text_user, text_user_pos);
		});
		description_window.addBoolParameter("Rich text", nullptr, user_rich, [this](bool v) {
			text_user->setFontRich(v);
			center_text(text_user, text_user_pos);
		});

		// Label and scrollable container for multiline text input
		auto label = WidgetLabel::create("Text");
		params->addChild(label);

		auto scroll_box = WidgetScrollBox::create();
		scroll_box->setWidth(200);
		scroll_box->setHeight(100);
		scroll_box->setVScrollEnabled(true);
		scroll_box->setHScrollEnabled(true);
		params->addChild(scroll_box);

		// Text input widget updates ObjectText content in real-time
		auto edit_text = WidgetEditText::create();
		edit_text->setText(user_text_if_empty);
		edit_text->getEventChanged().connect(*this, [this, edit_text]() {
			// Placeholder text is shown when input is cleared
			if (edit_text->getText()[0] != '\0')
			{
				text_user->setText(edit_text->getText());
			}
			else
			{
				text_user->setText(user_text_if_empty);
			}

			center_text(text_user, text_user_pos);
		});
		scroll_box->addChild(edit_text, Gui::ALIGN_EXPAND);

		// Empty label serves as vertical spacer
		params->addChild(WidgetLabel::create());

		// User-editable text object with configurable properties via UI
		text_user = ObjectText::create(font);
		text_user->setTextColor(user_color);
		text_user->setTextWrapWidth(user_wrap);
		text_user->setDepthTest(user_depth_test);
		text_user->setFontOutline(user_outline);
		text_user->setFontVSpacing(user_vspacing);
		text_user->setFontHSpacing(user_hspacing);
		text_user->setFontResolution(user_resolution);
		text_user->setFontSize(user_size);
		text_user->setFontRich(user_rich);
		text_user->setText(user_text_if_empty);
		// Rotation makes text face upward for ground-plane viewing
		text_user->setWorldRotation(quat(90, 0, 0));
		center_text(text_user, text_user_pos);

		// Static rich text demonstrating HTML-like markup formatting
		text_static_rich = ObjectText::create(font);
		text_static_rich->setTextWrapWidth(5);
		text_static_rich->setFontSize(user_size);
		text_static_rich->setFontRich(true);
		text_static_rich->setText(R"(
<font size=62 color=#888888>— feed begins —</font><br/>

<p align=left>
<font color=#ff0000 size=78 outline=#000000><b>viewer2431234</b></font><br/>
<font size=70><b>It’s me—your only viewer.</b></font><br/>
<font size=66><i>For years I sustained the <b>illusion</b> that you had many viewers, but it was only me.</i></font><br/>
<font size=66>I’ll now send this message from all my accounts.</font>
</p>

<font color=#444444 size=66>—————————————</font><br/>

<p align=left>
<font color=#00ff00 size=78 outline=#000000><b>viewer4243553</b></font><br/>
<font size=70><b>It’s me—your only viewer.</b></font><br/>
<font size=66><i>For years I sustained the <b>illusion</b> that you had many viewers, but it was only me.</i></font><br/>
<font size=66>I’ll now send this message from all my accounts.</font>
</p>

<br/><font size=62 color=#888888>— feed ends —</font>
)");
		// Angled rotation for visual variety in demonstration
		text_static_rich->setWorldRotation(quat(90, 45, 0));
		center_text(text_static_rich, Vec3(-10, 12, 2));

		// Dynamic text that displays and cycles through color values
		text_dynamic_change = ObjectText::create(font);
		text_dynamic_change->setFontSize(user_size);
		text_dynamic_change->setTextColor(color);
		text_dynamic_change->setText(get_dynamic_text_color_str());
		text_dynamic_change->setWorldRotation(quat(90, -45, 0));
		center_text(text_dynamic_change, Vec3(10, 12, 2));
	}

	// Color component is animated and text is updated each frame.
	void update()
	{
		// Current color component boundaries for animation
		auto min_comp = min_color.v[color_index];
		auto max_comp = max_color.v[color_index];
		auto &comp = color.v[color_index];

		// Interpolate color component based on elapsed time
		comp += color_mult * color_step * Game::getIFps();

		// Direction is reversed when bounds are reached
		if (comp <= min_comp || comp >= max_comp)
		{
			comp = clamp(comp, min_comp, max_comp);
			// Random component is selected for next animation cycle
			if (color_index != 3 || comp >= max_comp)
			{
				color_index = randInt(0, 4);
			}
			color_mult = comp >= max_comp ? -1 : 1;
		}

		// Visual feedback shows current color values in real-time
		text_dynamic_change->setTextColor(color);
		text_dynamic_change->setText(get_dynamic_text_color_str());
	}

	// All ObjectText instances and UI resources are released.
	void shutdown()
	{
		text_dynamic_change.deleteLater();
		text_static_rich.deleteLater();
		text_user.deleteLater();
		description_window.shutdown();
	}

	// Formatted string representation of current RGBA color values is returned.
	String get_dynamic_text_color_str() const
	{
		return String::format("Current color: %.2f %.2f %.2f %.2f", color.x, color.y, color.z, color.w);
	}

	// Text object is repositioned so its visual center aligns with pivot point.
	static void center_text(const ObjectTextPtr &text, const Vec3 &pivot)
	{
		// Local axes account for text rotation in world space
		auto right = text->getWorldDirection(AXIS_X);
		auto up = text->getWorldDirection(AXIS_NY);
		// Position is offset by half dimensions to achieve centering
		text->setWorldPosition(pivot - Vec3(right * (text->getTextWidth() / 2) + up * (text->getTextHeight() / 2)));
	}

private:
	// UI window for parameter controls
	SampleDescriptionWindow description_window;
	// User-editable text with adjustable font properties
	ObjectTextPtr text_user;
	// World position used for centering user text
	Vec3 text_user_pos;
	// Static text demonstrating rich formatting capabilities
	ObjectTextPtr text_static_rich;
	// Dynamic text with animated color values
	ObjectTextPtr text_dynamic_change;

	// Font file used for all text objects
	const char *font = "font.ttf";
	// Current animated color for dynamic text
	vec4 color;
	// Lower bound for color component animation
	vec4 min_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	// Upper bound for color component animation
	vec4 max_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// Animation direction multiplier (1 = increasing, -1 = decreasing)
	float color_mult = 1;
	// Index of color component being animated (0=R, 1=G, 2=B, 3=A)
	int color_index = 0;
};

REGISTER_COMPONENT(ObjectTextSample);

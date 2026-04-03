// Bouncing "No Signal" label for classic screen saver effect.
// Uses GuiToTexture with auto-update enabled, so only widget state
// needs updating while texture re-rendering happens automatically.

#pragma once

#include <UnigineComponentSystem.h>

class GuiToTexture;

// Animates a bouncing label that reflects off canvas edges.
class WidgetNoSignal final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(WidgetNoSignal, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Movement speed in pixels per second
	PROP_PARAM(Float, label_speed, 1000.f, nullptr, nullptr, nullptr, "min=0.1;max=3000");

private:
	void init();
	void update();

	void reflect_direction();

	static Unigine::Math::vec2 reflect_vector(const Unigine::Math::vec2 &vector,
		const Unigine::Math::vec2 &normal);

private:
	// Current position on the canvas
	Unigine::Math::ivec2 position{};

	// Container widget with background color
	Unigine::WidgetVBoxPtr container{};
	// Text label displaying "No Signal"
	Unigine::WidgetLabelPtr label;

	// Sub-pixel movement accumulator for smooth animation
	Unigine::Math::vec2 accumulated_delta{};
	// Normalized movement direction vector
	Unigine::Math::vec2 direction{1.f, 1.f};
	Unigine::Math::vec2 start_position;
};
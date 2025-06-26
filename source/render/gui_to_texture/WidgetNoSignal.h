#pragma once

#include <UnigineComponentSystem.h>


class GuiToTexture;

/// This sample demonstrates how the GuiToTexture component can be used to render custom widgets
/// In this component we're going to use GuiToTexture with auto update enabled and will update the widget state only
class WidgetNoSignal final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(WidgetNoSignal, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, label_speed, 1000.f, nullptr, nullptr, nullptr, "min=0.1;max=3000");

private:
	void init();
	void update();

	void reflect_direction();

	static Unigine::Math::vec2 reflect_vector(const Unigine::Math::vec2 &vector,
		const Unigine::Math::vec2 &normal);

private:

	Unigine::Math::ivec2 position{};

	Unigine::WidgetVBoxPtr container{};
	Unigine::WidgetLabelPtr label;

	Unigine::Math::vec2 accumulated_delta{};
	Unigine::Math::vec2 direction{1.f, 1.f};
	Unigine::Math::vec2 start_position;
};
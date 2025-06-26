#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>

class SimpleInformationBox;

class TouchInput : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TouchInput, Unigine::ComponentBase);

	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	class CanvasWithCircles
	{
	public:
		CanvasWithCircles();
		~CanvasWithCircles();

		void addCircle(int x, int y, int num, int color__num, Unigine::String str);
		void clear() { canvas->clear(); }

	private:
		Unigine::WidgetCanvasPtr canvas;

		const static int number_of_colors = 10;

		Unigine::Math::vec4 colors[number_of_colors] = {
			Unigine::Math::vec4(1.0f, 0.0f, 0.0f, 1.0f),
			Unigine::Math::vec4(0.0f, 1.0f, 0.0f, 1.0f),
			Unigine::Math::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			Unigine::Math::vec4(1.0f, 1.0f, 0.0f, 1.0f),
			Unigine::Math::vec4(0.0f, 1.0f, 1.0f, 1.0f),
			Unigine::Math::vec4(1.0f, 0.0f, 1.0f, 1.0f),
			Unigine::Math::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			Unigine::Math::vec4(0.5f, 0.0f, 0.0f, 1.0f),
			Unigine::Math::vec4(0.0f, 0.5f, 0.0f, 1.0f),
			Unigine::Math::vec4(0.0f, 0.0f, 0.5f, 1.0f), };

		float radius;
	};

	CanvasWithCircles *canvas = nullptr;
	SimpleInformationBox *info = nullptr;
};
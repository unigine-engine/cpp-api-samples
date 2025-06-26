#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineFields.h>
#include <UnigineGame.h>


class DynamicTextureFieldHeight : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(DynamicTextureFieldHeight, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

public:
	void setImageSize(int size);
	void setFrequency(float frequency);
	void setSpeed(float speed);
	void setAmplitude(float power);
	void setWidgetSprite(Unigine::WidgetSpritePtr sprite);

private:
	void init();
	void update();

	int size_map{ 1024 };
	float speed{ 1.0f };
	float frequency{ 1.0f };
	int amplitude{ 127 };

	Unigine::ImagePtr image;
	Unigine::WidgetSpritePtr sprite;
	Unigine::FieldHeightPtr instance;
	Unigine::TexturePtr texture;
	Unigine::ObjectGuiPtr gui;
};
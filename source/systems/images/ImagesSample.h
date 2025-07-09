#pragma once

#include <UnigineComponentSystem.h>

class ImagesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ImagesSample, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

	void image_init();
	void image_update();


private:
	float size{2.f};
	float velocity{1.f};
	float radius{0.5f};
	int num_fields{16};

	Unigine::Vector<Unigine::Math::vec3> positions;
	Unigine::Vector<Unigine::Math::vec3> velocities;
	Unigine::Vector<float> radiuses;


	Unigine::ImagePtr image;
	Unigine::MaterialPtr material;
};

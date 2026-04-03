#pragma once

#include <UnigineComponentSystem.h>

class ImagesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ImagesSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates creating and updating a 3D image in real time by "
							"writing raw pixel data and applying it as a texture to a volume object.")

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)

private:
	void init();
	void update();

	// Initialize and update fields that generate voxel density
	void image_init();
	void image_update();


private:
	// Simulation parameters
	float size{2.f};
	float velocity{1.f};
	float radius{0.5f};
	int num_fields{16};

	// State of moving fields
	Unigine::Vector<Unigine::Math::vec3> positions;
	Unigine::Vector<Unigine::Math::vec3> velocities;
	Unigine::Vector<float> radiuses;

	// Texture and material used for visualization
	Unigine::ImagePtr image;
	Unigine::MaterialPtr material;
};

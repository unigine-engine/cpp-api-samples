#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class EulerAnglesSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EulerAnglesSample, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, plane)

private:
	enum ROTATION_SEQUENCE
	{
		XYZ,
		XZY,
		YXZ,
		YZX,
		ZXY,
		ZYX,
	};

	void init();
	void update();
	void shutdown();

	void init_gui();

	void update_rotation();
	void update_decomposition_angles();


	Unigine::Math::vec3 euler_angles = Unigine::Math::vec3_zero;
	Unigine::Math::vec3 decomposition_angles = Unigine::Math::vec3_zero;
	ROTATION_SEQUENCE composition_sequence = XYZ;
	ROTATION_SEQUENCE decomposition_sequence = XYZ;
	SampleDescriptionWindow sample_description_window;
	Unigine::String status;
	bool visualizer_enabled = false;
};

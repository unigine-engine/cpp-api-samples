#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Demonstrates Euler angle rotation composition and decomposition.
// Shows all 6 rotation sequences (XYZ, XZY, YXZ, YZX, ZXY, ZYX) and how
// the same rotation can be represented differently depending on sequence.
// Visualizes rotation axes and gimbal rings in 3D space.
class EulerAnglesSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EulerAnglesSample, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, plane) // Target node to rotate

private:
	// Supported Euler rotation sequences
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

	void init_gui(); // Creates sliders and combo boxes for angle/sequence control

	void update_rotation();           // Applies Euler angles to node using composition sequence
	void update_decomposition_angles(); // Extracts Euler angles from current rotation

	Unigine::Math::vec3 euler_angles = Unigine::Math::vec3_zero;        // Input angles (pitch, roll, yaw)
	Unigine::Math::vec3 decomposition_angles = Unigine::Math::vec3_zero; // Angles extracted from rotation
	ROTATION_SEQUENCE composition_sequence = XYZ;   // Sequence for building rotation
	ROTATION_SEQUENCE decomposition_sequence = XYZ; // Sequence for extracting angles
	SampleDescriptionWindow sample_description_window;
	Unigine::String status;
	bool visualizer_enabled = false; // Original visualizer state to restore on shutdown
};

#pragma once

#include "UnigineMathLib.h"
namespace Utils {
	Unigine::Math::Vec3 catmullRomCentripetal(
		const Unigine::Math::Vec3& p0,
		const Unigine::Math::Vec3& p1,
		const Unigine::Math::Vec3& p2,
		const Unigine::Math::Vec3& p3,
		float t /* between 0 and 1 */,
		float alpha = .5f /* between 0 and 1 */);

	Unigine::Vector<float> getLengthCatmullRomCentripetal(
		const Unigine::Math::Vec3& p0,
		const Unigine::Math::Vec3& p1,
		const Unigine::Math::Vec3& p2,
		const Unigine::Math::Vec3& p3,
		int subdivisions);

	Unigine::Math::Vec3 catmullRomUniform(
		const Unigine::Math::Vec3& p0,
		const Unigine::Math::Vec3& p1,
		const Unigine::Math::Vec3& p2,
		const Unigine::Math::Vec3& p3,
		float t);

	Unigine::Vector<float> getLengthCatmullRomUniform(
		const Unigine::Math::Vec3& p0,
		const Unigine::Math::Vec3& p1,
		const Unigine::Math::Vec3& p2,
		const Unigine::Math::Vec3& p3,
		int subdivisions);

	Unigine::Math::quat squad(
		const Unigine::Math::quat& q0,
		const Unigine::Math::quat& q1,
		const Unigine::Math::quat& q2,
		const Unigine::Math::quat& q3,
		float t);

float mapRange(float input, float in_min, float in_max, float out_min, float out_max,
	bool clamp = true);

}
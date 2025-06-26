#include "Math.h"

using namespace Unigine;
using namespace Math;

namespace {
	float get_t(float t, float alpha, const Vec3& p0, const Vec3& p1)
	{
		auto d = p1 - p0;
		float a = (float)dot(d, d);
		float b = powf(a, alpha * .5f);
		return (b + t);
	}
}

Vec3 Utils::catmullRomCentripetal(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t /* between 0 and 1 */, float alpha/* between 0 and 1 */)
{
	float t0 = 0.0f;
	float t1 = get_t(t0, alpha, p0, p1);
	float t2 = get_t(t1, alpha, p1, p2);
	float t3 = get_t(t2, alpha, p2, p3);
	t = lerp(t1, t2, t);
	Vec3 A1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
	Vec3 A2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
	Vec3 A3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));
	Vec3 B1 = A1 * ((t2 - t) / (t2 - t0)) + A2 * ((t - t0) / (t2 - t0));
	Vec3 B2 = A2 * ((t3 - t) / (t3 - t1)) + A3 * ((t - t1) / (t3 - t1));
	Vec3 C = B1 * ((t2 - t) / (t2 - t1)) + B2 * ((t - t1) / (t2 - t1));
	return C;
}

Vector<float> Utils::getLengthCatmullRomCentripetal(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, int subdivisions)
{
	Vector<float> res;
	Vec3 start = Utils::catmullRomCentripetal(p0, p1, p2, p3, 0);
	for (int i = 1; i < subdivisions; i++)
	{
		Vec3 end = Utils::catmullRomCentripetal(p0, p1, p2, p3, float(i) / (subdivisions - 1));
		res.emplace_back(float(length(end - start)));
		start = end;
	}
	return res;
}

Vec3 Utils::catmullRomUniform(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t)
{
	Vec3 a = p1 * 2.0f;
	Vec3 b = p2 - p0;
	Vec3 c = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3;
	Vec3 d = -p0 + p1 * 3.0f - p2 * 3.0f + p3;
	return (a + b * t + c * t * t + d * t * t * t) * 0.5f;
}

Vector<float> Utils::getLengthCatmullRomUniform(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, int subdivisions)
{
	Vector<float> res;
	Vec3 start = catmullRomUniform(p0, p1, p2, p3, 0);
	for (int i = 1; i < subdivisions; i++)
	{
		Vec3 end = catmullRomUniform(p0, p1, p2, p3, float(i) / (subdivisions - 1));
		res.emplace_back(float(length(end - start)));
		start = end;
	}
	return res;
}

quat Utils::squad(const quat &q0, const quat &q1, const quat &q2, const quat &q3, float t)
{
	auto qexp = [](const quat &q) {
		float a = Math::fsqrt(q.x * q.x + q.y * q.y + q.z * q.z);
		float sin_a = Math::sin(a);
		vec4 r;
		r.w = Math::cos(a);
		if (Math::abs(sin_a) >= 1.0e-15)
		{
			float coeff = sin_a / a;
			r.x = q.x * coeff;
			r.y = q.y * coeff;
			r.z = q.z * coeff;
		}
		return quat(r);
	};
	auto qlog = [](const quat &q) {
		float a = Math::acos(q.w);
		float sin_a = Math::sin(a);
		if (Math::abs(sin_a) >= 1.0e-15)
		{
			float coeff = a / sin_a;
			vec4 r = vec4(q.x * coeff, q.y * coeff, q.z * coeff, 0);
			return quat(r);
		}
		return quat(vec4_zero);
	};
	auto slerp_no_invert = [](const quat &q0, const quat &q1, float t) {
		float d = dot(q0, q1);
		if (Math::abs(d) > 0.9999f)
			return q0;

		float theta = Math::acos(d);
		float sin_t = 1.0f / Math::sin(theta);
		float t0 = Math::sin((1.0f - t) * theta) * sin_t;
		float t1 = Math::sin(t * theta) * sin_t;
		return (q0 * t0 + q1 * t1); //.normalizeValid();
	};

	// modify quaternions for shortest path
	auto get_length = [](const quat &q) {
		return Math::fsqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	};
	quat q0m = get_length(q1 - q0) < get_length(q1 + q0) ? q0 : -q0;
	quat q2m = get_length(q1 - q2) < get_length(q1 + q2) ? q2 : -q2;
	quat q3m = get_length(q2m - q3) < get_length(q2m + q3) ? q3 : -q3;

	// calculate helper quaternions (tangent values)
	// https://www.geometrictools.com/Documentation/Quaternions.pdf (page 9 [31])
	quat q1inv = inverse(q1);
	quat q2inv = inverse(q2m);
	quat a = (q1 * qexp((qlog(q1inv * q2m) + qlog(q1inv * q0m)) * -0.25f));
	quat b = (q2m * qexp((qlog(q2inv * q3m) + qlog(q2inv * q1)) * -0.25f));

	return slerp(slerp(q1, q2m, t), slerp(a, b, t), 2.0f * t * (1.0f - t)).normalizeValid();
}

float Utils::mapRange(float input, float in_min, float in_max, float out_min, float out_max,
	bool clamp)
{
	if (in_min == in_max)
		return out_min;

	if (clamp)
	{
		float lower_bound = Unigine::Math::min(in_min, in_max);
		float upper_bound = Unigine::Math::max(in_min, in_max);
		input = Unigine::Math::clamp(input, lower_bound, upper_bound);
	}

	float normalized = (input - in_min) / (in_max - in_min);

	return normalized * (out_max - out_min) + out_min;
}
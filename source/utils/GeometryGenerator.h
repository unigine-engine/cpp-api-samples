#pragma once
#include <UnigineObjects.h>
#include <UnigineMathLibCommon.h>

namespace GeometryGenerator
{
struct SplineMeshUVSettings
{
	float length_tiling_amount = 5;
	float width_tiling_amount = 5;
	bool stretch_along_length = false;
	bool stretch_along_width = false;

};

Unigine::MeshPtr CreatePolygonMesh(Unigine::Vector<Unigine::Math::Vec3> points, bool make_flat = false);

bool computeSplinePoints(Unigine::Vector<Unigine::Math::Vec3>& in_points, Unigine::Vector<Unigine::Math::Vec3>& out_points, float segment_length);
Unigine::MeshPtr generateSplineMesh(const Unigine::Vector<Unigine::Math::Vec3>& spline_points, float width, int width_subdivisions, float mesh_segment_length, const SplineMeshUVSettings& uv_settings);
Unigine::MeshPtr generateSplineMeshSquare(const Unigine::Vector<Unigine::Math::Vec3>& spline_points, float width, int width_subdivisions, float mesh_segment_length, const SplineMeshUVSettings& uv_settings);
Unigine::MeshPtr generateSplineMeshRound(const Unigine::Vector<Unigine::Math::Vec3>& spline_points, float radius, int width_subdivisions, float mesh_segment_length, const SplineMeshUVSettings& uv_settings);
};
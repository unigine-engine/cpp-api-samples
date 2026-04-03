#include "GeometryGenerator.h"
#include "../utils/Math.h"
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

namespace GeometryGenerator
{

Unigine::MeshPtr CreatePolygonMesh(Unigine::Vector<Unigine::Math::Vec3> points, bool make_flat)
{
	Vector<unsigned short> indices = Vector<unsigned short>();
	Vector<vec2> vertices(points.size());
	WorldBoundBox bb = WorldBoundBox(points.get(), points.size());
	auto bbCenter = bb.getCenter();
	for (int index = 0; index < points.size(); index++)
	{
		auto centeredPoint = points[index] - bbCenter;
		vertices[index] = vec2(centeredPoint.x, centeredPoint.y);
	}
	triangulatePolygon(vertices, indices);
	if (indices.empty())
	{
		Log::warning("GeometryGenerator::CreatePolygonMesh: triangulation failed - figure points form a self-intersecting or degenerate polygon. Check figure point positions.\n");
		return nullptr;
	}
	MeshPtr mesh = Mesh::create();
	mesh->addSurface("mesh");
	for (int index = 0; index < vertices.size(); index++)
	{
		if (make_flat)
		{
			mesh->addVertex(vec3(vertices[index].x, vertices[index].y, 0.0f));
		} else
		{
			mesh->addVertex(vec3(vertices[index].x, vertices[index].y, points[index].z - bbCenter.z));
		}
		mesh->addTexCoord0(vec2(vertices[index].x, vertices[index].y));
		mesh->addNormal(vec3_up, 0);

	}
	for (int index = indices.size() - 1; index >= 0; index--)// reversed so that triangles are facing up
	{
		mesh->addIndex(indices[index]);
	}
	mesh->createBounds();
	mesh->createTangents();
	return mesh;
}

bool computeSplinePoints(Unigine::Vector<Unigine::Math::Vec3>& in_points, Unigine::Vector<Unigine::Math::Vec3>& out_points, float segment_length)
{
	if (in_points.size() < 2)
	{
		Log::warning("MeshObjectGenerator::GenerateObjectMeshStaticLine invalid points number");
		return false;
	}

	out_points.clear();
	//======================Creating spline==========================
	//computing spline points with Centripetal Catmull-Rom spline
	double leftover_segment_length = 0.0f;
	for (int index = 1; index < in_points.size(); index++)
	{
		Vec3 p1 = in_points[index - 1];
		Vec3 p2 = in_points[index];
		Vec3 p0 = index - 2 >= 0 ? in_points[index - 2] : p1 + normalize(p1 - p2);
		Vec3 p3 = index + 1 < in_points.size() ? in_points[index + 1] : p2 + normalize(p2 - p1);
		Vector<float> lengths = Utils::getLengthCatmullRomCentripetal(p0, p1, p2, p3, 1000);
		double current_segment_length = 0.0;
		for (auto& l : lengths)
		{
			if (Math::isNan(l))
			{
				l = 0;
			}

			current_segment_length += l;
		}

		if (leftover_segment_length > current_segment_length)
		{
			leftover_segment_length += current_segment_length;
			continue;
		} else
		{
			//handles all first points in meshSegments and taking into account leftover segment length. So that all segments have same length
			float leftover_t = leftover_segment_length / current_segment_length;
			Vec3 point = Utils::catmullRomCentripetal(p0, p1, p2, p3, leftover_t);
			out_points.append(point);
		}

		int meshSegmentsCount = ceil((current_segment_length - leftover_segment_length) / segment_length);

		for (int row = 1; row < meshSegmentsCount; row++) //leaving last meshSegment for next meshSegment or end of generation
		{
			float t = (leftover_segment_length + row * segment_length) / current_segment_length; //getting point position on spline based on the segment legth
			Vec3 point = Utils::catmullRomCentripetal(p0, p1, p2, p3, t);
			out_points.append(point);
		}

		if (current_segment_length < leftover_segment_length + (meshSegmentsCount)*segment_length)
		{
			leftover_segment_length = (meshSegmentsCount)*segment_length + leftover_segment_length - current_segment_length;
		} else
		{
			leftover_segment_length = 0.0;
		}
	}
	out_points.append(in_points.last());
	return true;
}

Unigine::MeshPtr generateSplineMesh(const Unigine::Vector<Unigine::Math::Vec3>& spline_points, float width, int width_subdivisions, float mesh_segment_length, const SplineMeshUVSettings& uv_settings)
{
	if (spline_points.size() < 2)
	{
		Log::warning("GenerateSplineMesh invalid points number");
		return nullptr;
	}
	double spline_length = 0.0;
	for (size_t index = 1; index < spline_points.size(); index++)
	{
		spline_length += (spline_points[index ] - spline_points[index - 1]).length();
	}

	float half_width = width * 0.5f;
	int vertex_column_number = width_subdivisions + 1;
	int vertex_row_number = spline_points.size();
	float mesh_segment_width = width / width_subdivisions;
	int vertex_number = vertex_column_number * vertex_row_number;

	MeshPtr mesh = Mesh::create();
	mesh->addSurface("spline");
	mesh->setNumVertex(vertex_number);
	mesh->setNumIndices((vertex_row_number - 1) * (vertex_column_number - 1) * 6);
	mesh->setNumNormals(vertex_number);
	mesh->setNumTexCoords0(vertex_number);
	mesh->setNumTexCoords1(vertex_number);
	float distance = 0;
	for (int row = 0; row < vertex_row_number; row++)
	{
		vec3 current_vertex = vec3(spline_points[row]);
		vec3 dir_next = (row + 1 < spline_points.size()) ?
			vec3(spline_points[row + 1] - spline_points[row]) :
			vec3(spline_points[row] - spline_points[row - 1]);
		vec3 dir_prev = (row > 0) ?
			vec3(spline_points[row - 1] - spline_points[row]) :
			-dir_next;

		float current_segment_length = length(dir_next);
		dir_next.z = 0;
		dir_prev.z = 0;
		dir_next.normalizeValid();
		dir_prev.normalizeValid();

		vec3 bissectrice = dir_next + dir_prev;
		bissectrice.normalizeValid();

		//checking right perpendicular and bissectrice for directing the same way
		vec3 right = cross(dir_next, vec3_up);
		auto dotSign = dot(right, bissectrice);

		//using bissectrice for better mesh turns generation
		if (abs(dotSign) < Math::Consts::EPS)
		{
			right = right.normalizeValid();
		} else if (dotSign > 0)
		{//to the right of the dir_next
			right = bissectrice;
		} else
		{
			right = -bissectrice;
		}

		vec3 start_point_left = current_vertex - right * half_width;
		for (int column = 0; column < vertex_column_number; column++)
		{
			int current_index = row * vertex_column_number + column;
			vec3 point = vec3_zero;
			float width_offset = column * mesh_segment_width;
			if (width_offset > width)
			{
				point = start_point_left + right * width;
				width_offset = width;
			} else
			{
				point = start_point_left + right * width_offset;
			}

			mesh->setVertex(current_index, point);
			float u = 0.0f;
			float v = 0.0f;
			if (uv_settings.stretch_along_width)
			{
				u = width_offset / width;
			} else
			{
				u = (width_offset / width - 0.5) * uv_settings.width_tiling_amount + 0.5f;
			}
			if (uv_settings.stretch_along_length)
			{
				v = distance / spline_length;
			} else
			{
				v = distance * uv_settings.length_tiling_amount;
			}
			mesh->setTexCoord0(current_index, vec2(u, v));
			mesh->setTexCoord1(current_index, vec2(u, v));
			mesh->setNormal(current_index, cross(right, dir_next));
		}
		distance += current_segment_length;
	}
	int index = 0;
	for (int row = 1; row < vertex_row_number; row++)
	{
		for (int column = 1; column < vertex_column_number; column++)
		{
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column - 1);//bottom left
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);//bottom right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);//top left

			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);//bottom right
			mesh->setIndex(index++, row * vertex_column_number + column);//top right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);//top left
		}
	}

	mesh->createTangents();
	mesh->createBounds();
	return mesh;
}

// Rodrigues' rotation formula: rotates vector v around axis by angle (radians)
static vec3 rotateVectorAroundAxis(const vec3 &v, const vec3 &axis, float angle)
{
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	return v * cos_angle +
		   cross(axis, v) * sin_angle +
		   axis * dot(axis, v) * (1.0f - cos_angle);
}

// Computes Parallel Transport Frames along the spline to minimize twist artifacts
static void computeParallelTransportFrames(const Vector<vec3> &points,
										   Vector<vec3> &tangents,
										   Vector<vec3> &normals,
										   Vector<vec3> &binormals)
{
	int num_points = points.size();

	tangents.clear();
	normals.clear();
	binormals.clear();

	// Compute tangents at each point
	for (int i = 0; i < num_points; i++)
	{
		vec3 tangent;
		if (i == 0)
			tangent = normalize(points[1] - points[0]);
		else if (i == num_points - 1)
			tangent = normalize(points[i] - points[i - 1]);
		else
			tangent = normalize(points[i + 1] - points[i - 1]);

		tangents.append(tangent);
	}

	// Initialize the first frame with an arbitrary perpendicular vector
	vec3 arbitrary = (abs(tangents[0].x) > 0.1f) ? vec3(0, 1, 0) : vec3(1, 0, 0);
	vec3 first_normal = normalize(cross(tangents[0], arbitrary));
	vec3 first_binormal = normalize(cross(tangents[0], first_normal));

	normals.append(first_normal);
	binormals.append(first_binormal);

	// Propagate frames using parallel transport
	for (int i = 1; i < num_points; i++)
	{
		vec3 prev_tangent = tangents[i - 1];
		vec3 curr_tangent = tangents[i];

		vec3 rotation_axis = cross(prev_tangent, curr_tangent);
		float rotation_axis_length = rotation_axis.length();

		vec3 new_normal, new_binormal;

		// If tangents are nearly parallel, reuse the previous frame
		if (rotation_axis_length < 0.0001f)
		{
			new_normal = normals[i - 1];
			new_binormal = binormals[i - 1];
		} else
		{
			rotation_axis = normalize(rotation_axis);
			float angle = acos(clamp(dot(prev_tangent, curr_tangent), -1.0f, 1.0f));

			new_normal = rotateVectorAroundAxis(normals[i - 1], rotation_axis, angle);
			new_binormal = rotateVectorAroundAxis(binormals[i - 1], rotation_axis, angle);
		}

		// Re-orthonormalize for numerical stability
		new_binormal = normalize(cross(curr_tangent, new_normal));
		new_normal = normalize(cross(new_binormal, curr_tangent));

		normals.append(new_normal);
		binormals.append(new_binormal);
	}
}

Unigine::MeshPtr generateSplineMeshSquare(const Unigine::Vector<Unigine::Math::Vec3>& spline_points, float width, int width_subdivisions, float mesh_segment_length, const SplineMeshUVSettings& uv_settings)
{
	if (spline_points.size() < 2)
	{
		Log::warning("GenerateSplineMesh invalid points number");
		return nullptr;
	}
	double spline_length = 0.0;
	for (size_t index = 1; index < spline_points.size(); index++)
	{
		spline_length += (spline_points[index ] - spline_points[index - 1]).length();
	}

	float half_width = width * 0.5f;
	int side_column_number = width_subdivisions + 1; //number of columns on 1 side
	int vertex_column_number = side_column_number * 4;
	int vertex_row_number = spline_points.size();
	float mesh_segment_width = width / width_subdivisions;
	int vertex_number = vertex_column_number * vertex_row_number;

	MeshPtr mesh = Mesh::create();
	mesh->addSurface("spline");
	mesh->setNumVertex(vertex_number);
	mesh->setNumIndices((vertex_row_number - 1) * (vertex_column_number - 1) * 6);
	mesh->setNumNormals(vertex_number);
	mesh->setNumTexCoords0(vertex_number);
	mesh->setNumTexCoords1(vertex_number);

	// Convert spline points to vec3 for PTF computation
	Vector<vec3> points_f(spline_points.size());
	for (int i = 0; i < spline_points.size(); i++)
		points_f[i] = vec3(spline_points[i]);

	// Compute Parallel Transport Frames along the spline
	Vector<vec3> tangents, ptf_normals, ptf_binormals;
	computeParallelTransportFrames(points_f, tangents, ptf_normals, ptf_binormals);

	// 4 sides of the square: each side has a direction and a face normal in the local frame
	// Side 0: top    (-n*hw + b*hw) → (+n*hw + b*hw), face normal = +b
	// Side 1: right  (+n*hw + b*hw) → (+n*hw - b*hw), face normal = +n
	// Side 2: bottom (+n*hw - b*hw) → (-n*hw - b*hw), face normal = -b
	// Side 3: left   (-n*hw - b*hw) → (-n*hw + b*hw), face normal = -n
	//n — normal b — binormal hw — half_width
	struct SideDesc
	{
		vec2 start;      // corner offset in (n, b) space
		vec2 direction;  // side direction in (n, b) space
		vec2 face_normal; // outward normal in (n, b) space
	};
	const SideDesc sides[4] = {
		{{-1, 1}, { 1, 0}, { 0, 1}},  // top
		{{ 1, 1}, { 0,-1}, { 1, 0}},  // right
		{{ 1,-1}, {-1, 0}, { 0,-1}},  // bottom
		{{-1,-1}, { 0, 1}, {-1, 0}},  // left
	};

	float distance = 0;
	for (int row = 0; row < vertex_row_number; row++)
	{
		vec3 center = points_f[row];
		vec3 n = ptf_normals[row];
		vec3 b = ptf_binormals[row];

		float segment_length = (row + 1 < vertex_row_number)
			? length(vec3(spline_points[row + 1] - spline_points[row]))
			: length(vec3(spline_points[row] - spline_points[row - 1]));

		for (int side = 0; side < 4; side++)
		{
			const SideDesc &sd = sides[side];
			vec3 corner = center + n * (sd.start.x * half_width) + b * (sd.start.y * half_width);
			vec3 side_dir = n * sd.direction.x + b * sd.direction.y;
			vec3 face_normal = n * sd.face_normal.x + b * sd.face_normal.y;

			for (int side_column = 0; side_column < side_column_number; side_column++)
			{
				int idx = row * vertex_column_number + side * side_column_number + side_column;
				float vertex_offset = side_column * mesh_segment_width;
				vec3 point = corner + side_dir * vertex_offset;
				mesh->setVertex(idx, point);
				mesh->setNormal(idx, face_normal);

				float u = 0.0f;
				float v = 0.0f;
				if (uv_settings.stretch_along_width)
				{
					u = side + vertex_offset / width;
				} else
				{
					u = (vertex_offset / width - 0.5f) * uv_settings.width_tiling_amount + 0.5f;
				}
				if (uv_settings.stretch_along_length)
				{
					v = distance / spline_length;
				} else
				{
					v = distance * uv_settings.length_tiling_amount;
				}
				mesh->setTexCoord0(idx, vec2(u, v));
				mesh->setTexCoord1(idx, vec2(u, v));
			}
		}
		distance += segment_length;
	}
	int index = 0;
	for (int row = 1; row < vertex_row_number; row++)
	{
		//column indexes arrangement in one row
		//0--1
		//|  |
		//3__2
		for (int column = 1; column < vertex_column_number; column++)
		{
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column - 1);//bottom left
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);//bottom right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);//top left

			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);//bottom right
			mesh->setIndex(index++, row * vertex_column_number + column);//top right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);//top left
		}
	}

	mesh->createTangents();
	mesh->createBounds();
	mesh->optimizeIndices(Mesh::VERTEX_CACHE);
	return mesh;
}

Unigine::MeshPtr generateSplineMeshRound(const Unigine::Vector<Unigine::Math::Vec3> &spline_points, float radius, int radial_segments, float seg_len, const SplineMeshUVSettings &uv_settings)
{
	if (spline_points.size() < 2)
	{
		Log::warning("GenerateSplineMesh invalid points number");
		return nullptr;
	}

	double spline_length = 0.0;
	for (size_t index = 1; index < spline_points.size(); index++)
	{
		spline_length += (spline_points[index] - spline_points[index - 1]).length();
	}

	// Convert spline points to vec3 for PTF computation
	Vector<vec3> points_f(spline_points.size());
	for (int i = 0; i < spline_points.size(); i++)
		points_f[i] = vec3(spline_points[i]);

	// Compute Parallel Transport Frames along the spline
	Vector<vec3> tangents, normals, binormals;
	computeParallelTransportFrames(points_f, tangents, normals, binormals);

	float circle_length = 2 * Math::Consts::PI * radius;
	int vertex_column_number = radial_segments + 1;
	int vertex_row_number = spline_points.size();
	int vertex_number = vertex_column_number * vertex_row_number;
	float angle_step = Math::Consts::PI * 2.0f / radial_segments;

	// Precompute unit circle offsets (cos/sin) once, reuse for every row
	Vector<vec2> circle_offsets(vertex_column_number);
	for (int column = 0; column < vertex_column_number; column++)
	{
		float angle = angle_step * column;
		circle_offsets[column] = vec2(cos(angle) * radius, sin(angle) * radius);
	}

	MeshPtr mesh = Mesh::create();
	mesh->addSurface("spline");
	mesh->setNumVertex(vertex_number);
	mesh->setNumIndices((vertex_row_number - 1) * (vertex_column_number - 1) * 6);
	mesh->setNumNormals(vertex_number);
	mesh->setNumTexCoords0(vertex_number);
	mesh->setNumTexCoords1(vertex_number);

	float distance = 0;

	for (int row = 0; row < vertex_row_number; row++)
	{
		vec3 center = points_f[row];
		vec3 n = normals[row];
		vec3 b = binormals[row];

		float segment_length = (row + 1 < vertex_row_number)
			? length(vec3(spline_points[row + 1] - spline_points[row]))
			: length(vec3(spline_points[row] - spline_points[row - 1]));

		for (int column = 0; column < vertex_column_number; column++)
		{
			int idx = row * vertex_column_number + column;

			// Circle point in the local frame defined by normal and binormal
			vec3 point = center + n * circle_offsets[column].x + b * circle_offsets[column].y;
			mesh->setVertex(idx, point);

			// Normal points outward from center
			vec3 vertex_normal = normalizeValid(point - center);
			mesh->setNormal(idx, vertex_normal);

			float u = 0.0f;
			float v = 0.0f;
			float width_fraction = (float)column / radial_segments;
			if (uv_settings.stretch_along_width)
			{
				u = width_fraction;
			} else
			{
				u = (width_fraction - 0.5f) * uv_settings.width_tiling_amount + 0.5f;
			}
			if (uv_settings.stretch_along_length)
			{
				v = distance / spline_length;
			} else
			{
				v = distance * uv_settings.length_tiling_amount;
			}
			mesh->setTexCoord0(idx, vec2(u, v));
			mesh->setTexCoord1(idx, vec2(u, v));
		}
		distance += segment_length;
	}

	int index = 0;
	for (int row = 1; row < vertex_row_number; row++)
	{
		for (int column = 1; column < vertex_column_number; column++)
		{
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column - 1); //bottom left
			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);     //bottom right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);       //top left

			mesh->setIndex(index++, (row - 1) * vertex_column_number + column);     //bottom right
			mesh->setIndex(index++, row * vertex_column_number + column);           //top right
			mesh->setIndex(index++, row * vertex_column_number + column - 1);       //top left
		}
	}

	mesh->createTangents();
	mesh->createBounds();
	mesh->optimizeIndices(Mesh::VERTEX_CACHE);
	return mesh;
}
}
#include "Spline.h"

using namespace Unigine;
using namespace Math;

Vec3 getBezierPoint(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, float k0)
{
	float k1 = 1.0f - k0;
	float k00 = k0 * k0;
	float k11 = k1 * k1;
	return p0 * (k11 * k1) + p1 * (3.0f * k11 * k0) + p2 * (3.0f * k00 * k1) + p3 * (k00 * k0);
}

void getAutoHandles(const Vec3 &prev, const Vec3 &current, const Vec3 &next, Vec3 &left, Vec3 &right)
{
	left = (prev - current) / 3.0f;
	right = (next - current) / 3.0f;
	left = (left - right) / 2.0f;
	right = -left;
}

void SplineNode::init(const NodePtr &container, const ObjectMeshStaticPtr &reference)
{
	for (int i = 0; i < container->getNumChildren(); i++)
	{
		nodes.append(container->getChild(i));
		nodes_pos.append();
		nodes_rot.append();
	}
}

void SplineNode::setSubdivLevel(int in_level)
{
	subdiv = in_level;
}

int SplineNode::getSubdivLevel() const
{
	return subdiv;
}

void SplineNode::setOffset(float in_offset)
{
	offset = in_offset;
}

float SplineNode::getOffset() const
{
	return offset;
}

void SplineNode::setSpacing(float in_spacing)
{
	spacing = in_spacing;
}

float SplineNode::getSpacing() const
{
	return spacing;
}

void SplineNode::update()
{
	mesh_transforms.clear();
	// get node positions and rotations
	for (int i = 0; i < nodes.size(); i++)
	{
		NodePtr& n = nodes[i];
		nodes_pos[i] = n->getPosition();
		nodes_rot[i] = n->getRotation();
	}

	// calculate spline
	int obj_num = 0;
	float current_spline_length = 0;
	float dist = spacing - Math::mod(offset, spacing);
	float real_spacing = total_obj_num == 0 ? spacing : spline_length / total_obj_num;
	
	for (int i = 0; i < nodes.size(); i++)
	{
		int prev_i = (i - 1 < 0 ? (nodes.size() - 1) : (i - 1));
		int cur_i = i;
		int next_i = (i + 1 < nodes.size() ? (i + 1) : 0);
		int next_next_i = ((i + 2) < nodes.size() ? (i + 2) : ((i + 2) % nodes.size()));

		const Vec3 &prev_point = nodes_pos[prev_i];
		const Vec3 &cur_point = nodes_pos[cur_i];
		const Vec3 &next_point = nodes_pos[next_i];
		const Vec3 &next_next_point = nodes_pos[next_next_i];

		Vec3 tmp, handle_right, handle_left;
		getAutoHandles(prev_point, cur_point, next_point, tmp, handle_right);
		getAutoHandles(cur_point, next_point, next_next_point, handle_left, tmp);

		Vec3 last_p = cur_point;
		for (int z = 0; z <= subdiv; z++)
		{
			float t = float(z + 1) / (subdiv + 1);
			Vec3 p = getBezierPoint(cur_point, cur_point + handle_right, handle_left + next_point, next_point, t);

			// update cluster
			float len = static_cast <float>(length(p - last_p));
			if (dist + len >= real_spacing) {
				int count = int(Math::floor((dist + len) / real_spacing));
				for (int j = 1; j <= count; j++) {
					float percent = (j * real_spacing - dist) / len;

					// add transform
					mesh_transforms.append(
						translate(lerp(last_p, p, percent)) *
						Mat4(rotate(slerp(nodes_rot[cur_i], nodes_rot[next_i], t + percent / (subdiv + 1)))));
					Mat4 &cur_t = mesh_transforms.last();
					cur_t = parent_world_transform * (cur_t * Mat4(rotateY(90.0f)));
					obj_num++;
				}
				dist = Math::mod((dist + len), real_spacing);
			}
			else
				dist += len;

			current_spline_length += len;
			last_p = p;
		}
	}
	spline_length = current_spline_length;
	total_obj_num = total_obj_num == 0 ? obj_num : total_obj_num;
}

float SplineNode::getLength() const
{
	return spline_length;
}
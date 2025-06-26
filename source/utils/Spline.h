#pragma once
#include <UnigineMathLib.h>
#include <UnigineNode.h>
#include <UnigineObjects.h>

Unigine::Math::Vec3 getBezierPoint(const Unigine::Math::Vec3 &p0, const Unigine::Math::Vec3 &p1, const Unigine::Math::Vec3 &p2, const Unigine::Math::Vec3 &p3, float k0);

class SplineNode
{
public:
	void init(const Unigine::NodePtr &container, const Unigine::ObjectMeshStaticPtr &reference);

	// spline quality
	void setSubdivLevel(int level);
	int getSubdivLevel() const;

	// object placement
	void setOffset(float offset);
	float getOffset() const;

	void setSpacing(float spacing);
	float getSpacing() const;

	// refresh spline / nodes
	void update();

	// info after update
	float getLength() const;

	void setParentTransform(const Unigine::Math::Mat4 &parent_transform) { parent_world_transform = parent_transform; }

	const Unigine::Vector<Unigine::Math::Mat4> &getMeshTrasform() const { return mesh_transforms; }

private:
	Unigine::Vector<Unigine::NodePtr> nodes; // spline nodes
	Unigine::Vector<Unigine::Math::Vec3> nodes_pos; // position of spline nodes
	Unigine::Vector<Unigine::Math::quat> nodes_rot; // rotation of spline nodes
	Unigine::Vector<Unigine::Math::Mat4> mesh_transforms;
	Unigine::Math::Mat4 parent_world_transform;
	
	int subdiv = 10;
	float offset = 0;
	float spacing = 1.0f;
	float spline_length = 0;
	int total_obj_num = 0;
};

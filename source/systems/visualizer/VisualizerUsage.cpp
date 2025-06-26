#include "VisualizerUsage.h"
#include <UnigineVisualizer.h>


using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(VisualizerUsage);

void VisualizerUsage::init()
{
	if (!node_boundBox_example || !node_boundSphere_example)
	{
		Log::error("VisualizerUsage::init() example nodes are not assigned: \n");
	}
	object_example = checked_ptr_cast<Object>(node_object_example.get());
	object_solid_example = checked_ptr_cast<Object>(node_object_solid_example.get());

	surface_example = checked_ptr_cast<Object>(node_surface_example.get());
	surface_solid_example = checked_ptr_cast<Object>(node_surface_solid_example.get());

	object_surface_boundBox_example = checked_ptr_cast<Object>(node_object_surface_boundBox_example.get());
	object_surface_boundSphere_example = checked_ptr_cast<Object>(node_object_surface_boundSphere_example.get());

	if (!object_example || !object_solid_example || !surface_example || !surface_solid_example
		|| !object_surface_boundBox_example || !object_surface_boundSphere_example)
	{
		Log::error("VisualizerUsage::init() example objects are not assigned: \n");
	}
	for (int i = 0; i < postament_nodes.size(); i++)
	{
		if (postament_nodes.get(i).get() == nullptr)
		{
			Log::error("VisualizerUsage::init() postament node with index: %i is not assigned\n", i);
		}
	}

	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void VisualizerUsage::update3D()
{
	int i = 0;
	//1-10
	Vec3 current_point = getPostamentPoint(i);
	Visualizer::renderLine3D(current_point, current_point + Vec3_up, current_point + Vec3_up + Vec3_right / 2, current_point + Vec3_up * 2, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderPoint3D(current_point, 0.5, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderTriangle3D(current_point, current_point + Vec3_down / 2 + Vec3_left / 2, current_point + Vec3_down / 2 + Vec3_right / 2, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderQuad3D(current_point + Vec3_left / 2, current_point + Vec3_left / 2 + Vec3_up, current_point + Vec3_up + Vec3_right / 2, current_point + Vec3_right / 2, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderBillboard3D(current_point, 0.5f, vec4_zero, true);// try use setTextureName
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderVector(current_point + Vec3_down / 2 + Vec3_left / 2, current_point + Vec3_up / 2 + Vec3_right / 2, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderDirection(current_point + Vec3_down / 2 + Vec3_left / 2, vec3(1, 0, 1), vec4_red, 0.25f, false);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderBox(vec3_one, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidBox(vec3_one, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	mat4 proj = perspective(60, 16 / 9, 0.1f, 1);
	Unigine::Math::Mat4 modelview = Unigine::Math::lookAt(current_point + Vec3_left / 2, current_point, Unigine::Math::vec3_up);
	//transformation matrix equals inversed modelview
	Visualizer::renderFrustum(proj, Math::inverse(modelview), vec4_red);
	i++;


	//10-20
	current_point = getPostamentPoint(i);
	Visualizer::renderCircle(0.5, Mat4(quat_identity * quat(90, 0, 0), current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSector(0.5, 60, Mat4(quat_identity * quat(90, 0, 0), current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderCone(0.5, 30, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSphere(0.5, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidSphere(0.5, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderCapsule(0.5, 0.5, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidCapsule(0.5, 0.5, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderCylinder(1, 1, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidCylinder(1, 1, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	vec3 center = vec3(0, 0, 0);// check out documentation for center format information
	Visualizer::renderMessage3D(current_point, center, "renderMessage3D exapmle", vec4_red);
	i++;

	//20-30
	current_point = getPostamentPoint(i);
	Visualizer::renderEllipse(vec3(0.5, 1, 1.5), Mat4((quat_identity), current_point + Vec3_up), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidEllipse(vec3(0.5, 1, 1.5), Mat4((quat_identity), current_point + Vec3_up), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	BoundBox bb = BoundBox(vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f));
	Visualizer::renderBoundBox(bb, Mat4(quat_identity * quat(90, 0, 0), current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	BoundSphere bs = BoundSphere(vec3_zero, 0.5f);
	Visualizer::renderBoundSphere(bs, Mat4(quat_identity, current_point), vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderNodeBoundBox(node_boundBox_example, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderNodeBoundSphere(node_boundSphere_example, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderObject(object_example, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidObject(object_solid_example, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderObjectSurface(surface_example, 0, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderSolidObjectSurface(surface_solid_example, 0, vec4_red);
	i++;


	//30-32
	current_point = getPostamentPoint(i);
	Visualizer::renderObjectSurfaceBoundBox(object_surface_boundBox_example, 0, vec4_red);
	i++;

	current_point = getPostamentPoint(i);
	Visualizer::renderObjectSurfaceBoundSphere(object_surface_boundSphere_example, 0, vec4_red);
	i++;
}
void VisualizerUsage::update2D()
{
	if (renderPoint2D) Visualizer::renderPoint2D(vec2(0.1f, 0.5), 0.01, vec4_red);
	if (renderLine2D) Visualizer::renderLine2D(vec2(0.1, 0.55), vec2(0.15, 0.55), vec2(0.15, 0.60), vec2(0.20, 0.60), vec4_red);
	if (renderTriangle2D) Visualizer::renderTriangle2D(vec2(0.2, 0.65), vec2(0.1, 0.62), vec2(0.1, 0.68), vec4_red);
	if (renderQuad2D) Visualizer::renderQuad2D(vec2(0.1, 0.8), vec2(0.08, 0.75), vec2(0.1, 0.70), vec2(0.12, 0.75), vec4_red);
	if (renderRectangle) Visualizer::renderRectangle(vec4(0.1f,0.1f , 0.15f, 0.15f), vec4_red);
	if (renderMessage2D) Visualizer::renderMessage2D(vec3(0.1, 0.95, 0), vec3(0, 0, 0), "renderMessage2D example", vec4_red);
}
void VisualizerUsage::update()
{
	update3D();
	update2D();
}

Unigine::Math::Vec3 VisualizerUsage::getPostamentPoint(int index)
{
	Vec3 result = Vec3_one;
	if (index < postament_nodes.size())
	{
		NodePtr node = postament_nodes.get(index).get();
		if (node)
		{
			result = node->getWorldPosition();
		}
		else {
			Log::message("VisualizerUsage::getNextPostamentPoint postament_nodes: Node with index:%i is nullptr\n", index);
		}
	}
	else {
		Log::message("Visualizer usage doesn't have enough pedestal's display nodes to draw all visualizer examples\n");
	}
	return result;
}
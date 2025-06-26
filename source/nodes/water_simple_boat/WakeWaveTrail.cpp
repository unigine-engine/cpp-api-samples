#include "WakeWaveTrail.h"

#include <UnigineVisualizer.h>
#include <UnigineGame.h>
#include <UnigineProfiler.h>
#include <UnigineEditor.h>


REGISTER_COMPONENT(WakeWaveTrail);

void WakeWaveTrail::clearButton()
{
	path_points.clear();
	clear_button = false;
	need_remove_last = false;
}

void WakeWaveTrail::saveMesh()
{
	if (mesh)
	{
		String name = String::format("mesh_%f.mesh", Game::getTime());
		mesh->save(name);
		Log::warning("saved to %s\n", name.get());
	}
	save_mesh = false;
}

void WakeWaveTrail::init()
{
	if (!mesh_decal_node.isEmpty())
	{
		decal = checked_ptr_cast<DecalMesh>(mesh_decal_node.get());
		if (!decal)
		{
			Log::error("WakeWaveTrail should be on decal mesh!!!\n");
			return;
		}
		itransform = decal->getIWorldTransform();
	}
	if (!mesh)
		mesh = Mesh::create();
}

void WakeWaveTrail::update()
{
	if (clear_button)
		clearButton();
	if (save_mesh)
		saveMesh();

	init();
	if (!decal)
		return;

	addPoint(decal->getIWorldTransform() * node->getWorldPosition());
	hide();
	build_mesh();

	draw_debug();

	if (path_points.size() > 1)
	{
		const auto &first = path_points.first();
		const auto &last = path_points.last();

		start_distance = first.distance;
		start_time = first.time;

		end_distance = last.distance;
		end_time = last.time;

		num_quad = path_points.size();

		auto mat = Editor::isLoaded() ? decal->getMaterial() : decal->getMaterialInherit();
		if (mat)
		{
			mat->setParameterInt(num_quad_material_param_name.get(), num_quad);
			mat->setParameterFloat(start_time_material_param_name.get(), start_time);
			mat->setParameterFloat(end_time_material_param_name.get(), end_time);
			mat->setParameterFloat(start_distance_material_param_name.get(), start_distance);
			mat->setParameterFloat(end_distance_material_param_name.get(), end_distance);
			mat->setParameterFloat(speed_material_param_name.get(), speed);
			mat->setParameterFloat(max_distance_material_param_name.get(), max_distance_hide);
			mat->setParameterFloat(width_material_param_name.get(), width);
		}
	}
}

void WakeWaveTrail::shutdown()
{
}

void WakeWaveTrail::addPoint(const Vec3 &point)
{
	if (prev_pos == point)
		return;

	float distance_between_frame = toFloat((prev_pos - point).length());
	speed = distance_between_frame / Game::getIFps();

	if (distance_between_frame < distance_eps)
		return;

	if (path_points.size() == 0)
	{
		add_point_internal(point);
		prev_frame_dir = vec3_zero;
		return;
	} else
	{
		if (need_remove_last)
		{
			path_points.removeLast();
			need_remove_last = false;
		}
		add_point_internal(point);
		calculate_last_tangents();

		vec3 frame_dir = vec3(normalize(point - prev_pos));
		if (!compare(prev_frame_dir, frame_dir, direction_eps))
		{
			prev_frame_dir = frame_dir;
		} else
		{
			need_remove_last = true;
		}
	}
	prev_pos = point;
}

void WakeWaveTrail::calculate_last_tangents()
{
	auto &p0 = path_points[path_points.size() - 1];
	auto &p1 = path_points[path_points.size() - 2];

	Vec3 distance = p0.point - p1.point;
	p0.distance = p1.distance + toFloat(distance.length());

	vec3 direction = vec3(normalize(distance));
	p1.forward = direction;
	p0.forward = direction; // it will be override next step

	if (path_points.size() > 2)
	{
		const auto &p2 = path_points[path_points.size() - 3]; // predposlednyaya
		vec3 right_1 = normalize(cross(p2.forward, vec3_up));
		vec3 right_2 = normalize(cross(p1.forward, vec3_up));
		p1.right = normalize((right_1 + right_2) * 0.5);
		p0.right = p1.right; // it will be override next step
	}
}

void WakeWaveTrail::draw_debug()
{
	if (debug)
	{
		for (int i = 1; i < path_points.size(); i++)
		{
			const auto &p0 = path_points[i - 1];
			const auto &p1 = path_points[i];
			Visualizer::renderLine3D(p0.point + Vec3(p0.right * width), p1.point + Vec3(p1.right * width), vec4_green);
			Visualizer::renderLine3D(p0.point - Vec3(p0.right * width), p1.point - Vec3(p1.right * width), vec4_red);
			Visualizer::renderLine3D(p1.point + Vec3(p1.right * width), p1.point - Vec3(p1.right * width), vec4_blue);
			Visualizer::renderLine3D(p0.point, p1.point, vec4_white);
			Visualizer::renderMessage3D(p0.point, vec3_up, String::ftoa(p0.time), vec4_white);
		}
		Log::message("before %d after %d\n", path_points.size(), path_points.size());
	}
}

void WakeWaveTrail::build_mesh()
{
	if (path_points.size() < 1)
		return;

	mesh->clear();
	mesh->addSurface("");

	auto add_point_func = [this](const vec3 &pos, const vec3 &forward, const vec3 &right, float w, int number, float time, float distance, float speed) {
		mesh->addVertex(pos - right * w, 0);
		mesh->addVertex(pos, 0);
		mesh->addVertex(pos + right * w, 0);
		mesh->addTexCoord0(vec2(1, distance), 0);
		mesh->addTexCoord0(vec2(0.5, distance), 0);
		mesh->addTexCoord0(vec2(0, distance), 0);
		mesh->addTexCoord1(vec2(1, time), 0);
		mesh->addTexCoord1(vec2(0.5, time), 0);
		mesh->addTexCoord1(vec2(0, time), 0);
		mesh->addNormal(vec3_up, 0);
		mesh->addNormal(vec3_up, 0);
		mesh->addNormal(vec3_up, 0);
		mesh->addTangent(quat(-right, -forward, vec3_up), 0);
		mesh->addTangent(quat(right, forward, vec3_up), 0);
		mesh->addTangent(quat(right, forward, vec3_up), 0);
		//		mesh->addColor(vec4(time, number, speed, 0), 0);
		//		mesh->addColor(vec4(time, number, speed, 0), 0);
	};

	auto link_indices = [this](int m_num_quad) {
		//		m_num_quad *= 2;
		//		mesh->addCIndex(0 + m_num_quad, 0);
		//		mesh->addCIndex(1 + m_num_quad, 0);
		//		mesh->addCIndex(2 + m_num_quad, 0);
		//		mesh->addCIndex(1 + m_num_quad, 0);
		//		mesh->addCIndex(3 + m_num_quad, 0);
		//		mesh->addCIndex(2 + m_num_quad, 0);

		//		mesh->addTIndex(0 + m_num_quad, 0);
		//		mesh->addTIndex(1 + m_num_quad, 0);
		//		mesh->addTIndex(2 + m_num_quad, 0);
		//		mesh->addTIndex(1 + m_num_quad, 0);
		//		mesh->addTIndex(3 + m_num_quad, 0);
		//		mesh->addTIndex(2 + m_num_quad, 0);
		m_num_quad *= 3;

		mesh->addCIndex(0 + m_num_quad, 0);
		mesh->addCIndex(1 + m_num_quad, 0);
		mesh->addCIndex(3 + m_num_quad, 0);

		mesh->addCIndex(1 + m_num_quad, 0);
		mesh->addCIndex(4 + m_num_quad, 0);
		mesh->addCIndex(3 + m_num_quad, 0);

		mesh->addCIndex(1 + m_num_quad, 0);
		mesh->addCIndex(2 + m_num_quad, 0);
		mesh->addCIndex(4 + m_num_quad, 0);

		mesh->addCIndex(2 + m_num_quad, 0);
		mesh->addCIndex(5 + m_num_quad, 0);
		mesh->addCIndex(4 + m_num_quad, 0);

		mesh->addTIndex(0 + m_num_quad, 0);
		mesh->addTIndex(1 + m_num_quad, 0);
		mesh->addTIndex(3 + m_num_quad, 0);
		mesh->addTIndex(1 + m_num_quad, 0);
		mesh->addTIndex(4 + m_num_quad, 0);
		mesh->addTIndex(3 + m_num_quad, 0);
		mesh->addTIndex(1 + m_num_quad, 0);
		mesh->addTIndex(2 + m_num_quad, 0);
		mesh->addTIndex(4 + m_num_quad, 0);
		mesh->addTIndex(2 + m_num_quad, 0);
		mesh->addTIndex(5 + m_num_quad, 0);
		mesh->addTIndex(4 + m_num_quad, 0);
	};

	BoundBox bb;
	for (int i = 0; i < path_points.size(); i++)
	{
		const auto &p = path_points[i];
		vec3 point, r, f;
		mul(point, itransform, p.point);
		mul3(r, itransform, p.right);
		mul3(f, itransform, p.forward);
		bb.expand(BoundSphere(point, bound_increase));
		add_point_func(point, f, r, width, i, p.time, p.distance, p.speed);
		if (i == 0)
			continue;
		link_indices(i - 1);
	}

	mesh->setBoundBox(bb);

	if (decal && path_points.size() > 2)
		decal->applyCopyMeshProceduralForce(mesh);
}

void WakeWaveTrail::hide()
{
	if (path_points.size() < 2)
		return;

	float d = path_points.last().distance - path_points[1].distance;
	if (d > max_distance_hide)
	{
		path_points.removeFirst();
	}
}

void WakeWaveTrail::add_point_internal(const Vec3 &point)
{
	auto &p = path_points.append();
	p.point = point;
	p.time = Game::getTime();
	p.speed = speed;
}

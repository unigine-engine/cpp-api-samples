#include "SensorZoneVisualizer.h"

REGISTER_COMPONENT(SensorZoneVisualizer)

using namespace Unigine;
using namespace Math;

void SensorZoneVisualizer::refresh()
{
	init_params();
}

const vec4 &SensorZoneVisualizer::getColor() const
{
	return current_color;
}

void SensorZoneVisualizer::setColor(const vec4 &color)
{
	if (material_param_index < 0)
		return;
	current_color = color;
	material->setParameterFloat4(material_param_index, color);
}

void SensorZoneVisualizer::init()
{
	init_params();
	getProperty()->getEventParameterChanged().connect(this, &SensorZoneVisualizer::init_params);

	mesh = Mesh::create();
	obj = ObjectMeshStatic::create();
	obj->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_FILE);
	node->addChild(obj);

	if (!mat.nullCheck())
	{
		material = mat.get()->inherit();
		material_param_index = material->findParameter(color_parameter.get());
		if (material_param_index > -1)
		{
			current_color = material->getParameterFloat4(material_param_index);
			// keep the color logic in one place (setColor); only override the
			// material's own color when the sample asked for it
			if (change_color.get())
				setColor(mat_color.get());
		}
	}

	// Build the initial mesh and assign the material once here: the single
	// surface is reused on every rebuild, so update() only refreshes geometry.
	update_mesh(mesh);
	obj->applyMoveMeshProceduralForce(mesh);
	if (material)
		obj->setMaterial(material, "*");
	changed = false;
}

void SensorZoneVisualizer::update()
{
	if (changed)
	{
		update_mesh(mesh);
		obj->applyMoveMeshProceduralForce(mesh);
		changed = false;
	}
}

void SensorZoneVisualizer::shutdown()
{
	obj.deleteLater();
	mesh.clear();
}

void SensorZoneVisualizer::on_enable()
{
	if (obj)
		obj->setEnabled(true);
}

void SensorZoneVisualizer::on_disable()
{
	if (obj)
		obj->setEnabled(false);
}

void SensorZoneVisualizer::init_params()
{
	Vec2 v_angles(clamp(vertical_angles.get(), vec2(-180.f, -180.f), vec2(180.f, 180.f)));
	Vec2 h_angles(clamp(horizontal_angles.get(), vec2(-180.f, -180.f), vec2(180.f, 180.f)));

	min_horizontal_angle = h_angles.x;
	max_horizontal_angle = h_angles.y;

	min_vertical_angle = v_angles.x;
	max_vertical_angle = v_angles.y;

	slices = num_slices < 2 ? 2 : num_slices;
	stacks = num_stacks < 2 ? 2 : num_stacks;
	deviders = num_side_segments < 1 ? 1 : num_side_segments;

	near_dist = near > 0.1f ? near : 0.1f;
	far_dist = far > 0.2f ? far : 0.2f;
	changed = true;
}

void SensorZoneVisualizer::update_mesh(Unigine::MeshPtr mesh)
{
	if (mesh->getNumSurfaces() != 1)
	{
		mesh->clear();
		mesh->addSurface("");
	}
	else
	{
		mesh->clearSurface();
	}

	Unigine::Vector<vec3>& vertices = mesh->getVertices();
	Unigine::Vector<int>& cindices = mesh->getCIndices();
	Unigine::Vector<int>& tindices = mesh->getTIndices();
	Unigine::Vector<quat>& tangents = mesh->getTangents();
	Unigine::Vector<vec3>& normals = mesh->getNormals();

	auto appendIndex = [&cindices, &tindices](Vector<int> index) {
		cindices.append(index);
		tindices.append(index);
		};

	SectorSurface sphere, left, right, top, bottom, top_sector, bottom_sector;

	float max_horizontal = max_horizontal_angle * Consts::DEG2RAD;
	float min_horizontal = min_horizontal_angle * Consts::DEG2RAD;
	float horizontal_fov = max_horizontal - min_horizontal;

	float min_vertical = min_vertical_angle * Consts::DEG2RAD;
	float max_vertical = max_vertical_angle * Consts::DEG2RAD;

	int horizontal_segments = slices * horizontal_fov / Consts::PI2;


	float extra_max_v = 0.f;
	float extra_min_v = 0.f;
	bool inverse_top = false;
	bool inverse_bottom = false;
	int extra_h_seg = -1;

	if (min_vertical < -Consts::PI05)
	{
		inverse_bottom = true;
		extra_min_v = (min_vertical + Consts::PI05);
		min_vertical = -Consts::PI05;
		extra_h_seg = ceil(horizontal_segments * (Consts::PI2 - horizontal_fov) / horizontal_fov);
	}

	if (max_vertical > Consts::PI05)
	{
		inverse_top = true;
		extra_max_v = (max_vertical - Consts::PI05);
		max_vertical = Consts::PI05;
		extra_h_seg = ceil(horizontal_segments * (Consts::PI2 - horizontal_fov) / horizontal_fov);
	}

	float v_fov = max_vertical - min_vertical;

	if (v_fov <= Consts::EPS && horizontal_fov <= Consts::EPS)
		return;

	int v_segs = stacks * v_fov / Consts::PI2;

	int cols = horizontal_segments + 1;
	int rows = v_segs + 1;

	int cols_extra = extra_h_seg + 1;

	int rows_bottom = 0;
	int rows_top = 0;
	int top_row = rows;
	int bottom_row = 0;

	if (inverse_top || inverse_bottom)
	{
		float bottom_border = min_vertical - extra_min_v;
		float top_border = max_vertical - extra_max_v;

		float unit = v_fov / v_segs;

		for (int j = 0; j < rows; j++)
		{
			float v = Math::lerp(min_vertical, max_vertical, (float)j / v_segs);
			if (v <= bottom_border)
				rows_bottom++;

			if (v >= top_border)
				rows_top++;
		}

		top_row = rows - rows_top;
		bottom_row = rows_bottom - 1;
		if (bottom_row > 0 && (bottom_border - bottom_row * unit < unit) && bottom_row < top_row)
			bottom_row++;
	}

	for (int j = 0; j < rows; j++)
	{
		float v = Math::lerp(min_vertical, max_vertical, (float)j / v_segs);
		float cos_v = Math::cos(v);
		float cos_sign = Math::sign(v);
		bool is_pole = cos_v < Consts::EPS;

		for (int i = 0; i < cols; i++)
		{
			float h = Math::lerp(min_horizontal, max_horizontal, (float)i / horizontal_segments);

			vec3 dir(Math::sin(h) * cos_v, Math::cos(h) * cos_v, Math::sin(v));
			dir.normalize();

			vec3 xy_dir(cos_sign * Math::sin(h), cos_sign * Math::cos(h), 0.f);
			xy_dir.normalize();

			// sphere data
			add_sphere_data(sphere, dir, is_pole);

			// left side
			if (i == 0 && j >= bottom_row && j <= top_row)
				add_hsides_vertex_data(left, dir, !is_pole ? vec3_down : xy_dir);

			// right side
			if (i == cols - 1 && j >= bottom_row && j <= top_row)
				add_hsides_vertex_data(right, dir, !is_pole ? vec3_up : -xy_dir);

			// bottom side
			if (!inverse_bottom && j == 0)
				add_vsides_vertex_data(bottom, dir, vec3_down);

			// top side
			if (!inverse_top && j == rows - 1)
				add_vsides_vertex_data(top, dir, vec3_up);
		}

		if ((!inverse_bottom && !inverse_top) || (j > bottom_row && j < top_row))
			continue;

		// create additional sides for v_fov > 180
		for (int i = 0; i < cols_extra; i++)
		{
			float h = Math::lerp(max_horizontal, Consts::PI2 + min_horizontal, (float)i / extra_h_seg);

			vec3 dir(Math::sin(h) * cos_v, Math::cos(h) * cos_v, Math::sin(v));
			dir.normalize();

			vec3 xy_dir(cos_sign * Math::sin(h), cos_sign * Math::cos(h), 0.f);
			xy_dir.normalize();

			if (inverse_bottom)
			{
				// bottom additional sphere
				if (j <= bottom_row)
					add_sphere_data(bottom_sector, dir, is_pole);

				// bottom side
				if (j == bottom_row)
					add_vsides_vertex_data(bottom, dir, vec3_up);
			}

			if (inverse_top)
			{
				// top additional sphere
				if (j >= top_row)
					add_sphere_data(top_sector, dir, is_pole);

				// top side
				if (j == top_row)
					add_vsides_vertex_data(top, dir, vec3_down);
			}
		}
	}

	auto append_surface = [&vertices, &normals, &tangents](SectorSurface& s) {
		s.offset = vertices.size();
		vertices.append(s.vertices);
		normals.append(s.normals);
		tangents.append(s.tangents);
		};


	append_surface(sphere);
	bool is_open_ring = horizontal_fov < Consts::PI2;
	if (is_open_ring)
	{
		append_surface(left);
		append_surface(right);
	}
	append_surface(bottom);
	append_surface(top);

	append_surface(bottom_sector);
	append_surface(top_sector);

	auto add_side_indexes = [&appendIndex](int col, int num_strip_segments, int offset, bool left) {
		int base = col * (num_strip_segments + 1) + offset;
		int base_u = base + (num_strip_segments + 1);

		for (int k = 0; k < num_strip_segments; k++)
		{
			if (left)
				appendIndex(Vector<int>{base, base_u + 1, base + 1, base, base_u, base_u + 1});
			else
				appendIndex(Vector<int>{base, base + 1, base_u + 1, base, base_u + 1, base_u});
			base++;
			base_u++;
		}
		};

	auto add_sphere_indexes = [&appendIndex](int num_cols, int col, int row, int offset) {
		int base = (row * num_cols + col) * 2 + offset;
		int base_r = base + 2;
		int base_u = base + num_cols * 2;
		int base_ru = base_u + 2;

		appendIndex(Vector<int>{base, base_r, base_ru, base, base_ru, base_u});
		appendIndex(
			Vector<int>{base + 1, base_u + 1, base_ru + 1, base + 1, base_ru + 1, base_r + 1});
		};

	// indexes for main sector surfaces
	for (int j = 0; j < v_segs; j++)
	{
		for (int i = 0; i < horizontal_segments; i++)
		{
			// sphere
			add_sphere_indexes(cols, i, j, sphere.offset);

			// top + bottom sides
			if (j == 0)
			{
				if (!inverse_top)
					add_side_indexes(i, deviders, top.offset, true);
				if (!inverse_bottom)
					add_side_indexes(i, deviders, bottom.offset, false);
			}
		}

		// left + right sides
		if (is_open_ring && j >= bottom_row && j < top_row)
		{
			add_side_indexes(j - bottom_row, 1, left.offset, true);
			add_side_indexes(j - bottom_row, 1, right.offset, false);
		}
	}

	// indexes for additional surfaces for v_fov > 180
	for (int i = 0; i < extra_h_seg; i++)
	{
		if (inverse_bottom)
		{
			// bottom side
			add_side_indexes(i, deviders, bottom.offset, true);
			// bottom additional sphere
			for (int j = 0; j < bottom_row; j++)
				add_sphere_indexes(cols_extra, i, j, bottom_sector.offset);
		}
		if (inverse_top)
		{
			// top side
			add_side_indexes(i, deviders, top.offset, false);
			// top additional sphere
			for (int j = 0; j < rows_top - 1; j++)
				add_sphere_indexes(cols_extra, i, j, top_sector.offset);
		}
	}

	mesh->createBounds();
}

void SensorZoneVisualizer::add_sphere_data(SectorSurface& s, vec3 dir, bool is_pole)
{
	s.vertices.push_back(dir * near_dist);
	s.vertices.push_back(dir * far_dist);

	create_sphere_vertex_data(s, -dir, vec3_down);
	create_sphere_vertex_data(s, dir, !is_pole ? vec3_up : vec3_right);
}

void SensorZoneVisualizer::create_sphere_vertex_data(SectorSurface& s, vec3 normal, vec3 up)
{
	vec3 tangent = cross(normal, up);
	tangent.normalize();
	quat q;
	q.set(tangent, cross(normal, tangent), normal);
	s.normals.push_back(normal);
	s.tangents.push_back(q);
}

void SensorZoneVisualizer::add_hsides_vertex_data(SectorSurface& s, vec3 dir, vec3 up)
{
	vec3 normal = cross(dir, up);
	normal.normalize();
	vec3 tangent = cross(dir, normal);
	tangent.normalize();
	quat q;
	q.set(tangent, dir, normal);

	s.vertices.push_back(dir * near_dist);
	s.vertices.push_back(dir * far_dist);
	s.normals.push_back(normal);
	s.normals.push_back(normal);
	s.tangents.push_back(q);
	s.tangents.push_back(q);
}

void SensorZoneVisualizer::add_vsides_vertex_data(SectorSurface& s, vec3 dir, vec3 up)
{
	vec3 tangent = cross(dir, up);
	tangent.normalize();
	vec3 normal = cross(tangent, dir);
	normal.normalize();
	quat q;
	q.set(tangent, dir, normal);

	for (int k = 0; k < deviders + 1; k++)
	{
		s.vertices.push_back(dir * (near_dist + (far_dist - near_dist) * k / deviders));
		s.normals.push_back(normal);
		s.tangents.push_back(q);
	}
}

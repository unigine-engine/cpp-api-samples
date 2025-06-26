#include "ObjectFrame.h"

#include <UnigineGame.h>
#include <UnigineViewport.h>

REGISTER_COMPONENT(ObjectFrame)


using namespace Unigine;
using namespace Math;

Unigine::WidgetCanvasPtr ObjectFrame::canvas;
bool ObjectFrame::canvas_clean;
int ObjectFrame::components_count = 0;

void ObjectFrame::init()
{
	components_count++;
	auto init_transform = node->getWorldTransform();
	node->setWorldTransform(Mat4(Math::scale(node->getScale())));
	collect_mesh_bound_box(node);

	points.resize(8);
	bound_box.getPoints(points.get(), 8);

	local_center = bound_box.getCenter();
	node->setWorldTransform(init_transform);

	if (!canvas)
	{
		auto gui = Gui::getCurrent();
		canvas = WidgetCanvas::create(gui);
		gui->addChild(canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_EXPAND);
		canvas_clean = false;
	}

	max_visibility_distance = visibility_distance * visibility_distance;
}

void ObjectFrame::update()
{
	auto window = WindowManager::getMainWindow();
	auto player = Game::getPlayer();
	if (!window || !player)
		return;

	is_frame_rendered = false;
	if (!canvas_clean)
	{
		canvas_clean = true;
		canvas->clear();
	}
	float distance = (node->getWorldPosition() - player->getWorldPosition()).length2();
	if (distance > max_visibility_distance)
		return;

	auto screen_size = window->getClientSize();
	min_point = {screen_size.x, screen_size.y};
	max_point = {0, 0};

	int x, y;
	bool visible = false;
	auto transform = node->getWorldTransform();
	for (const auto &point : points)
	{
		visible |= get_screen_position(x, y, Vec3(transform * point), player, screen_size);
		min_point.x = Math::min(min_point.x, x);
		min_point.y = Math::min(min_point.y, y);
		max_point.x = Math::max(max_point.x, x);
		max_point.y = Math::max(max_point.y, y);
	}

	if (!visible || !is_rectangle_on_screen(screen_size, min_point, max_point))
		return;

	auto world_center = transform * local_center;
	auto obj = World::getIntersection(player->getWorldPosition(), Vec3(world_center), ~0);
	if (!obj || !is_child(obj))
		return;

	int order = int(-distance * 100);
	ivec2 minmax_point = ivec2{min_point.x, max_point.y};
	ivec2 maxmin_point = ivec2{max_point.x, min_point.y};

	draw_rectangle(order, minmax_point, min_point - ivec2{border_size});
	draw_rectangle(order, min_point, maxmin_point + ivec2{1, -1} * border_size);
	draw_rectangle(order, maxmin_point, max_point + ivec2{border_size});
	draw_rectangle(order, max_point, minmax_point + ivec2{-1, 1} * border_size);

	int text_id = canvas->addText(order + 1);
	canvas->setFontSize(label_text_size);
	ivec2 text_size = canvas->toUnitSize(canvas->getTextRenderSize(label_text));

	auto header_point_min = min_point - ivec2{border_size};
	auto header_point_max = header_point_min + ivec2{text_size.x, -text_size.y}
		+ ivec2{2, -1} * border_size;
	draw_rectangle(order, header_point_min, header_point_max);

	canvas->setTextPosition(text_id, vec2(min_point.x, header_point_max.y));
	canvas->setTextText(text_id, label_text);
	canvas->setTextSize(text_id, label_text_size);
	canvas->setTextAlign(text_id, Gui::ALIGN_LEFT | Gui::ALIGN_BOTTOM | Gui::ALIGN_BACKGROUND);
	canvas->setTextColor(text_id, label_text_color);
	canvas->setTextOutline(text_id, label_text_outline);

	is_frame_rendered = true;
}

void ObjectFrame::post_update()
{
	if (canvas_clean)
		canvas_clean = false;
}

void ObjectFrame::shutdown()
{
	components_count--;
	if (components_count == 0 && (canvas || !canvas.isDeleted()))
	{
		canvas.deleteLater();
		canvas = nullptr;
	}
}

void ObjectFrame::collect_mesh_bound_box(const NodePtr &n)
{
	if (!n)
		return;

	int type = n->getType();
	if (type == Node::NODE_REFERENCE)
		collect_mesh_bound_box(static_ptr_cast<NodeReference>(n)->getReference());
	else if (type == Node::OBJECT_MESH_STATIC || type == Node::OBJECT_MESH_SKINNED
		|| type == Node::OBJECT_MESH_DYNAMIC)
	{
		bound_box.expand(n->getBoundBox());
	}

	for (int i = 0; i < n->getNumChildren(); i++)
		collect_mesh_bound_box(n->getChild(i));
}

bool ObjectFrame::get_screen_position(int &x, int &y, const Vec3 &world_point,
	const Unigine::PlayerPtr &player, const Unigine::Math::ivec2 &screen_size)
{
	float width = itof(screen_size.x);
	float height = itof(screen_size.y);

	mat4 projection = player->getProjection();
	mat4 modelview = mat4(player->getCamera()->getModelview());
	projection.m00 *= height / width;

	Vec4 p = projection * Vec4(modelview * Vec4(world_point, 1));
	if (p.w > 0)
	{
		// in front of camera
		x = (static_cast<float>(width * (0.5f + p.x * 0.5f / p.w)));
		y = (static_cast<float>(height - height * (0.5f + p.y * 0.5f / p.w)));
		return true;
	}
	else
	{
		// behind camera
		x = (width - static_cast<float>(width * (0.5f + p.x * 0.5f / p.w)));
		y = (height - static_cast<float>(height - height * (0.5f + p.y * 0.5f / p.w)));
		return false;
	}
}

int ObjectFrame::draw_rectangle(int order, const ivec2 &p1, const ivec2 &p2)
{
	int id = canvas->addPolygon(order);

	canvas->addPolygonPoint(id, vec3(p1.x, p1.y, 0));
	canvas->addPolygonPoint(id, vec3(p1.x, p2.y, 0));
	canvas->addPolygonPoint(id, vec3(p2.x, p2.y, 0));
	canvas->addPolygonPoint(id, vec3(p2.x, p1.y, 0));
	canvas->addPolygonIndex(id, 0);
	canvas->addPolygonIndex(id, 1);
	canvas->addPolygonIndex(id, 2);
	canvas->addPolygonIndex(id, 2);
	canvas->addPolygonIndex(id, 3);
	canvas->addPolygonIndex(id, 0);

	canvas->setPolygonColor(id, border_color);
	return id;
}

bool ObjectFrame::is_rectangle_on_screen(const Unigine::Math::ivec2 &screen_size, const ivec2 &min,
	const ivec2 &max)
{
	if ((min.x <= 0 && max.x <= 0) || (min.x >= screen_size.x && max.x >= screen_size.x)
		|| (min.y <= 0 && max.y <= 0) || (min.y >= screen_size.y && max.y >= screen_size.y))
		return false;
	return true;
}

bool ObjectFrame::is_child(const Unigine::NodePtr &n)
{
	if (n == node)
		return true;
	auto parent = n->getParent();
	if (!parent)
		parent = n->getPossessor();
	return parent ? is_child(parent) : false;
}

void ObjectFrame::setObjectFramesEnabled(bool enabled)
{
	if (!canvas)
		return;

	auto gui = Gui::getCurrent();
	if (enabled)
		gui->addChild(canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_EXPAND | Gui::ALIGN_BACKGROUND);
	else
		gui->removeChild(canvas);
}

JsonPtr ObjectFrame::getJsonMeta() const
{
	JsonPtr json = Json::create();
	json->setObject();
	json->addChild("node_id", node->getID());
	json->addChild("node_name", node->getName());

	auto pos = node->getWorldPosition();
	auto pos_json = json->addChild("position");
	pos_json->setObject();
	pos_json->addChild("x", pos.x);
	pos_json->addChild("y", pos.y);
	pos_json->addChild("z", pos.z);

	auto rot = node->getWorldRotation();
	auto rot_json = json->addChild("rotation");
	rot_json->setObject();
	rot_json->addChild("x", rot.getAngle(vec3_right));
	rot_json->addChild("y", rot.getAngle(vec3_forward));
	rot_json->addChild("z", rot.getAngle(vec3_up));

	auto min_frame_json = json->addChild("screen_position");
	min_frame_json->setObject();
	min_frame_json->addChild("min_x", min_point.x);
	min_frame_json->addChild("min_y", min_point.y);
	min_frame_json->addChild("max_x", max_point.x);
	min_frame_json->addChild("max_y", max_point.y);

	return json;
}

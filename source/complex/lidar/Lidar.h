#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>

class Lidar final : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Lidar, ComponentBase)

	PROP_PARAM(Material, lidar_post_material)

	PROP_PARAM(Int, yaw_rays, 264)
	PROP_PARAM(Int, pitch_rays, 64)

	PROP_PARAM(Vec2, yaw_angle_range, { -180.f, 180.f })
	PROP_PARAM(Vec2, pitch_angle_range, { -40.f, 30.f })

	PROP_PARAM(Vec2, distance_range, { 0.5f, 128.f })
	PROP_PARAM(IVec2, resolution, { 640, 640 })

	PROP_PARAM(Int, num_views, 4)

	PROP_PARAM(Toggle, is_debug, true)
	PROP_PARAM(Toggle, is_debug_render_textures, false)
	PROP_PARAM(Toggle, is_debug_render_points, true)
	PROP_PARAM(Toggle, is_debug_render_frustums, false)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	friend class LidarSample;

	struct View
	{
		// pixel location: view(image) id + offset
		struct Location
		{
			constexpr static uint32_t invalid_view_idx = ~0;

			operator bool() const { return view_idx != invalid_view_idx; }
			bool operator <(Location const& r) const { return (view_idx == r.view_idx) ? pixel_offset < r.pixel_offset : view_idx < r.view_idx; }

			uint32_t view_idx = invalid_view_idx;
			uint32_t pixel_offset = 0; // pixel offset in the "image" with depth and intensity data
		};

		Unigine::Math::mat4 transform; // local transform of the view / camera
		Unigine::Math::vec4 plane; // camera frustum's near plane

		Unigine::TexturePtr texture; // texture (vram) to which depth and intensity data is written
		Unigine::ImagePtr image; // image (ram) that is used in the main thread for updating rays

		Unigine::WidgetSpritePtr debug_sprite;

		enum State
		{
			IDLE, // texture is ready to be rendered to
			TRANSFER, // texture has been rendered and is currently being transferred from gpu to cpu
			UPDATED, // texture has been transferred to cpu (image) and is ready to be used
			UNUSED, // no rays intersect with this view plane, so it's unused
		};

		int state = UNUSED;
	};

	struct Ray
	{
		Unigine::Math::vec3 direction; // local direction of the ray
		View::Location location; // pixel location of the ray on the view plane it intersects
		float distance = 0.f;
		float intensity = 0.f;
	};

	struct Frustum
	{
		float left = 0.f;
		float right = 0.f;
		float bottom = 0.f;
		float top = 0.f;
		float near = 0.f;
		float far = 0.f;
	};

	void init();
	void update();
	void shutdown();

	void init_rays();
	void update_rays();

	void init_views();
	void update_views();
	void render_views();
	void on_gbuffer_ready();

	void save_lidar_data_to_las(Unigine::StringStack<> const& path) const;

	void debug_render_dots() const;
	void debug_render_frustums() const;
	void debug_update_sprites();

	View::Location cast_ray_onto_view(Unigine::Math::vec3 const& direction);

	float read_depth(View::Location location) const;
	float read_intensity(View::Location location) const;

	Unigine::Vector<View> views;
	Unigine::Vector<Ray> rays;

	Unigine::Math::Mat4 last_transform;

	Unigine::RenderTargetPtr render_target;
	Unigine::ViewportPtr viewport;
	Unigine::CameraPtr camera;
	Frustum frustum;

	int active_view_idx = 0;
	bool visualizer_enabled = false;
	bool console_on_screen = false;

	Unigine::EventConnections connections;
};

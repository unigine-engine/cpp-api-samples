#include "Lidar.h"

#include <UnigineGame.h>
#include <UnigineConsole.h>
#include <UnigineProfiler.h>
#include <UnigineVisualizer.h>

#include "Las.h"

REGISTER_COMPONENT(Lidar)

using namespace Unigine;
using namespace Math;

void Lidar::init()
{
	// setup the perspective projection frustum that is used when rendering each view
	{
		frustum.near = Math::min(0.1f, distance_range.get().x);
		frustum.far = distance_range.get().y;
		frustum.left = frustum.near * Math::tan(-Consts::PI / float(num_views));
		frustum.right = frustum.near * Math::tan(Consts::PI / float(num_views));
		frustum.bottom = frustum.near * Math::tan(-50.f * Consts::DEG2RAD);
		frustum.top = frustum.near * Math::tan(50 * Consts::DEG2RAD);
	}

	// create camera
	{
		camera = Camera::create();
		camera->setProjection(Math::frustum(
			frustum.left,
			frustum.right,
			frustum.bottom,
			frustum.top,
			frustum.near,
			frustum.far
		));
	}

	// create viewport and render target that will be used for rendering views
	{
		viewport = Viewport::create();
		viewport->setSkipFlags(~0); // set all skip flags - i.e. we don't want posteffects, shadows, etc.
		viewport->setLifetime(60 * 60); // set a large lifetime so that viewport doesn't free tmp resources between renders
		viewport->setRenderMode(Viewport::RENDER_DEPTH_GBUFFER); // we will need depth and gbuffer
		viewport->getEventEndOpacityGBuffer().connect(connections, this, &Lidar::on_gbuffer_ready);

		render_target = RenderTarget::create();
	}

	{
		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(is_debug);

		console_on_screen = Console::isOnscreen();
		Console::setOnscreen(true);
	}

	init_views();
	init_rays();
}

void Lidar::update()
{
	// update all views when none are awaiting texture-to-image transfer
	if (std::all_of(views.begin(), views.end(), [](auto const& view) { return view.state == View::IDLE || view.state == View::UNUSED; }))
		update_views();

	// update all rays when all views' textures have been updated and transfered to cpu
	if (std::all_of(views.begin(), views.end(), [](auto const& view) { return view.state == View::UPDATED || view.state == View::UNUSED; }))
		update_rays();

	if (is_debug)
	{
		if (is_debug_render_points)
			debug_render_dots();

		if (is_debug_render_frustums)
			debug_render_frustums();

		debug_update_sprites();
	}
}

void Lidar::shutdown()
{
	for (auto& view : views)
	{
		view.debug_sprite.deleteLater();
	}

	Visualizer::setEnabled(visualizer_enabled);
	Console::setOnscreen(console_on_screen);
}

void Lidar::init_views()
{
	views.resize(num_views);

	for (int i = 0; i < num_views; i += 1)
	{
		auto& view = views[i];

		{
			// assume the view is unused by default
			view.state = View::UNUSED;
		}

		// calculate view / plane transform
		{
			float turn = float(i) / float(num_views);
			float plane_angle = (turn - .25f) * Consts::PI2;
			view.plane = vec4(Math::cos(plane_angle), Math::sin(plane_angle), 0.f, frustum.near);
			view.transform = rotateZ(360.f * turn) * rotateX(90.f);
		}

		{
			// the texture will contain depth and intensity information in two 32-bit float channels:
			// R channel for linear depth, and G channel for intensity
			int format = Texture::FORMAT_RG32F;
			int flags = Texture::FORMAT_USAGE_RENDER; // set FORMAT_USAGE_RENDER flag because we will be rendering to this texture

			view.texture = Texture::create();
			view.texture->create2D(resolution.get().x, resolution.get().y, format, flags);
		}

		{
			view.image = Image::create();
		}

		// create a debug sprite for rendering the texture with depth/intensity data
		{
			int sprite_size = ftoi(itof(WindowManager::getMainWindow()->getSize().x) * 0.65f / num_views);

			view.debug_sprite = WidgetSprite::create("core/textures/common/black.texture");
			view.debug_sprite->setPosition(i * (sprite_size + 5), WindowManager::getMainWindow()->getSize().y - sprite_size);
			view.debug_sprite->setWidth(sprite_size);
			view.debug_sprite->setHeight(sprite_size);
			view.debug_sprite->setRender(view.texture, 0);
			view.debug_sprite->setHidden(!(is_debug && is_debug_render_textures));

			Gui::getCurrent()->addChild(view.debug_sprite, Gui::ALIGN_OVERLAP);
		}
	}
}

void Lidar::init_rays()
{
	// set up ray direction vectors and find their corresponding locations on the view planes

	rays.resize(yaw_rays * pitch_rays);

	int num_stacks = pitch_rays;
	int num_slices = yaw_rays;

	int ray_idx = 0;

	for (int stack_idx = 0; stack_idx < num_stacks; stack_idx += 1)
	{
		float phi_time = float(stack_idx) / float(num_stacks - 1);
		float phi = lerp(pitch_angle_range.get().x, pitch_angle_range.get().y, phi_time) * Consts::DEG2RAD;

		for (int slice_idx = 0; slice_idx < num_slices; slice_idx += 1)
		{
			float theta_time = float(slice_idx) / float(num_slices - 1);
			float theta = lerp(yaw_angle_range.get().x, yaw_angle_range.get().y, theta_time) * Consts::DEG2RAD - Consts::PI05;

			vec3 direction = vec3(Math::cos(phi) * Math::cos(theta), -Math::cos(phi) * Math::sin(theta), Math::sin(phi));

			{
				Ray& ray = rays[ray_idx];

				ray.direction = direction;
				ray.location = cast_ray_onto_view(direction);
				ray.distance = 0.f;
				ray.intensity = 0.f;
			}

			ray_idx += 1;
		}
	}

	// sort the rays based on their image/pixel location to ensure (more or less) linear memory access
	// later on in Lidar::refresh_rays() when reading image pixel data to extract depth and intensity --
	// a predictable memory access pattern will help performance a bit

	quickSort<Ray>(rays.get(), rays.size(), [](Ray const& l, Ray const& r) { return l.location < r.location; });
}

void Lidar::update_views()
{
	last_transform = node->getWorldTransform();

	render_views();
}

void Lidar::render_views()
{
	// render all views to textures and schedule texture-to-image transfers

	UNIGINE_PROFILER_FUNCTION;

	for (int i = 0; i < num_views; i += 1)
	{
		auto& view = views[i];

		// skip unused views
		if (view.state == View::UNUSED)
			continue;

		// render the view
		{
			active_view_idx = i;
			camera->setModelview(inverse(last_transform * Mat4(view.transform)));

			RenderState::saveState();
			RenderState::clearStates();
			RenderState::setViewport(0, 0, resolution.get().x, resolution.get().y);

			render_target->enable();

			{
				// calls Lidar::on_gbuffer_ready
				viewport->render(camera);
			}

			render_target->disable();

			RenderState::restoreState();
		}

		// transfer the rendered texture to image
		{
			auto on_transfered = [&view, pnode = node](ImagePtr)
			{
				// because this is an async callback
				// check that the node is still alive
				if (pnode.isDeleted())
					return;

				// set the view state to 'UPDATED' in the main thread
				view.state = View::UPDATED;
			};

			auto on_transfered_async = [&view, pnode = node](ImagePtr image)
			{
				// because this is an async callback
				// check that the node is still alive
				if (pnode.isDeleted())
					return;

				// grab the new image and clear the old image

				// deleting a large image can take a while
				// so we do this here in the async thread

				view.image->swap(image);
				image->clear();
			};

			view.state = View::TRANSFER;

			Render::asyncTransferTextureToImage(
				MakeCallback(on_transfered),
				MakeCallback(on_transfered_async),
				view.texture
			);
		}
	}
}

void Lidar::on_gbuffer_ready()
{
	// when the gbuffer is ready we run our post material (fragment shader) that:
	// a) stores the linear depth for each fragment in the R-channel as 32-bit float
	// b) calculates intensity for each fragment using the metalness, roughness and normal textures from gbuffer

	// see "lidar_post.basemat" for further details

	auto& view = views[active_view_idx];

	// use the existing post render target for rendering
	auto post_render_target = Renderer::getPostRenderTarget();

	RenderState::saveState();
	RenderState::clearStates();

	// bind textures that our post material will use as input
	RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, Renderer::getTextureOpacityDepth()); // depth
	RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, Renderer::getTextureGBufferNormal()); // normal, roughness
	RenderState::setTexture(RenderState::BIND_FRAGMENT, 2, Renderer::getTextureGBufferShading()); // metalness

	// bind our view's RG32F texture that will be used as output by our material
	post_render_target->bindColorTexture(0, view.texture);
	post_render_target->enable();

	{
		lidar_post_material->renderScreen(Render::PASS_POST);
	}

	post_render_target->disable();
	post_render_target->unbindColorTextures();

	RenderState::restoreState();
}

void Lidar::update_rays()
{
	// update each ray's distance (depth) and intesity data
	// by reading the corresponding pixel data in the view images

	UNIGINE_PROFILER_FUNCTION;

	for (auto& ray : rays)
	{
		if (ray.location)
		{
			ray.distance = read_depth(ray.location);
			ray.intensity = read_intensity(ray.location);
			views[ray.location.view_idx].state = View::IDLE;
		}
	}
}

float Lidar::read_depth(View::Location location) const
{
	// read the pixel data at the R 32-bit float channel: depth

	return * reinterpret_cast<float *>(views[location.view_idx].image->getPixels() + location.pixel_offset);
}

float Lidar::read_intensity(View::Location location) const
{
	// read the pixel data at the G 32-bit float channel: intensity

	return * reinterpret_cast<float *>(views[location.view_idx].image->getPixels() + location.pixel_offset + sizeof(float));
}

Lidar::View::Location Lidar::cast_ray_onto_view(vec3 const& direction)
{
	// find the intersection of a ray (with the given direction vector) with the view planes
	// and return the location of the intersection point as view(image) id + pixel offset

	View::Location result;

	for (int i = 0; i < num_views; i += 1)
	{
		View& view = views[i];

		// try to intersect the view plane
		vec3 point;

		if (!rayPlaneIntersection(vec3_zero, direction, view.plane, point))
			continue;

		// get coordinates from the plane's center
		float x = dot(point, cross(vec3_up, vec3(view.plane)));
		float y = dot(point, vec3_up);

		if (x < frustum.left || x > frustum.right || y < frustum.bottom || y > frustum.top)
			continue;

		{
			// ray hit a plane

			// normalize coordinates [0, 1]
			x = (x - frustum.left) / (frustum.right - frustum.left);
			y = (y - frustum.bottom) / (frustum.top - frustum.bottom);

			// flip y
			y = 1.f - y;

			// clear the "unused" flag
			view.state = View::IDLE;

			// store the view index
			result.view_idx = uint32_t(i);

			// find and store pixel offset in the view texture/image
			{
				uint32_t pixel_size = sizeof(float /* depth */) + sizeof(float /* intensity */);

				result.pixel_offset = {
					uint32_t(resolution.get().x) * (uint32_t(y * float(resolution.get().y)) * pixel_size) +
					uint32_t(x * float(resolution.get().x)) * pixel_size
				};
			}

			break;
		}
	}

	return result;
}

void Lidar::save_lidar_data_to_las(StringStack<> const& path) const
{
	// save rays as point cloud where each ray, if it hit anything, is
	// described as a hit point with position vector (x, y, z) and intensity

	// the point cloud is saved in the LAS format

	Las::LasWriter las_writer;
	las_writer.init();

	for (auto const& ray : rays)
	{
		if (ray.distance < distance_range.get().x || ray.distance > distance_range.get().y)
			continue;

		vec3 ray_position = ray.direction * ray.distance;
		las_writer.add_point(ray_position, ray.intensity);
	}

	las_writer.save(path);

	Log::message("Saved lidar data to %s\n", FileSystem::getAbsolutePath(path).get());
}

void Lidar::debug_render_dots() const
{
	for (auto const& ray : rays)
	{
		bool is_valid = {
			ray.location &&
			ray.distance > distance_range.get().x &&
			ray.distance < distance_range.get().y
		};

		if (!is_valid)
			continue;

		Vec3 position = last_transform * Vec3(ray.direction * ray.distance);
		Visualizer::renderPoint3D(position, 0.0075f, vec4(vec3(ray.intensity), 1.f));
	}
}

void Lidar::debug_render_frustums() const
{
	mat4 projection = camera->getProjection();

	for (int i = 0; i < num_views; i += 1)
	{
		Mat4 transform = node->getWorldTransform() * Mat4(views[i].transform);

		Visualizer::renderFrustum(projection, transform, vec4(itof(i) / num_views, 1, itof(4 - i) / num_views, 1));

		{
			// float arrow_size = .6f;
			// float arrow_end_size = .15f;
			// float arrow_opacity = .8f;

			Vec3 position = transform.getTranslate();
			// Vec3 axis_x = transform.getAxisX();
			// Vec3 axis_y = transform.getAxisY();
			Vec3 axis_z = transform.getAxisZ();

			// Visualizer::renderVector(position, position + axis_x * arrow_size, vec4_red * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			// Visualizer::renderVector(position, position + axis_y * arrow_size, vec4_green * vec4(vec3(1.f), arrow_opacity), arrow_end_size);
			// Visualizer::renderVector(position, position + axis_z * arrow_size, vec4_blue * vec4(vec3(1.f), arrow_opacity), arrow_end_size);

			Visualizer::renderMessage3D(position - axis_z, vec3(0.f), String::format("%d", i), vec4_white);
		}
	}
}

void Lidar::debug_update_sprites()
{
	for (int i = 0; i < num_views; i += 1)
	{
		auto& view = views[i];
		float sprite_size = float(view.debug_sprite->getWidth());
		view.debug_sprite->setPosition(i * (sprite_size + 5), WindowManager::getMainWindow()->getSize().y - sprite_size);
		view.debug_sprite->setHidden(!is_debug_render_textures);
	}
}

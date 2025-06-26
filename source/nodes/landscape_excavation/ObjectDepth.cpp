#include "ObjectDepth.h"
#include <Unigine.h>
#include <UnigineTextures.h>

using namespace Unigine;
using namespace Math;

ObjectDepth::ObjectDepth()
{
	camera = Camera::create();

	viewport = Viewport::create();
	viewport->setAspectCorrection(0);
	viewport->setLifetime(60 * 60 * 10); // keep the temporary resources alive between calls to viewport->render*
	viewport->setSkipFlags(~0); // set all skip flags, since we don't want any shadows, post effects, etc.

	render_target = RenderTarget::create();

	// set render mode to RENDER_DEPTH, as we will only be using the depth buffer
	viewport->setRenderMode(Viewport::RENDER_DEPTH);

	// set up the end screen callback where the actual rendering will take place
	viewport->getEventEndScreen().connect(on_end_screen_connection, this, &ObjectDepth::on_end_screen);

	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("digging_tool.basemat"));
	if (guid.isValid())
		digging_tool_mat = Materials::findMaterialByFileGUID(guid)->inherit();
	else
		Log::warning("ObjectDepth::ObjectDepth(): can not find \"digging_tool.basemat\" material\n");
}

void ObjectDepth::renderDepthTexture(
	TexturePtr &tex,
	const NodePtr &target,
	const Vec3 &pos,
	const Vec3 &size,
	const ivec2 &resolution
)
{
	if (!target)
		return;

	Vec3 center = pos + size * .5f;

	// save the bound box height and min z: these values will later be used
	// in the `digging_tool.basemat` shader, in the "object_depth" pass, when
	// mapping the depth value from clip space [0, 1] to the world space z
	{
		bb_height = (float)size.z;
		bb_min_z = (float)pos.z;
	}

	// set the camera's orthographic projection and modelview such that it matches the given position and size of the object's bound box
	{
		mat4 projection = ortho((float)(-size.x * 0.5f), (float)(size.x * 0.5f), (float)(-size.y * 0.5f), (float)(size.y * 0.5f), 0.0f, (float)size.z);
		Mat4 transform = translate(Vec3(center.x, center.y, pos.z)) * Mat4(rotateX(-180.0f));

		camera->setProjection(projection);
		camera->setModelview(inverse(transform));
	}

	// render the digging object to our texture
	{
		RenderState::saveState();
		RenderState::clearStates();
		RenderState::setViewport(0, 0, resolution.x, resolution.y);

		render_target->bindColorTexture(0, tex);
		render_target->enable();

		{
			viewport->renderNode(camera, target);
		}

		render_target->disable();
		render_target->unbindColorTexture(0);

		RenderState::restoreState();
	}
}

void ObjectDepth::on_end_screen()
{
	digging_tool_mat->setTexture("depth", Renderer::getTextureCurrentDepth());
	digging_tool_mat->setParameterFloat("bb_height", bb_height);
	digging_tool_mat->setParameterFloat("bb_min_z", bb_min_z);
	digging_tool_mat->renderScreen("object_depth");
}

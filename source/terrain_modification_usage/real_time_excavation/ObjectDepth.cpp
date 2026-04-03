// Renders an object's depth buffer from an orthographic top-down view. The depth
// texture is used by the excavation system to determine how deep the object cuts
// into the terrain at each texel position.

#include "ObjectDepth.h"
#include <Unigine.h>
#include <UnigineTextures.h>

using namespace Unigine;
using namespace Math;

ObjectDepth::ObjectDepth()
{
	camera = Camera::create();

	viewport = Viewport::create();
	// Disable aspect ratio correction for exact pixel mapping
	viewport->setAspectCorrection(0);
	// Keep temporary resources alive for 10 hours to reuse between renders
	viewport->setLifetime(60 * 60 * 10);
	// Skip all post-effects, shadows, etc. since only depth is needed
	viewport->setSkipFlags(~0);

	render_target = RenderTarget::create();

	// RENDER_DEPTH mode outputs only depth buffer, no color
	viewport->setRenderMode(Viewport::RENDER_DEPTH);

	// Connect callback where shader pass converts depth to world-space height
	viewport->getEventEndScreen().connect(on_end_screen_connection, this, &ObjectDepth::on_end_screen);

	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("digging_tool.basemat"));
	if (guid.isValid())
		// inherit() creates modifiable material instance
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

	// Store bounding box parameters for shader depth-to-world conversion
	// Shader maps clip-space [0,1] depth back to world-space Z coordinates
	{
		bb_height = (float)size.z;
		bb_min_z = (float)pos.z;
	}

	// Configure orthographic camera to exactly match bounding box dimensions
	{
		// Orthographic projection spans the XY extent of the bounding box
		mat4 projection = ortho((float)(-size.x * 0.5f), (float)(size.x * 0.5f), (float)(-size.y * 0.5f), (float)(size.y * 0.5f), 0.0f, (float)size.z);
		// Camera positioned at box bottom, looking up (rotated 180 around X)
		Mat4 transform = translate(Vec3(center.x, center.y, pos.z)) * Mat4(rotateX(-180.0f));

		camera->setProjection(projection);
		// Modelview = inverse of world transform
		camera->setModelview(inverse(transform));
	}

	// Render the object to depth texture
	{
		// Save and clear render state to avoid interference
		RenderState::saveState();
		RenderState::clearStates();
		RenderState::setViewport(0, 0, resolution.x, resolution.y);

		// Bind output texture as color attachment (depth converted to color)
		render_target->bindColorTexture(0, tex);
		render_target->enable();

		{
			// renderNode draws a single node with the configured camera
			viewport->renderNode(camera, target);
		}

		render_target->disable();
		render_target->unbindColorTexture(0);

		RenderState::restoreState();
	}
}

void ObjectDepth::on_end_screen()
{
	// Pass current depth buffer and bounding box info to shader
	digging_tool_mat->setTexture("depth", Renderer::getTextureCurrentDepth());
	digging_tool_mat->setParameterFloat("bb_height", bb_height);
	digging_tool_mat->setParameterFloat("bb_min_z", bb_min_z);
	// Execute fullscreen pass to convert depth to world-space height
	digging_tool_mat->renderScreen("object_depth");
}

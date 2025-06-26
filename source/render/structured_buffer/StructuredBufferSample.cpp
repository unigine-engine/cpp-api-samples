#include "StructuredBufferSample.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(StructuredBufferSample)

void StructuredBufferSample::init()
{
	// Compute material
	MaterialPtr compute_material = Materials::findManualMaterial("gpu_dxt_compute");

	// Load source texture
	TexturePtr source_texture = Texture::create();
	source_texture->load(source_texture_param.get());

	Log::message("Source texture size %ix%i: %i kb\n", source_texture->getWidth(),
		source_texture->getHeight(),
		(source_texture->getWidth() * source_texture->getHeight() * 3) / 1024);

	// Output structure
	struct DXT1Block
	{
		unsigned int colors;
		unsigned int indices;
	};

	// Calculate DXT Size
	const int dxt_size_x = source_texture->getWidth() / 4;
	const int dxt_size_y = source_texture->getHeight() / 4;

	// Defined by shader
	const int threads_in_group_x = 8;
	const int threads_in_group_y = 8;

	// Create output structure buffer (gpu_read-readwrite)
	StructuredBufferPtr dxt_buffer = StructuredBuffer::create();
	dxt_buffer->create(StructuredBuffer::USAGE_RENDER, NULL, sizeof(DXT1Block),
		dxt_size_x * dxt_size_y);

	Unigine::Log::message("Structure buffer (compressed DXT) size in video memory: %s\n",
		Unigine::String::memory(sizeof(DXT1Block) * dxt_size_x * dxt_size_y).get());

	// Create compute render
	RenderTargetPtr render_target = RenderTarget::create();

	// Bind source texture
	RenderState::setTexture(RenderState::BIND_ALL, 0, source_texture);

	// Setup unordered access buffer (read-write)
	render_target->bindStructuredBuffer(0, dxt_buffer);
	render_target->enableCompute();

	// Calculate groups for dispatch compute shader
	int groups_x = (dxt_size_x) / threads_in_group_x;
	int groups_y = (dxt_size_y) / threads_in_group_y;

	if ((dxt_size_x) % threads_in_group_x > 0)
	{
		groups_x++;
	}

	if ((dxt_size_y) % threads_in_group_y > 0)
	{
		groups_y++;
	}

	long begin_time = clock();

	// Calculate warm-up time (shader compilation)
	compute_material->renderCompute(Render::PASS_POST, 1, 1);
	render_target->flush();

	float time_warmup = float(clock() - begin_time) / CLOCKS_PER_SEC * 1000;
	begin_time = clock();

	// Dispatch groups_x * groups_y groups threads, 256 thread each
	compute_material->renderCompute(Render::PASS_POST, groups_x, groups_y);
	render_target->flush();

	float time_execution = float(clock() - begin_time) / CLOCKS_PER_SEC * 1000;

	Unigine::Log::message("Shader setup and compilation time: %f ms\n", time_warmup);
	Unigine::Log::message(
		"Dispatched %ix%i group threads (%ix%i threads each), execution time: %f ms\n", groups_x,
		groups_y, threads_in_group_x, threads_in_group_y, time_execution);

	render_target->disable();
	render_target->unbindStructuredBuffers();

	// Create image
	ImagePtr dxt_image = Image::create();
	dxt_image->create2D(source_texture->getWidth(), source_texture->getHeight(), Image::FORMAT_DXT1,
		1, 0, 1);

	begin_time = clock();

	// Transfer data from GPU to Image CPU
	Render::asyncTransferStructuredBuffer(nullptr,
		MakeCallback([begin_time, dxt_image, dxt_buffer](void *data) {
			Unigine::Log::message("Transfer GPU -> CPU time: %f ms\n",
				float(clock() - begin_time) / CLOCKS_PER_SEC * 1000);
			memcpy(dxt_image->getPixels(), data, sizeof(DXT1Block) * dxt_buffer->getNumElements());

			// Save image
			String path = String::format("%s%s", Engine::get()->getSavePath(),
				"compressed_image.dds");
			dxt_image->save(path.get());
			Unigine::Log::message("Compressed texture saved: %s\n", path.get());
		}),
		dxt_buffer);

	dxt_buffer.clear();
}

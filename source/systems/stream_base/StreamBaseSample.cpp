#include "StreamBaseSample.h"

#include <UnigineConsole.h>

MyFile::MyFile(const char *path, const char *mode)
{
	const Unigine::String absolute_path = Unigine::FileSystem::getAbsolutePath(path);
	file = fopen(absolute_path, mode);
}

MyFile::~MyFile()
{
	fclose(file);
}

int MyFile::isOpened()
{
	return file != nullptr;
}

int MyFile::isAvailable()
{
	return feof(file) != 0;
}

size_t MyFile::read(void *ptr, size_t size, size_t nmemb)
{
	return fread(ptr, size, nmemb, file);
}

size_t MyFile::write(const void *ptr, size_t size, size_t nmemb)
{
	return fwrite(ptr, size, nmemb, file);
}

void StreamBaseSample::sample()
{
	Unigine::String file_str = filepath.get();
	if (file_str.size() <= 0)
	{
		Unigine::Log::error("StreamBaseSample::sample(): Filepath param is empty!");
		return;
	}

	Unigine::Console::setOnscreen(true);
	// create the file
	{
		MyFile file(file_str, "wb");

		// here you can get access to stream
		// and use all of its features
		Unigine::StreamPtr stream = file.getStream();
		stream->writeString("Some string");
		stream->writeInt(42);
		stream->writeFloat(3.14f);
	}

	// read from the file
	{
		MyFile file(file_str, "rb");
		Unigine::StreamPtr stream = file.getStream();
		Unigine::Log::message("%s\n", stream->readString().get());
		Unigine::Log::message("%d\n", stream->readInt());
		Unigine::Log::message("%f\n", stream->readFloat());
	}
}

void StreamBaseSample::shutdown()
{
	Unigine::Console::setOnscreen(false);
}

REGISTER_COMPONENT(StreamBaseSample)

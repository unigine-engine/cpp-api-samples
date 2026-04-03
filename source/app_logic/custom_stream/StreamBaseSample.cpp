// Demonstrates custom stream implementation by wrapping C stdio functions with
// StreamBase interface. Custom streams enable integration of external I/O sources
// (network, compression, encryption) with Unigine's serialization system.

#include "StreamBaseSample.h"
#include <UnigineConsole.h>

// File stream is opened at specified path with given mode.
MyFile::MyFile(const char *path, const char *mode)
{
	const Unigine::String absolute_path = Unigine::FileSystem::getAbsolutePath(path);
	file = fopen(absolute_path, mode);
}

// File handle is closed when stream is destroyed.
MyFile::~MyFile()
{
	if (file)
		fclose(file);
}

// Non-zero is returned if file was successfully opened.
int MyFile::isOpened()
{
	return file != nullptr;
}

// Non-zero is returned if data is available for reading (not at EOF).
int MyFile::isAvailable()
{
	return file && feof(file) == 0;
}

// Data is read from file into buffer; number of elements read is returned.
size_t MyFile::read(void *ptr, size_t size, size_t nmemb)
{
	return fread(ptr, size, nmemb, file);
}

// Data is written from buffer to file; number of elements written is returned.
size_t MyFile::write(const void *ptr, size_t size, size_t nmemb)
{
	return fwrite(ptr, size, nmemb, file);
}

// Custom stream is demonstrated by writing and reading typed data.
void StreamBaseSample::sample()
{
	Unigine::String file_str = filepath.get();
	if (file_str.size() <= 0)
	{
		Unigine::Log::error("StreamBaseSample::sample(): Filepath param is empty!");
		return;
	}

	Unigine::Console::setOnscreen(true);

	// Data is written to file using custom stream
	{
		MyFile file(file_str, "wb");
		Unigine::StreamPtr stream = file.getStream();
		stream->writeString("Some string");
		stream->writeInt(42);
		stream->writeFloat(3.14f);
	}

	// Data is read back from file using custom stream
	{
		MyFile file(file_str, "rb");
		Unigine::StreamPtr stream = file.getStream();
		Unigine::Log::message("%s\n", stream->readString().get());
		Unigine::Log::message("%d\n", stream->readInt());
		Unigine::Log::message("%f\n", stream->readFloat());
	}
}

// On-screen console is disabled.
void StreamBaseSample::shutdown()
{
	Unigine::Console::setOnscreen(false);
}

REGISTER_COMPONENT(StreamBaseSample)

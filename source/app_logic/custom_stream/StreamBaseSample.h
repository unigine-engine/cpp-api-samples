#pragma once
#include <UnigineComponentSystem.h>

// Demonstrates implementing a custom stream by extending StreamBase.
// StreamBase provides an abstract interface for binary I/O that can wrap
// any underlying data source. Here, standard C FILE operations are wrapped
// to integrate with UNIGINE's streaming API (readString, writeInt, etc.).

// Custom stream implementation for binary file I/O using stdio FILE.
// Wraps fopen/fread/fwrite to provide StreamBase compatibility.
class MyFile final : public Unigine::StreamBase
{
public:
	MyFile(const char *path, const char *mode);
	~MyFile() override;
	int isOpened() override;
	int isAvailable() override;
	size_t read(void *ptr, size_t size, size_t nmemb) override;
	size_t write(const void *ptr, size_t size, size_t nmemb) override;

private:
	FILE *file;
};

class StreamBaseSample final : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(StreamBaseSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates custom file streaming by implementing a wrapper "
							"around standard file I/O.")

	COMPONENT_INIT(sample)
	COMPONENT_SHUTDOWN(shutdown)

	// File path parameter (set in the Editor)
	PROP_PARAM(File, filepath)

private:
	void sample();
	void shutdown();
};

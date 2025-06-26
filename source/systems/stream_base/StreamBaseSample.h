#pragma once
#include <UnigineComponentSystem.h>


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

	COMPONENT_INIT(sample)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(File, filepath)

private:
	void sample();
	void shutdown();
};

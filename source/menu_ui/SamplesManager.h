#pragma once

#include <UnigineVector.h>
#include <UnigineString.h>
#include <UnigineImage.h>


struct Sample;
struct Category;

class SamplesManager
{
public:
	static SamplesManager *get()
	{
		static SamplesManager instance;
		return &instance;
	}
	~SamplesManager() {};

	bool parseMetaXml(Unigine::String path_relative_to_data);
	bool isEmpty() const;
	void clear();

	const Unigine::Vector<Category> &getCategories() const { return categories; }
	const Unigine::Vector<Unigine::String> &getTags() const { return tags; }

	const Category *getCategoryBySampleID(const Unigine::String &world_path) const;
	const Sample *getSampleByWorldPath(Unigine::String world_path) const;
	const Sample *getSampleByID(Unigine::String &id) const;
	void getPrevNextSamplesID(const Unigine::String &world_name, Unigine::String &prev_world,
		Unigine::String &next_world);

private:
	SamplesManager() {};

private:
	Unigine::Vector<Category> categories;
	Unigine::Vector<Unigine::String> tags;
	// <ID (world_name), Sample>
	Unigine::HashMap<Unigine::String, Sample> samples_map;

	Unigine::String meta_path;
};


struct Sample
{
	Unigine::String title;
	Unigine::String description;
	Unigine::Vector<Unigine::String> tags;
	Unigine::String world_name;

	Unigine::String category_id;
};


struct Category
{
	Unigine::String id;
	Unigine::ImagePtr icon;
	Unigine::String title;

	Unigine::Vector<Sample> samples;
};

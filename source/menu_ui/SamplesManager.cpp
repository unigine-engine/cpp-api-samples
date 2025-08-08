#include "SamplesManager.h"

#include <UnigineXml.h>
#include <UnigineFileSystem.h>
#include <UnigineEngine.h>

using namespace Unigine;

bool SamplesManager::parseMetaXml(Unigine::String path_relative_to_data)
{
	if (!isEmpty() && path_relative_to_data == meta_path)
		return true;

	String cpp_samples_xml_path = FileSystem::getAbsolutePath(
		String::joinPaths(Engine::get()->getDataPath(), path_relative_to_data));

	XmlPtr cpp_samples_xml = Xml::create();
	if (!cpp_samples_xml->load(cpp_samples_xml_path))
	{
		Unigine::Log::warning("SamplesMetaParser::parseMetaXml(): can't open %s file\n",
			cpp_samples_xml_path.get());
		return false;
	}

	clear();
	meta_path = path_relative_to_data;

	XmlPtr cpp_samples_samples_pack = cpp_samples_xml->getChild("samples_pack");
	XmlPtr categories_xml = cpp_samples_samples_pack->getChild("categories");
	XmlPtr samples_xml = cpp_samples_samples_pack->getChild("samples");

	HashMap<String, Category> categories_map;
	Vector<String> categories_id;
	HashSet<String> tags_set;

	for (int i = 0; i < categories_xml->getNumChildren(); ++i)
	{
		XmlPtr category_xml = categories_xml->getChild(i);

		String icon_path = category_xml->getArg("img");
		icon_path = icon_path.trimFirst("data/");

		Category c;
		c.icon = Image::create(icon_path.get());
		c.title = category_xml->getArg("name");

		c.id = category_xml->getArg("id");

		categories_id.append(c.id);
		categories_map[c.id] = c;
	}

	for (int i = 0; i < samples_xml->getNumChildren(); ++i)
	{
		XmlPtr sample_xml = samples_xml->getChild(i);

		Sample s;
		s.title = sample_xml->getArg("title");
		s.description = sample_xml->getChild("sdk_desc")->getData();
		s.world_name = sample_xml->getArg("id");

		XmlPtr tags_xml = sample_xml->getChild("tags");
		for (int j = 0; j < tags_xml->getNumChildren(); ++j)
		{
			String tag = tags_xml->getChild(j)->getData();

			if (!tags_set.contains(tag))
				tags_set.insert(tag);

			s.tags.push_back(tag);
		}

		s.category_id = sample_xml->getArg("category_id");

		if (categories_map.contains(s.category_id))
			categories_map[s.category_id].samples.push_back(s);
		else
			Unigine::Log::error("Category with id %s doesn't exist in .sample file\n",
				s.category_id.get());

		samples_map[s.world_name] = s;
	}

	categories.clear();

	for (const auto &id : categories_id)
	{
		if (categories_map.contains(id) && categories_map.value(id).samples.size() > 0)
			categories.push_back(categories_map.value(id));
	}
	categories_id.clear();

	tags.clear();
	for (const auto &t : tags_set)
	{
		tags.push_back(t.key);
	}

	return true;
}

bool SamplesManager::isEmpty() const
{
	if (meta_path.empty() || categories.empty())
		return true;
	else
		return false;
}

void SamplesManager::clear()
{
	meta_path = "";
	categories.clear();
	samples_map.clear();
	tags.clear();
}

const Category *SamplesManager::getCategoryBySampleID(const Unigine::String &sample_id) const
{
	if (isEmpty() || !samples_map.contains(sample_id))
		return nullptr;

	const Sample &sample = samples_map[sample_id];

	for (auto &category : categories)
	{
		if (category.id == sample.category_id)
			return &category;
	}

	return nullptr;
}

const Sample *SamplesManager::getSampleByWorldPath(Unigine::String world_path) const
{
	String sample_id = String::filename(world_path);
	return getSampleByID(sample_id);
}

const Sample *SamplesManager::getSampleByID(Unigine::String &id) const
{
	if (isEmpty() || !samples_map.contains(id))
		return nullptr;

	return &samples_map[id];
}

void SamplesManager::getPrevNextSamplesID(const Unigine::String &world_name,
	Unigine::String &prev_world, Unigine::String &next_world)
{
	auto *category = getCategoryBySampleID(world_name);
	if (!category)
	{
		prev_world = world_name;
		next_world = world_name;
		return;
	}

	auto &samples = category->samples;
	for (int i = 0; i < samples.size(); i++)
	{
		String id = samples[i].world_name;
		if (id != world_name)
			continue;

		prev_world = i == 0 ? samples.last().world_name : samples[i - 1].world_name;
		next_world = i == samples.size() - 1 ? samples.first().world_name
											 : samples[i + 1].world_name;
		return;
	}
}

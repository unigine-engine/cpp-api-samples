#include "DescriptionWindowCreator.h"

REGISTER_COMPONENT(DescriptionWindowCreator);

SampleDescriptionWindow &DescriptionWindowCreator::getWindow()
{
	init();
	return sample_description_window;
}

void DescriptionWindowCreator::init()
{
	if (!sample_description_window.getWindow())
	{
		sample_description_window.createWindow(window_align, window_width);
	}
}

void DescriptionWindowCreator::shutdown()
{
	sample_description_window.shutdown();
}

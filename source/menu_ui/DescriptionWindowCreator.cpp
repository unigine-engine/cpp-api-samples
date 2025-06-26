#include "DescriptionWindowCreator.h"

REGISTER_COMPONENT(DescriptionWindowCreator);

void DescriptionWindowCreator::init()
{
	sample_description_window.createWindow(window_align, window_width);
}

void DescriptionWindowCreator::shutdown()
{
	sample_description_window.shutdown();
}

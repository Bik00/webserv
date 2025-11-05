#include "../../../includes/config/block/EventBlock.hpp"

EventBlock::EventBlock(void)
{
}

EventBlock::~EventBlock(void)
{
}

EventBlock::EventBlock(const EventBlock &ref)
{
	*this = ref;
}

EventBlock &EventBlock::operator=(const EventBlock &ref)
{
	(void)ref;
	return *this;
}


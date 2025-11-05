#include "../../../includes/config/block/LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
}

LocationBlock::~LocationBlock(void)
{
}

LocationBlock::LocationBlock(const LocationBlock &ref)
{
	*this = ref;
}

LocationBlock &LocationBlock::operator=(const LocationBlock &ref)
{
	(void)ref;
	return *this;
}


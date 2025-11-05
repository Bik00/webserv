(#include "../../../includes/config/block/ServerBlock.hpp")

ServerBlock::ServerBlock(void)
{
}

ServerBlock::~ServerBlock(void)
{
}

ServerBlock::ServerBlock(const ServerBlock &ref)
{
	*this = ref;
}

ServerBlock &ServerBlock::operator=(const ServerBlock &ref)
{
	if (this != &ref)
	{
		this->locationBlocks = ref.locationBlocks;
	}
	return *this;
}


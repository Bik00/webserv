#include "../../../includes/config/block/HttpBlock.hpp"

HttpBlock::HttpBlock(void)
{
}

HttpBlock::~HttpBlock(void)
{
}

HttpBlock::HttpBlock(const HttpBlock &ref)
{
	*this = ref;
}

HttpBlock &HttpBlock::operator=(const HttpBlock &ref)
{
	if (this != &ref)
	{
		this->serverBlocks = ref.serverBlocks;
	}
	return *this;
}

void HttpBlock::addServerBlock(const ServerBlock &sb)
{
	this->serverBlocks.push_back(sb);
}


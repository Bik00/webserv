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

bool HttpBlock::addServerBlock(const ServerBlock &sb)
{
	// strict mode: check default_server duplicates per listen addr
	const std::vector<ListenAddr> &newAddrs = sb.getListenAddrs();
	for (size_t i = 0; i < this->serverBlocks.size(); ++i)
	{
	const std::vector<ListenAddr> &existing = this->serverBlocks[i].getListenAddrs();
		for (size_t a = 0; a < existing.size(); ++a)
		{
			for (size_t b = 0; b < newAddrs.size(); ++b)
			{
				if (existing[a].host == newAddrs[b].host && existing[a].port == newAddrs[b].port)
				{
					if (existing[a].defaultServerFlag && newAddrs[b].defaultServerFlag)
					{
						// duplicate default_server for same listen addr
						std::ostringstream oss;
						oss << "duplicate default_server for listen " << existing[a].host << ":" << existing[a].port;
						throw std::runtime_error(oss.str());
					}
				}
			}
		}
	}
	this->serverBlocks.push_back(sb);
	return true;
}


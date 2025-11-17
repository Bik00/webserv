#include "../../../includes/config/block/ServerBlock.hpp"

ServerBlock::ServerBlock(void)
{
	serverNames.clear();
	// BaseBlock constructor initializes root/indexFiles/errorPages/clientMaxBodySize/autoindex
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
		this->listenAddrs = ref.listenAddrs;
		this->serverNames = ref.serverNames;
		this->root = ref.root;
		this->indexFiles = ref.indexFiles;
		this->errorPages = ref.errorPages;
		this->clientMaxBodySize = ref.clientMaxBodySize;
		this->autoindex = ref.autoindex;
	}
	return *this;
}

void ServerBlock::addLocationBlock(const LocationBlock &lb)
{
	this->locationBlocks.push_back(lb);
}


void ServerBlock::addServerName(const std::string &name)
{
	this->serverNames.push_back(name);
}

void ServerBlock::setRoot(const std::string &r)
{
	this->root = r;
}

void ServerBlock::setIndexFiles(const std::vector<std::string> &files)
{
	this->indexFiles = files;
}

void ServerBlock::setClientMaxBodySize(size_t size)
{
	this->clientMaxBodySize = size;
}

void ServerBlock::setAutoindex(bool on)
{
	this->autoindex = on;
}

void ServerBlock::addListen(const std::string &host, int port, bool def)
{
	// normalize host/port defaults
	std::string h = host;
	int p = port;
	if (h.empty()) h = DEFAULT_LISTEN_HOST;
	if (p == 0) p = DEFAULT_LISTEN_PORT;
	// deduplicate: if exists, merge default flag
	for (size_t i = 0; i < this->listenAddrs.size(); ++i)
	{
		if (this->listenAddrs[i].host == h && this->listenAddrs[i].port == p)
		{
			if (def) this->listenAddrs[i].defaultServerFlag = true;
			return;
		}
	}
	ListenAddr la;
	la.host = h;
	la.port = p;
	la.defaultServerFlag = def;
	this->listenAddrs.push_back(la);
}

const std::vector<ListenAddr> &ServerBlock::getListenAddrs() const
{
	return this->listenAddrs;
}

void ServerBlock::clearListenAddrs()
{
	this->listenAddrs.clear();
}

void ServerBlock::ensureDefaultListen()
{
	if (this->listenAddrs.empty())
	{
		ListenAddr la;
		la.host = std::string(DEFAULT_LISTEN_HOST);
		la.port = DEFAULT_LISTEN_PORT;
		la.defaultServerFlag = false;
		this->listenAddrs.push_back(la);
	}
}

void ServerBlock::setDefaultServer(bool def)
{
	if (def)
	{
		// ensure there is at least one listen addr, then mark the first as default
		this->ensureDefaultListen();
		if (!this->listenAddrs.empty())
			this->listenAddrs[0].defaultServerFlag = true;
	}
	else
	{
		// clear default flag from all listen addrs
		for (size_t i = 0; i < this->listenAddrs.size(); ++i)
			this->listenAddrs[i].defaultServerFlag = false;
	}
}


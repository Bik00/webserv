#include "../../../includes/config/block/ServerBlock.hpp"

ServerBlock::ServerBlock(void)
{
	listenHost = std::string();
	listenPort = 0;
	defaultServer = false;
	serverNames.clear();
	root = std::string();
	indexFiles.clear();
	errorPages.clear();
	clientMaxBodySize = 0;
	autoindex = false;
}

ServerBlock::~ServerBlock(void)
{
}

ServerBlock::ServerBlock(const ServerBlock &ref)
{
	this->locationBlocks = ref.locationBlocks;
	this->listenHost = ref.listenHost;
	this->listenPort = ref.listenPort;
	this->defaultServer = ref.defaultServer;
	this->serverNames = ref.serverNames;
	this->root = ref.root;
	this->indexFiles = ref.indexFiles;
	this->errorPages = ref.errorPages;
	this->clientMaxBodySize = ref.clientMaxBodySize;
	this->autoindex = ref.autoindex;
}

ServerBlock &ServerBlock::operator=(const ServerBlock &ref)
{
	if (this != &ref)
	{
		this->locationBlocks = ref.locationBlocks;
	this->listenHost = ref.listenHost;
	this->listenPort = ref.listenPort;
	this->defaultServer = ref.defaultServer;
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


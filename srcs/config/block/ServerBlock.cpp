#include "../../../includes/config/block/ServerBlock.hpp"

ServerBlock::ServerBlock(void)
{
	listenHost = DEFAULT_LISTEN_HOST;
	listenPort = DEFAULT_LISTEN_PORT;
	root = DEFAULT_ROOT;
	defaultServer = false;

	serverNames.clear();
	indexFiles.clear();
	indexFiles.push_back(std::string(DEFAULT_INDEX_FILE));
	errorPages.clear();
	clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;
	autoindex = false;
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

void ServerBlock::addIndexFile(const std::string &f)
{
	this->indexFiles.push_back(f);
}

void ServerBlock::addErrorPage(int code, const std::string &path)
{
	this->errorPages[code] = path;
}

void ServerBlock::setListenHost(const std::string &host)
{
	this->listenHost = host;
}

void ServerBlock::setListenPort(int port)
{
	this->listenPort = port;
}

void ServerBlock::setDefaultServer(bool def)
{
	this->defaultServer = def;
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


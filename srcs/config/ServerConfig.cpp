#include "../../includes/config/ServerConfig.hpp"

ServerConfig::ServerConfig(void) : address("localhost"), port(80), root(""), index("index.html")
{
}

ServerConfig::~ServerConfig(void)
{
}

ServerConfig::ServerConfig(const ServerConfig &ref)
{
    *this = ref;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &ref)
{
    if (this != &ref)
    {
        this->address = ref.address;
        this->port = ref.port;
        this->root = ref.root;
        this->index = ref.index;
        this->error_pages = ref.error_pages;
        this->locations = ref.locations;
    }
    return *this;
}

std::string ServerConfig::getHost(void) const
{
    return this->address;
}

unsigned int ServerConfig::getPort(void) const
{
    return this->port;
}

std::string ServerConfig::getRoot(void) const
{
    return this->root;
}

std::string ServerConfig::getIndex(void) const
{
    return this->index;
}

void ServerConfig::setHost(const std::string &host)
{
    this->address = host;
}

void ServerConfig::setPort(unsigned int port)
{
    this->port = port;
}

void ServerConfig::setRoot(const std::string &root)
{
    this->root = root;
}

void ServerConfig::setIndex(const std::string &index)
{
    this->index = index;
}

void ServerConfig::addServerName(const std::string &name)
{
    this->server_names.push_back(name);
}

void ServerConfig::addErrorPage(int code, const std::string &path)
{
    this->error_pages[code] = path;
}

void ServerConfig::addLocation(const LocationConfig &location)
{
    this->locations.push_back(location);
}
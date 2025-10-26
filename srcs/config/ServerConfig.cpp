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
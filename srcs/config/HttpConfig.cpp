#include "../../includes/Header.hpp"

HttpConfig::HttpConfig(void)
{
}

HttpConfig::~HttpConfig(void)
{
}

HttpConfig::HttpConfig(const HttpConfig &ref)
{
    *this = ref;
}

HttpConfig &HttpConfig::operator=(const HttpConfig &ref)
{
    if (this != &ref)
    {
        this->servers = ref.servers;
    }
    return *this;
}

bool HttpConfig::loadFromFile(const std::string &path)
{
    (void)path;
    // parser not yet implemented
    return false;
}

void    HttpConfig::getInfo(void) const
{
    std::cout << "HttpConfig: " << servers.size() << " servers loaded" << std::endl;
    for (size_t i = 0; i < servers.size(); ++i)
    {
        const ServerConfig &s = servers[i];
        std::cout << "Server " << i << ": " << s.getHost() << ":" << s.getPort() << std::endl;
        std::cout << "  root: " << s.getRoot() << " index: " << s.getIndex() << std::endl;
        std::cout << std::endl;
    }
}
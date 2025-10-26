#include "../../includes/Header.hpp"

HttpConfig::HttpConfig(void) : port(80), address("localhost")
{
}

HttpConfig::HttpConfig(unsigned int port, std::string address) : port(port), address(address)
{
}

HttpConfig::~HttpConfig(void)
{
}

HttpConfig::HttpConfig(const HttpConfig &ref) : port(ref.port), address(ref.address)
{
}

HttpConfig &HttpConfig::operator=(const HttpConfig &ref)
{
    if (this != &ref)
    {
        this->port = ref.port;
        this->address = ref.address;
    }
    return *this;
}

void    HttpConfig::getInfo(void)
{
    std::cout << "HttpConfig has been launched!" << std::endl;
    std::cout << "port: " << this->port << std::endl;
    std::cout << "address: " << this->address << std::endl;
}
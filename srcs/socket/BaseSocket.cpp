#include "../../includes/socket/BaseSocket.hpp"

BaseSocket::BaseSocket(void) : fd(-1), host(std::string()), port(0)
{
}

BaseSocket::BaseSocket(const std::string &h, int p) : fd(-1), host(h), port(p)
{
}

BaseSocket::~BaseSocket(void)
{
    setClose();
}

BaseSocket::BaseSocket(const BaseSocket &ref) : fd(ref.fd), host(ref.host), port(ref.port)
{
}

BaseSocket &BaseSocket::operator=(const BaseSocket &ref)
{
    if (this != &ref)
    {
        fd = ref.fd;
        host = ref.host;
        port = ref.port;
    }
    return *this;
}

void BaseSocket::setClose()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}

int BaseSocket::getFd() const
{
    return fd;
}

const std::string &BaseSocket::getHost() const
{
    return host;
}

int BaseSocket::getPort() const
{
    return port;
}

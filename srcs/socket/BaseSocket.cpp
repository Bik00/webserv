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

void BaseSocket::setNonBlocking()
{
    if (fd < 0)
    {
        throw std::runtime_error("Cannot set non-blocking on invalid socket");
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        throw std::runtime_error("Failed to get socket flags");
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw std::runtime_error("Failed to set socket non-blocking");
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

#include "../../includes/socket/ListenSocket.hpp"

ListenSocket::ListenSocket(void) : fd(-1), host("0.0.0.0"), port(80), bound(false)
{
}

ListenSocket::ListenSocket(const std::string &host, int port) 
    : fd(-1), host(host), port(port), bound(false)
{
}

ListenSocket::~ListenSocket(void)
{
    setClose();
}

ListenSocket::ListenSocket(const ListenSocket &ref)
    : fd(ref.fd), host(ref.host), port(ref.port), bound(ref.bound)
{
}

ListenSocket &ListenSocket::operator=(const ListenSocket &ref)
{
    if (this != &ref)
    {
        host = ref.host;
        port = ref.port;
        bound = ref.bound;
        fd = ref.fd;
    }
    return (*this);
}

void ListenSocket::setBind()
{
    if (bound)
        return;
    
    // Create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }
    
    // Set SO_REUSEADDR to allow quick restart
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(fd);
        fd = -1;
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }
    
    // Setup address structure
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Convert host string to address
    if (host == "0.0.0.0" || host.empty())
    {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
        {
            close(fd);
            fd = -1;
            std::ostringstream err;
            err << "Invalid host address: " << host;
            throw std::runtime_error(err.str());
        }
    }
    
    // Bind socket
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close(fd);
        fd = -1;
        std::ostringstream err;
        err << "Failed to bind to " << host << ":" << port;
        throw std::runtime_error(err.str());
    }
    
    bound = true;
}

void ListenSocket::setListen()
{
    if (!bound)
    {
        throw std::runtime_error("Cannot listen on unbound socket");
    }
    
    if (listen(fd, SOMAXCONN) < 0)
    {
        throw std::runtime_error("Failed to listen on socket");
    }
}

void ListenSocket::setNonBlocking()
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

void ListenSocket::setClose()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
        bound = false;
    }
}

int ListenSocket::getFd() const
{
    return fd;
}

const std::string &ListenSocket::getHost() const
{
    return host;
}

int ListenSocket::getPort() const
{
    return port;
}

bool ListenSocket::isBound() const
{
    return bound;
}

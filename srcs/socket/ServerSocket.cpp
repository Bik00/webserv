#include "../../includes/socket/ServerSocket.hpp"

ServerSocket::ServerSocket(void) : BaseSocket(), bound(false)
{
    host = std::string(DEFAULT_LISTEN_HOST);
    port = DEFAULT_LISTEN_PORT;
}

ServerSocket::ServerSocket(const std::string &h, int p) 
    : BaseSocket(h, p), bound(false)
{
}

ServerSocket::~ServerSocket(void)
{
    setClose();
}

ServerSocket::ServerSocket(const ServerSocket &ref)
    : BaseSocket(ref), bound(ref.bound)
{
}

ServerSocket &ServerSocket::operator=(const ServerSocket &ref)
{
    if (this != &ref)
    {
        BaseSocket::operator=(ref);
        bound = ref.bound;
    }
    return (*this);
}

void ServerSocket::setBind()
{
    if (bound)
        return;

    // Create socket (explicit TCP protocol)
    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
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

void ServerSocket::setListen()
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

void ServerSocket::setClose()
{
    BaseSocket::setClose();
    bound = false;
}

bool ServerSocket::isBound() const
{
    return bound;
}

int ServerSocket::getFd() const
{
    return BaseSocket::getFd();
}

const std::string &ServerSocket::getHost() const
{
    return BaseSocket::getHost();
}

int ServerSocket::getPort() const
{
    return BaseSocket::getPort();
}

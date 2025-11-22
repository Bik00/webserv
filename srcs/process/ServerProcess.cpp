#include "../../includes/process/ServerProcess.hpp"

// Global flag for signal handling
static volatile sig_atomic_t g_shutdown = 0;

// Signal handlers
static void sigterm_handler(int sig)
{
    (void)sig;
    g_shutdown = 1;
}

ServerProcess::ServerProcess(void) : epollFd(-1)
{
}

ServerProcess::~ServerProcess(void)
{
    // Clean up all client connections
    for (std::map<int, ClientSocket*>::iterator it = clients.begin(); 
         it != clients.end(); ++it)
    {
        delete it->second;
    }
    clients.clear();
    
    // Close epoll
    if (epollFd >= 0)
    {
        close(epollFd);
    }
    
    // Cleanup server sockets
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        serverSockets[i]->setClose();
        delete serverSockets[i];
    }
    serverSockets.clear();
}

ServerProcess::ServerProcess(const ServerProcess &ref) : epollFd(-1)
{
    (void)ref;
}

ServerProcess &ServerProcess::operator=(const ServerProcess &ref)
{
    (void)ref;
    return (*this);
}

void ServerProcess::Run(const Config &config)
{
    try
    {
        std::cout << "Server process starting..." << std::endl;
        
        // Step 1: Setup server sockets
        setupServerSockets(config);
        std::cout << "Listening on " << serverSockets.size() << " address(es)" << std::endl;
        
        // Step 2: Setup epoll
        setupEpoll();
        addListenSockets();
        
        // Step 3: Install signal handlers
        installSignalHandlers();
        
        // Step 4: Event loop
        std::cout << "Server entering event loop..." << std::endl;
        eventLoop();
        
        std::cout << "Server shutting down..." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        throw;
    }
}

void ServerProcess::setupServerSockets(const Config &config)
{
    const HttpBlock &http = config.getHttpBlock();
    const std::vector<ServerBlock> &servers = http.getServerBlocks();
    
    // Collect unique listen addresses
    std::set<std::string> uniqueListens;
    
    for (size_t si = 0; si < servers.size(); ++si)
    {
        const std::vector<ListenAddr> &listens = servers[si].getListenAddrs();
        
        for (size_t li = 0; li < listens.size(); ++li)
        {
            const ListenAddr &la = listens[li];
            std::ostringstream key;
            key << la.host << ":" << la.port;
            uniqueListens.insert(key.str());
        }
    }
    
    // Create, bind, and listen on each unique address
    for (std::set<std::string>::const_iterator it = uniqueListens.begin();
         it != uniqueListens.end(); ++it)
    {
        // Parse host:port from key
        std::string key = *it;
        size_t colonPos = key.find(':');
        if (colonPos == std::string::npos)
            continue;
        
        std::string host = key.substr(0, colonPos);
        std::string portStr = key.substr(colonPos + 1);
        int port = 0;
        std::istringstream(portStr) >> port;
        
        // Create and setup socket
        ServerSocket *sock = new ServerSocket(host, port);
        sock->setBind();
        sock->setListen();
        
        serverSockets.push_back(sock);
        
        std::cout << "Bound to " << host << ":" << port 
                  << " (fd=" << sock->getFd() << ")" << std::endl;
    }
    
    if (serverSockets.empty())
    {
        throw std::runtime_error("No server sockets created");
    }
}

void ServerProcess::setupEpoll()
{
    epollFd = epoll_create(1);
    if (epollFd < 0)
    {
        throw std::runtime_error("epoll_create() failed");
    }
    std::cout << "Epoll fd: " << epollFd << std::endl;
}

void ServerProcess::addListenSockets()
{
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;  // Edge-triggered for listen sockets
        ev.data.fd = serverSockets[i]->getFd();
        
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSockets[i]->getFd(), &ev) < 0)
        {
            std::ostringstream err;
            err << "epoll_ctl(ADD) failed for listen fd " << serverSockets[i]->getFd();
            throw std::runtime_error(err.str());
        }
        
        std::cout << "Added listen fd " << serverSockets[i]->getFd() << " to epoll" << std::endl;
    }
}

void ServerProcess::installSignalHandlers()
{
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
        throw std::runtime_error("Failed to install SIGTERM handler");

    if (signal(SIGINT, sigterm_handler) == SIG_ERR)
        throw std::runtime_error("Failed to install SIGINT handler");
}

void ServerProcess::eventLoop()
{
    struct epoll_event events[MAX_EVENTS];
    
    while (!g_shutdown)
    {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        
        if (nfds < 0)
        {
            if (errno == EINTR)
                continue;  // Interrupted by signal, retry
            throw std::runtime_error("epoll_wait() failed");
        }
        
        for (int i = 0; i < nfds; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t evs = events[i].events;
            
            if (isListenSocket(fd))
            {
                // Accept new connections
                if (evs & EPOLLIN)
                    handleListenEvent(fd);
            }
            else
            {
                // Client socket events
                if (evs & (EPOLLERR | EPOLLHUP))
                {
                    closeClient(fd);
                }
                else
                {
                    if (evs & EPOLLIN)
                        handleClientRead(fd);
                    if (evs & EPOLLOUT)
                        handleClientWrite(fd);
                }
            }
        }
    }
}

bool ServerProcess::isListenSocket(int fd) const
{
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        if (serverSockets[i]->getFd() == fd)
            return true;
    }
    return false;
}

void ServerProcess::handleListenEvent(int listenFd)
{
    // Edge-triggered: accept all pending connections
    while (true)
    {
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        
        int clientFd = accept4(listenFd, (struct sockaddr*)&clientAddr, 
                               &addrLen, SOCK_NONBLOCK);
        
        if (clientFd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No more connections to accept
                break;
            }
            else if (errno == EINTR)
            {
                continue;  // Interrupted, retry
            }
            else
            {
                std::cerr << "accept4() error: " << strerror(errno) << std::endl;
                break;
            }
        }
        
        // Create ClientSocket object
        ClientSocket *client = new ClientSocket(clientFd, clientAddr, addrLen);
        clients[clientFd] = client;
        
        // Add to epoll with EPOLLIN | EPOLLOUT | EPOLLET
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.fd = clientFd;
        
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0)
        {
            std::cerr << "epoll_ctl(ADD) failed for client: " << strerror(errno) << std::endl;
            closeClient(clientFd);
            continue;
        }
        
        std::cout << "Accepted client fd " << clientFd << std::endl;
    }
}

void ServerProcess::handleClientRead(int fd)
{
    ClientSocket *client = clients[fd];
    if (!client)
        return;
    
    char buf[4096];
    
    // Edge-triggered: read all available data
    while (true)
    {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        
        if (n > 0)
        {
            client->appendRecv(std::string(buf, n));
            client->touch();
        }
        else if (n == 0)
        {
            // Connection closed by client
            std::cout << "Client " << fd << " closed connection" << std::endl;
            closeClient(fd);
            break;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No more data available
                break;
            }
            else if (errno == EINTR)
            {
                continue;  // Interrupted, retry
            }
            else
            {
                std::cerr << "recv() error: " << strerror(errno) << std::endl;
                closeClient(fd);
                break;
            }
        }
    }
    
    // TODO: Parse HTTP request and prepare response
    // For now, just echo back a simple HTTP response
    if (!client->getRecvBuffer().empty())
    {
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/plain\r\n";
        response << "Content-Length: 13\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << "Hello, World!";
        
        client->getSendBuffer() = response.str();
    }
}

void ServerProcess::handleClientWrite(int fd)
{
    ClientSocket *client = clients[fd];
    if (!client)
        return;
    
    std::string &sendBuf = client->getSendBuffer();
    
    if (sendBuf.empty())
        return;
    
    // Edge-triggered: send all available data
    while (!sendBuf.empty())
    {
        ssize_t n = send(fd, sendBuf.c_str(), sendBuf.size(), MSG_NOSIGNAL);
        
        if (n > 0)
        {
            sendBuf.erase(0, n);
            client->touch();
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // Socket buffer full, will retry later
                break;
            }
            else if (errno == EINTR)
            {
                continue;  // Interrupted, retry
            }
            else
            {
                std::cerr << "send() error: " << strerror(errno) << std::endl;
                closeClient(fd);
                break;
            }
        }
    }
    
    // If send buffer is empty, close connection (for now, no keep-alive)
    if (sendBuf.empty())
    {
        std::cout << "Response sent, closing client " << fd << std::endl;
        closeClient(fd);
    }
}

void ServerProcess::closeClient(int fd)
{
    std::map<int, ClientSocket*>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;
    
    // Remove from epoll
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    
    // Delete ClientSocket (will close fd)
    delete it->second;
    clients.erase(it);
}

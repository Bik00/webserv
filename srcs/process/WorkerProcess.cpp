#include "../../includes/process/WorkerProcess.hpp"

#define MAX_EVENTS 64

WorkerProcess::WorkerProcess(void) : epollFd(-1)
{
}

WorkerProcess::~WorkerProcess(void)
{
    // Clean up all client connections
    for (std::map<int, ClientSocket*>::iterator it = clients.begin(); 
         it != clients.end(); ++it)
    {
        delete it->second;
    }
    clients.clear();
    
    if (epollFd >= 0)
    {
        close(epollFd);
    }
}

WorkerProcess::WorkerProcess(const WorkerProcess &ref) : epollFd(-1)
{
    (void)ref;
}

WorkerProcess &WorkerProcess::operator=(const WorkerProcess &ref)
{
    (void)ref;
    return (*this);
}

void WorkerProcess::Run(const std::vector<int> &fds)
{
    serverSocketFds = fds;
    
    std::cout << "[Worker " << getpid() << "] Starting with " 
              << serverSocketFds.size() << " listen socket(s)" << std::endl;
    
    try
    {
        setupEpoll();
        addListenSockets();
        eventLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Worker " << getpid() << "] Error: " 
                  << e.what() << std::endl;
        _exit(1);
    }
}

void WorkerProcess::setupEpoll()
{
    epollFd = epoll_create(1);
    if (epollFd < 0)
    {
        throw std::runtime_error("epoll_create() failed");
    }
    std::cout << "[Worker " << getpid() << "] Epoll fd: " << epollFd << std::endl;
}

void WorkerProcess::addListenSockets()
{
    for (size_t i = 0; i < serverSocketFds.size(); ++i)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;  // Edge-triggered for listen sockets
        ev.data.fd = serverSocketFds[i];
        
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocketFds[i], &ev) < 0)
        {
            std::ostringstream err;
            err << "epoll_ctl(ADD) failed for listen fd " << serverSocketFds[i];
            throw std::runtime_error(err.str());
        }
        
        std::cout << "[Worker " << getpid() << "] Added listen fd " 
                  << serverSocketFds[i] << " to epoll" << std::endl;
    }
}

void WorkerProcess::eventLoop()
{
    struct epoll_event events[MAX_EVENTS];
    
    std::cout << "[Worker " << getpid() << "] Entering event loop..." << std::endl;
    
    while (true)
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
            
            // Check if this is a listen socket
            bool isListenSocket = false;
            for (size_t j = 0; j < serverSocketFds.size(); ++j)
            {
                if (serverSocketFds[j] == fd)
                {
                    isListenSocket = true;
                    break;
                }
            }
            
            if (isListenSocket)
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

void WorkerProcess::handleListenEvent(int listenFd)
{
    // Edge-triggered: accept all pending connections
    while (true)
    {
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        
        // Use accept4 with SOCK_NONBLOCK to set non-blocking immediately
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
                std::cerr << "[Worker " << getpid() << "] accept4() error: " 
                          << strerror(errno) << std::endl;
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
            std::cerr << "[Worker " << getpid() << "] epoll_ctl(ADD) failed for client: "
                      << strerror(errno) << std::endl;
            closeClient(clientFd);
            continue;
        }
        
        std::cout << "[Worker " << getpid() << "] Accepted client fd " 
                  << clientFd << std::endl;
    }
}

void WorkerProcess::handleClientRead(int fd)
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
            std::cout << "[Worker " << getpid() << "] Client " << fd 
                      << " closed connection" << std::endl;
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
                std::cerr << "[Worker " << getpid() << "] recv() error: " 
                          << strerror(errno) << std::endl;
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

void WorkerProcess::handleClientWrite(int fd)
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
                std::cerr << "[Worker " << getpid() << "] send() error: " 
                          << strerror(errno) << std::endl;
                closeClient(fd);
                break;
            }
        }
    }
    
    // If send buffer is empty, close connection (for now, no keep-alive)
    if (sendBuf.empty())
    {
        std::cout << "[Worker " << getpid() << "] Response sent, closing client " 
                  << fd << std::endl;
        closeClient(fd);
    }
}

void WorkerProcess::closeClient(int fd)
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

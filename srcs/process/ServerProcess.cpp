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
    
    // Collect unique listen addresses with associated servers
    std::map<std::string, std::vector<const ServerBlock*> > listenMap;
    
    for (size_t si = 0; si < servers.size(); ++si)
    {
        const std::vector<ListenAddr> &listens = servers[si].getListenAddrs();
        
        for (size_t li = 0; li < listens.size(); ++li)
        {
            const ListenAddr &la = listens[li];
            std::ostringstream key;
            key << la.host << ":" << la.port;
            listenMap[key.str()].push_back(&servers[si]);
        }
    }
    
    // Create, bind, and listen on each unique address
    for (std::map<std::string, std::vector<const ServerBlock*> >::const_iterator it = listenMap.begin();
         it != listenMap.end(); ++it)
    {
        // Parse host:port from key
        std::string key = it->first;
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
        
        // Add all servers listening on this socket
        const std::vector<const ServerBlock*> &sockServers = it->second;
        for (size_t i = 0; i < sockServers.size(); ++i)
        {
            sock->addServer(sockServers[i]);
        }
        
        serverSockets.push_back(sock);
        
        std::cout << "Bound to " << host << ":" << port 
                  << " (fd=" << sock->getFd() << ", " << sockServers.size() << " servers)" << std::endl;
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
        
        int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, 
                              &addrLen);
        
        if (clientFd < 0)
        {
            // No more connections to accept (EAGAIN/EWOULDBLOCK) or error
            break;
        }
        
        // Create ClientSocket object with listenFd reference
        ClientSocket *client = new ClientSocket(clientFd, clientAddr, addrLen, listenFd);
        clients[clientFd] = client;
        
        // Add to epoll with EPOLLIN | EPOLLOUT | EPOLLET
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.fd = clientFd;
        
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0)
        {
            std::cerr << "epoll_ctl(ADD) failed for client" << std::endl;
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
        ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
        
        if (n > 0)
        {
            client->appendRecv(std::string(buf, n));
            client->touch();
            
            // Parse HTTP request
            HttpTransaction &trans = client->getTransaction();
            trans.appendRequestData(std::string(buf, n));
            
            // Check if request is complete
            if (trans.isRequestComplete())
            {
                const HttpRequest &req = trans.getRequest();
                
                std::cout << "Received HTTP Request:" << std::endl;
                std::cout << "  Method: " << req.getMethod() << std::endl;
                std::cout << "  URI: " << req.getUri() << std::endl;
                std::cout << "  Path: " << req.getPath() << std::endl;
                std::cout << "  Query: " << req.getQuery() << std::endl;
                std::cout << "  Version: " << req.getHttpVersion() << std::endl;
                
                // Print headers
                const std::map<std::string, std::string> &headers = req.getHeaders();
                if (!headers.empty())
                {
                    std::cout << "  Headers:" << std::endl;
                    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
                         it != headers.end(); ++it)
                    {
                        std::cout << "    " << it->first << ": " << it->second << std::endl;
                    }
                }
                
                // Print body if present
                if (!req.getBody().empty())
                {
                    std::cout << "  Body (" << req.getBody().size() << " bytes): " 
                              << req.getBody() << std::endl;
                }
                else if (req.getContentLength() > 0)
                {
                    std::cout << "  Body expected (" << req.getContentLength() 
                              << " bytes) but empty! ParseState=" << req.getState() << std::endl;
                }
                
                // Check for parsing errors
                if (!req.isRequestValid())
                {
                    std::cerr << "  ERROR: " << req.getErrorMsg() << std::endl;
                    
                    // Build error response
                    HttpResponse &resp = trans.getResponse();
                    resp.setStatus(req.getErrorCode());
                    resp.setBody("<html><body><h1>" + resp.getReasonPhrase() + "</h1></body></html>");
                    resp.setContentType("text/html");
                    resp.setContentLength(resp.getBody().size());
                    resp.setConnection("close");
                    client->getSendBuffer() = resp.build();
                }
                else
                {
                    // Find matching server and location
                    const ServerBlock *server = findMatchingServer(client->getListenFd(), req);
                    const LocationBlock *location = findMatchingLocation(server, req.getPath());
                    
                    if (server)
                    {
                        std::cout << "  Matched Server: " << (server->getServerNames().empty() ? "(default)" : server->getServerNames()[0]) << std::endl;
                    }
                    if (location)
                    {
                        std::cout << "  Matched Location: " << location->getPath() << std::endl;
                        if (location->hasCgi())
                        {
                            std::cout << "  CGI Enabled: Yes" << std::endl;
                            const std::vector<std::string> &exts = location->getCgiExtensions();
                            std::cout << "  CGI Extensions: ";
                            for (size_t i = 0; i < exts.size(); ++i)
                            {
                                std::cout << exts[i];
                                if (i + 1 < exts.size())
                                    std::cout << ", ";
                            }
                            std::cout << std::endl;
                        }
                    }
                    
                    // Build response (will check for CGI internally)
                    trans.buildResponse(server, location);
                    client->getSendBuffer() = trans.getResponseData();
                }
                
                // Data parsed, break from read loop
                break;
            }
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
            // n < 0: No more data (EAGAIN/EWOULDBLOCK) or error
            break;
        }
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
        ssize_t n = send(fd, sendBuf.c_str(), sendBuf.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
        
        if (n > 0)
        {
            sendBuf.erase(0, n);
            client->touch();
        }
        else
        {
            // n <= 0: Socket buffer full (EAGAIN/EWOULDBLOCK) or error
            break;
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

// Helper: Find matching ServerBlock based on listen socket and Host header
const ServerBlock *ServerProcess::findMatchingServer(int listenFd, const HttpRequest &req) const
{
    // Find which ServerSocket this fd belongs to
    const ServerSocket *listenSock = NULL;
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        if (serverSockets[i]->getFd() == listenFd)
        {
            listenSock = serverSockets[i];
            break;
        }
    }
    
    if (!listenSock)
        return NULL;
    
    // Get Host header
    std::string host = req.getHeader("host");
    
    // Get all servers listening on this socket
    const std::vector<const ServerBlock*> &servers = listenSock->getServers();
    
    if (servers.empty())
        return NULL;
    
    // Try to match by server_name
    if (!host.empty())
    {
        // Strip port from host if present
        std::string hostname = host;
        size_t colonPos = hostname.find(':');
        if (colonPos != std::string::npos)
            hostname = hostname.substr(0, colonPos);
        
        for (size_t i = 0; i < servers.size(); ++i)
        {
            const std::vector<std::string> &serverNames = servers[i]->getServerNames();
            for (size_t j = 0; j < serverNames.size(); ++j)
            {
                if (serverNames[j] == hostname)
                    return servers[i];
            }
        }
    }
    
    // Return first (default) server if no match
    return servers[0];
}

// Helper: Find matching LocationBlock for request path
const LocationBlock *ServerProcess::findMatchingLocation(const ServerBlock *server, const std::string &path) const
{
    if (!server)
        return NULL;
    
    const std::vector<LocationBlock> &locations = server->getLocationBlocks();
    
    if (locations.empty())
        return NULL;
    
    // Find best matching location (longest prefix match)
    const LocationBlock *bestMatch = NULL;
    size_t bestMatchLen = 0;
    
    for (size_t i = 0; i < locations.size(); ++i)
    {
        const std::string &locPath = locations[i].getPath();
        
        // Check if path starts with location path
        if (path.find(locPath) == 0)
        {
            if (locPath.length() > bestMatchLen)
            {
                bestMatch = &locations[i];
                bestMatchLen = locPath.length();
            }
        }
    }
    
    // If no match, use first location as default
    return bestMatch ? bestMatch : &locations[0];
}

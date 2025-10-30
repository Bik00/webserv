#include "../includes/Header.hpp"

#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <sstream>

ServerManager::ServerManager(void)
{
}

// helper to read entire file into string
static bool loadFileToString(const std::string &path, std::string &out)
{
    std::ifstream ifs(path.c_str());
    if (!ifs) return false;
    std::ostringstream ss;
    ss << ifs.rdbuf();
    out = ss.str();
    return true;
}

ServerManager::~ServerManager(void)
{
    closeServers();
}

ServerManager::ServerManager(const ServerManager &ref)
{
    (void)ref;
}

ServerManager &ServerManager::operator=(const ServerManager &ref)
{
    (void)ref;
    return (*this);
}

bool ServerManager::createServer(const ServerConfig &cfg)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "socket() failed" << std::endl;
        return false;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(cfg.getPort());
    std::string host = cfg.getHost();
    if (host.empty() || host == "0.0.0.0") addr.sin_addr.s_addr = INADDR_ANY;
    else inet_aton(host.c_str(), &addr.sin_addr);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed for " << cfg.getHost() << ":" << cfg.getPort() << std::endl;
        close(fd);
        return false;
    }

    if (listen(fd, 10) < 0)
    {
        std::cerr << "listen() failed" << std::endl;
        close(fd);
        return false;
    }

    // set non-blocking for edge-safe accept loop
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    listen_fds.push_back(fd);
    servers.push_back(cfg);
    return true;
}

bool ServerManager::runServers(void)
{
    const int MAX_EVENTS = 64;
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        epfd = epoll_create(1024);
        if (epfd == -1)
        {
            std::cerr << "epoll_create failed: " << strerror(errno) << std::endl;
            return false;
        }
    }

    // add listening fds to epoll (edge-triggered)
    for (size_t i = 0; i < listen_fds.size(); ++i)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = listen_fds[i];
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fds[i], &ev) == -1)
        {
            std::cerr << "epoll_ctl ADD listen fd failed: " << strerror(errno) << std::endl;
            // continue trying others
        }
    }

    std::cout << "runServers: " << listen_fds.size() << " listening sockets" << std::endl;

    struct epoll_event events[MAX_EVENTS];
    for (;;)
    {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            if (errno == EINTR) continue;
            std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
            break;
        }

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;

            // check if this fd is a listening socket
            bool is_listen = (std::find(listen_fds.begin(), listen_fds.end(), fd) != listen_fds.end());

            if (is_listen)
            {
                // accept loop for non-blocking listening socket (drain all pending accepts)
                for (;;)
                {
                    struct sockaddr_in cli_addr;
                    socklen_t cli_len = sizeof(cli_addr);
                    int client_fd = accept(fd, (struct sockaddr*)&cli_addr, &cli_len);
                    if (client_fd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break; // no more
                        std::cerr << "accept() failed: " << strerror(errno) << std::endl;
                        break;
                    }

                    // make client non-blocking
                    int cflags = fcntl(client_fd, F_GETFL, 0);
                    if (cflags != -1)
                        fcntl(client_fd, F_SETFL, cflags | O_NONBLOCK);

                    // add client to epoll (edge-triggered)
                    struct epoll_event cev;
                    cev.events = EPOLLIN | EPOLLET;
                    cev.data.fd = client_fd;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &cev) == -1)
                    {
                        std::cerr << "epoll_ctl ADD client failed: " << strerror(errno) << std::endl;
                        close(client_fd);
                    }
                }
            }
            else
            {
                // client socket ready for read or error
                if (ev & (EPOLLHUP | EPOLLERR))
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }

                if (ev & EPOLLIN)
                {
                    // drain all available data (edge-triggered)
                    std::string request;
                    for (;;)
                    {
                        char buf[4096];
                        ssize_t r = recv(fd, buf, sizeof(buf), 0);
                        if (r > 0)
                        {
                            request.append(buf, buf + r);
                            continue;
                        }
                        else if (r == 0)
                        {
                            // EOF
                            break;
                        }
                        else
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                            {
                                // all data read
                                break;
                            }
                            // error
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                            close(fd);
                            goto next_event;
                        }
                    }

                    // determine requested path from request start-line
                    std::string req_line;
                    std::istringstream rs(request);
                    if (!std::getline(rs, req_line)) req_line = "";
                    // expected: METHOD SP PATH SP HTTP/1.1
                    std::string method, path, proto;
                    std::istringstream ls(req_line);
                    ls >> method >> path >> proto;

                    std::string body;
                    int status = 200;
                    // only serve index for root or explicit index path
                    if (path == "/" || path == "/index.html")
                    {
                        if (!loadFileToString("./www/index.html", body))
                        {
                            status = 404;
                            if (!loadFileToString("./www/errors/404.html", body)) body = "404 Not Found";
                        }
                        else status = 200;
                    }
                    else
                    {
                        status = 404;
                        if (!loadFileToString("./www/errors/404.html", body)) body = "404 Not Found";
                    }

                    std::ostringstream header;
                    if (status == 200) header << "HTTP/1.1 200 OK\r\n";
                    else header << "HTTP/1.1 404 Not Found\r\n";
                    header << "Content-Length: " << body.size() << "\r\n";
                    header << "Content-Type: text/html\r\n";
                    header << "Connection: close\r\n";
                    header << "\r\n";

                    std::string resp = header.str() + body;
                    ssize_t total = 0;
                    ssize_t len = resp.size();
                    while (total < len)
                    {
                        ssize_t s = send(fd, resp.c_str() + total, len - total, 0);
                        if (s <= 0)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                            break;
                        }
                        total += s;
                    }

                    // close connection after response
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
            next_event: ;
        }
    }

    close(epfd);
    return true;
}

void ServerManager::closeServers(void)
{
    for (size_t i = 0; i < listen_fds.size(); ++i)
    {
        close(listen_fds[i]);
    }
    listen_fds.clear();
    servers.clear();
}
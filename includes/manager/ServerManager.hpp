#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP
# include <vector>
# include <map>
# include <fstream>
# include <cstdlib>
# include <cctype>
# include "../config/ServerConfig.hpp"
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>

class ServerManager
{
public:
    ServerManager(void);
    ~ServerManager(void);
    ServerManager(const ServerManager &ref);
    ServerManager &operator=(const ServerManager &ref);
    
    // create a listening socket for the provided server configuration
    // returns true on success
    bool createServer(const ServerConfig &cfg);

    // start running servers (non-blocking placeholder)
    bool runServers(void);

    // close all opened listening sockets
    void closeServers(void);

private:
    std::vector<int> listen_fds;
    std::vector<ServerConfig> servers;

};

#endif /* SERVER_MANAGER_HPP */
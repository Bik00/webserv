#ifndef SERVER_PROCESS_HPP
# define SERVER_PROCESS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../socket/ServerSocket.hpp"
# include "../socket/ClientSocket.hpp"

# define MAX_EVENTS 64

class ServerProcess
{
private:
    std::vector<ServerSocket*> serverSockets;
    int epollFd;
    std::map<int, ClientSocket*> clients;
    
    // Setup
    void setupServerSockets(const Config &config);
    void setupEpoll();
    void addListenSockets();
    
    // Signal handling
    void installSignalHandlers();
    
    // Event loop
    void eventLoop();
    void handleListenEvent(int fd);
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
    void closeClient(int fd);
    bool isListenSocket(int fd) const;
    
    // Helper functions for request routing
    const ServerBlock *findMatchingServer(int listenFd, const HttpRequest &req) const;
    const LocationBlock *findMatchingLocation(const ServerBlock *server, const std::string &path) const;
    
public:
    ServerProcess(void);
    ~ServerProcess(void);
    ServerProcess(const ServerProcess &ref);
    ServerProcess &operator=(const ServerProcess &ref);
    void Run(const Config &config);
};

#endif /* SERVER_PROCESS_HPP */

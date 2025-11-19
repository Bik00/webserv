#ifndef LISTEN_SOCKET_HPP
# define LISTEN_SOCKET_HPP

# include "../libs/Libs.hpp"

class ListenSocket
{
private:
    int fd;
    std::string host;
    int port;
    bool bound;
    
public:
    ListenSocket(void);
    ListenSocket(const std::string &host, int port);
    ~ListenSocket(void);
    ListenSocket(const ListenSocket &ref);
    ListenSocket &operator=(const ListenSocket &ref);
    
    // Socket operations
    void bindSocket();
    void listenSocket(int backlog = 128);
    void closeSocket();
    void setNonBlocking();

    // Getters
    int getFd() const;
    const std::string &getHost() const;
    int getPort() const;
    bool isBound() const;
};

#endif /* LISTEN_SOCKET_HPP */

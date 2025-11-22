#ifndef SERVER_SOCKET_HPP
# define SERVER_SOCKET_HPP

# include "BaseSocket.hpp"

class ServerSocket : public BaseSocket
{
private:
    bool bound;

public:
    ServerSocket(void);
    ServerSocket(const std::string &host, int port);
    ~ServerSocket(void);
    ServerSocket(const ServerSocket &ref);
    ServerSocket &operator=(const ServerSocket &ref);

    // Socket operations
    void setBind();
    void setListen();
    void setClose();

    // Getters
    int getFd() const;
    const std::string &getHost() const;
    int getPort() const;
    bool isBound() const;
};

#endif /* SERVER_SOCKET_HPP */

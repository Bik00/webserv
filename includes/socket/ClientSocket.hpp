#ifndef CLIENT_SOCKET_HPP
# define CLIENT_SOCKET_HPP

# include "BaseSocket.hpp"
# include "../http/HttpTransaction.hpp"

class ClientSocket : public BaseSocket
{
private:
    struct sockaddr_in addr;
    socklen_t addrLen;
    std::string recvBuffer;
    std::string sendBuffer;
    time_t lastActivity;
    bool closed;
    HttpTransaction transaction;
    int listenFd;  // The server socket that accepted this client

public:
    ClientSocket(void);
    ClientSocket(int fd, const struct sockaddr_in &addr, socklen_t addrLen, int listenFd = -1);
    ~ClientSocket(void);
    ClientSocket(const ClientSocket &ref);
    ClientSocket &operator=(const ClientSocket &ref);

    void setClose();

    // Buffers
    void appendRecv(const std::string &data);
    const std::string &getRecvBuffer() const;
    std::string &getSendBuffer();

    // Activity
    void touch();
    time_t getLastActivity() const;

    // Address
    const struct sockaddr_in &getAddr() const;
    socklen_t getAddrLen() const;

    bool isClosed() const;
    
    // HTTP Transaction
    HttpTransaction &getTransaction();
    const HttpTransaction &getTransaction() const;
    
    // Server socket reference
    int getListenFd() const;
    void setListenFd(int fd);
};

#endif /* CLIENT_SOCKET_HPP */

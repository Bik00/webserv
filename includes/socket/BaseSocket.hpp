#ifndef BASE_SOCKET_HPP
# define BASE_SOCKET_HPP

# include "../libs/Libs.hpp"

class BaseSocket
{
protected:
    int fd;
    std::string host;
    int port;

public:
    BaseSocket(void);
    BaseSocket(const std::string &host, int port);
    virtual ~BaseSocket(void);
    BaseSocket(const BaseSocket &ref);
    BaseSocket &operator=(const BaseSocket &ref);

    /* Generic socket operations */
    virtual void setClose();
    void setNonBlocking();

    /* Getters */
    int getFd() const;
    const std::string &getHost() const;
    int getPort() const;
};

#endif /* BASE_SOCKET_HPP */

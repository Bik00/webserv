#ifndef LISTEN_SOCKET_HPP
# define LISTEN_SOCKET_HPP

# include "../libs/Libs.hpp"

class ListenSocket
{
public:
    ListenSocket(void);
    ~ListenSocket(void);
    ListenSocket(const ListenSocket &ref);
    ListenSocket &operator=(const ListenSocket &ref);
};

#endif /* LISTEN_SOCKET_HPP */

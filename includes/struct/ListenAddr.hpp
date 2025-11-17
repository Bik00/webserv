// central ListenAddr definition for reuse
#ifndef LISTEN_ADDR_HPP
# define LISTEN_ADDR_HPP

# include "../libs/Libs.hpp"

struct ListenAddr {
    std::string host; // empty -> use DEFAULT_LISTEN_HOST
    int port; // 0 -> use DEFAULT_LISTEN_PORT
    bool defaultServerFlag;
};

#endif // LISTEN_ADDR_HPP

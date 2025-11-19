#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../../includes/parse/ConfigParser.hpp"
# include "../../includes/master/MasterProcess.hpp"

class ServerManager
{
private:
    Config config;

public:
    ServerManager(void);
    ~ServerManager(void);
    ServerManager(const ServerManager &ref);
    ServerManager &operator=(const ServerManager &ref);

    void    Run(int argc, char **argv);
    int     exitServer(const std::string &message);
};

#endif /* SERVER_MANAGER_HPP */
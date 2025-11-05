#include "../../includes/manager/ServerManager.hpp"

ServerManager::ServerManager(void)
{
}

ServerManager::~ServerManager(void)
{
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

int ServerManager::run(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return (EXIT_SUCCESS);
}

int ServerManager::exitServer(const std::string &msg)
{
    std::cerr << msg << std::endl;
    closeServers();
    return (EXIT_FAILURE);
}
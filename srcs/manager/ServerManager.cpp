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

void ServerManager::Run(int argc, char **argv)
{
    ConfigParser parser;
    ServerProcess serverProcess;

    if (parser.Parse(argc, argv, this->config) == false)
        throw std::runtime_error("Configuration parsing failed");
    serverProcess.Run(this->config);
}

int ServerManager::exitServer(const std::string &message)
{
    if (!message.empty())
        std::cerr << "Error: " << message << std::endl;
    return (EXIT_FAILURE);
}
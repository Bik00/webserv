#include "../../includes/manager/ServerManager.hpp"
#include "../../includes/parse/ConfigParser.hpp"

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

    if (parser.Parse(argc, argv, this->config) == false)
        throw new std::runtime_error("Configuration parsing failed");
}

int ServerManager::exitServer(const std::string &message)
{
    if (!message.empty())
        std::cerr << "Error: " << message << std::endl;
    return (EXIT_FAILURE);
}
#include "../includes/Header.hpp"

int main(int argc, char **argv)
{
    HttpConfig *config;
    ConfigParser parser;
    std::string configPath;

    configPath = (argc == 2) ? argv[1] : "./config/default.conf";
    try
    {
        if (!parser.parseConfigFile(&config, configPath))
        {
            throw std::runtime_error(std::string("Could not parse config file: ") + argv[1]);
        }

        config->getInfo();

        ServerManager manager;
        size_t count = config->getServerCount();
        for (size_t i = 0; i < count; ++i)
        {
            const ServerConfig &srv = config->getServer(i);
            if (!manager.createServer(srv))
            {
                std::cerr << "Failed to create server for " << srv.getHost() << ":" << srv.getPort() << std::endl;
            }
        }

        manager.runServers();

        // in a real server we'd block here; for now close and exit
        manager.closeServers();

        delete config;
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
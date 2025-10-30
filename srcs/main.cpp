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
        delete config;
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
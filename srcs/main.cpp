#include "../includes/Header.hpp"

int main(int argc, char **argv)
{
    HttpConfig *config;
    ConfigParser parser;
    try
    {
        if (argc != 2)
        {
            throw std::runtime_error("write './webserv <config_file>'.");
        }

        if (!parser.parseConfigFile(&config, argv[1]))
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
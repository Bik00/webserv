#include "../includes/Header.hpp"

int main(int argc, char **argv)
{
    HttpConfig *config;
    try
    {
        if (argc != 2)
        {
            throw std::runtime_error("write './webserv <config_file>'.");
        }
        else
        {
            config = new HttpConfig();

            config->loadFromFile(argv[1]);
            config->getInfo();
            delete config;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
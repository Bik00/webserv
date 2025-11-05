#include "../includes/main.hpp"

int main(int argc, char **argv)
{
    int             ret = EXIT_SUCCESS;
    ServerManager   manager;

    try
    {
        manager.Run(argc, argv);
    }
    catch(const std::exception &e)
    {
        ret = manager.exitServer(e.what());
    }

    return (ret);
}
#include "../includes/main.hpp"

int main(int argc, char **argv)
{
    ServerManager manager;

    try
    {
        ret = manager.run(argc, argv);
    }
    catch(const std::exception &e)
    {
        ret = manager.exitServer("Error: " + e.what());
    }

    return (ret);
}
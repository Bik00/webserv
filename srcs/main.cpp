#include "../includes/Header.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    HttpConfig *config = new HttpConfig(80, "localhost");
    config->getInfo();

    delete config;

    return 0;
}
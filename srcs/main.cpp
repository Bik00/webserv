#include "../includes/Header.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    HttpConfig *config = new HttpConfig();
    config->getInfo();

    delete config;

    return 0;
}
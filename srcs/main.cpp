#include "../includes/header.hpp"

int main(int argc, char **argv)
{
    HttpConfig config;

    config = new HttpConfig(80, "localhost");
    config.getInfo();

    delete config;

    return 0;
}
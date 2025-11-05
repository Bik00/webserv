#include "../../includes/config/Config.hpp"

Config::Config(void)
{
}

Config::~Config(void)
{
}

Config::Config(const Config &ref)
{
    *this = ref;
}

Config &Config::operator=(const Config &ref)
{
    if (this != &ref)
    {
    this->simpleDirectives = ref.simpleDirectives;
    this->httpBlocks = ref.httpBlocks;
    this->eventBlocks = ref.eventBlocks;
    }
    return *this;
}


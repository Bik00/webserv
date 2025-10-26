#include "../../includes/config/LocationConfig.hpp"

LocationConfig::LocationConfig(void)
{
}

LocationConfig::~LocationConfig(void)
{
}

LocationConfig::LocationConfig(const LocationConfig &ref)
{
    *this = ref;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &ref)
{
    if (this != &ref)
    {
        // copy members if any
    }
    return *this;
}
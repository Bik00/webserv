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
        this->configPath = ref.configPath;
    }
    return *this;
}

void Config::setConfigPath(const std::string &path)
{
    this->configPath = path;
}

const std::string &Config::getConfigPath() const
{
    return this->configPath;
}


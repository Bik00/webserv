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
    this->httpBlocks = ref.httpBlocks;
    this->eventBlocks = ref.eventBlocks;
    this->configPath = ref.configPath;
    this->workerProcesses = ref.workerProcesses;
    }
    return *this;
}

void Config::setConfigPath(const std::string &path)
{
    this->configPath = path;
}

void Config::setWorkerProcesses(int num)
{
    this->workerProcesses = num;
}

const std::string &Config::getConfigPath() const
{
    return this->configPath;
}

int Config::getWorkerProcesses() const
{
    return this->workerProcesses;
}

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
    this->httpBlock = ref.httpBlock;
    this->eventBlock = ref.eventBlock;
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

void Config::addEventBlock(const EventBlock &eventBlock)
{
    this->eventBlock = eventBlock;
}

const std::string &Config::getConfigPath() const
{
    return this->configPath;
}

int Config::getWorkerProcesses() const
{
    return this->workerProcesses;
}

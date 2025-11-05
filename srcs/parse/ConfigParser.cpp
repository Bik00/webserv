#include "../../includes/parse/ConfigParser.hpp"

ConfigParser::ConfigParser(void)
{
}

ConfigParser::~ConfigParser(void)
{
}

ConfigParser::ConfigParser(const ConfigParser &ref)
{
    (void)ref;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &ref)
{
    (void)ref;
    return (*this);
}

bool ConfigParser::Parse(int argc, char **argv, Config &config)
{
    bool ret = true;

    ret = generalParse(argc, argv);
    if (ret == false)
        return (false);
    ret = blockParse(config);
    if (ret == false)
        return (false);
    return (true);
}

bool ConfigParser::generalParse(int argc, char **argv)
{
    GeneralParseUtils   utils;
    bool                ret = true;
    std::string         configPath;

    configPath = utils.CheckArgc(argc, argv);
    if (configPath.empty())
        ret = false;
    if (ret)
    {
        ret = utils.ValidatePath(configPath);
    }

    return ret;
}

bool ConfigParser::blockParse(Config &config)
{
    (void)config;
    return (true);
}
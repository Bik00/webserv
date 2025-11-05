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

    ret = validate(argc, argv);
    if (ret)
        ret = generalParse(argc, argv, config);
    if (ret)
        ret = blockParse(argc, argv, config);
    return (true);
}

bool ConfigParser::validate(int argc, char **argv)
{
    bool ret = true;

    ret = validateGeneral(argc, argv);
    if (ret)
        ret = validateBlocks(argc, argv);
    return (ret);
}

bool ConfigParser::validateGeneral(int argc, char **argv)
{
    GeneralParseUtils   generalUtils;
    BlockParseUtils     blockUtils;
    bool                ret = true;
    std::string         configPath;

    configPath = generalUtils.CheckArgc(argc, argv);
    if (configPath.empty())
        ret = false;
    if (ret)
        ret = generalUtils.ValidatePath(configPath);
    if (ret)
        ret = generalUtils.CheckSimpleDirectives(configPath);

    return ret;
}

bool ConfigParser::validateBlocks(int argc, char **argv)
{
    (void)config;
    return (true);
}

bool ConfigParser::setGeneral(int argc, char **argv, Config &config)
{
    (void)argc;
    (void)argv;
    (void)config;
    return (true);
}

bool ConfigParser::setBlocks(int argc, char **argv, Config &config)
{
    (void)argc;
    (void)argv;
    (void)config;
    return (true);
}
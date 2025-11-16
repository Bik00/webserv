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

    ret = validate(argc, argv, config);
    if (ret)
        ret = setGeneral(argv, config);
    if (ret)
        ret = setBlocks(argv, config);
    return (ret);
}

bool ConfigParser::validate(int argc, char **argv, Config &config)
{
    bool ret = true;

    ret = validateGeneral(argc, argv, config);
    if (ret)
        ret = validateBlocks(argv, config);
    return (ret);
}

bool ConfigParser::validateGeneral(int argc, char **argv, Config &config)
{
    GeneralParseUtils   generalUtils;
    bool                ret = true;

    std::string path = generalUtils.CheckArgc(argc, argv);
    config.setConfigPath(path);
    if (ret)
        ret = generalUtils.ValidatePath(path);
    if (ret)
        ret = generalUtils.CheckSimpleDirectives(path);

    return ret;
}

bool ConfigParser::validateBlocks(char **argv, Config &config)
{
    bool            ret = true;
    BlockParseUtils blockUtils;

    ret = blockUtils.CheckEventBlock(argv, config);
    if (ret)
        ret = blockUtils.CheckHttpBlock(argv, config);

    return ret;
}

bool ConfigParser::setGeneral(char **argv, Config &config)
{
    (void)argv;
    (void)config;
    return (true);
}

bool ConfigParser::setBlocks(char **argv, Config &config)
{
    (void)argv;
    (void)config;
    return (true);
}
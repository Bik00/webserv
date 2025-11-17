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
        ret = setValue(argv, config);
    return (ret);
}

bool ConfigParser::validate(int argc, char **argv, Config &config)
{
    ConfigParserUtils utils;
    bool ret = true;

    ret = utils.ValidatePath(utils.CheckArgc(argc, argv));
    if (ret)
    {
        // ...
    }    
    return (ret);
}

bool ConfigParser::setValue(char **argv, Config &config)
{
    bool ret = true;

    return (ret);
}
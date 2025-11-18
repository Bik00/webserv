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
    ConfigValidatorUtils    validatorUtils;
    ConfigSetterUtils       setterUtils;
    bool ret = true;

    ret = validatorUtils.Validate(argc, argv, config);
    if (ret)
        ret = setterUtils.SetValue(argv, config);
    if (ret)
        config.printConfig();
    return (ret);
}
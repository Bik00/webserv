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

    std::string path = utils.CheckArgc(argc, argv);
    config.setConfigPath(path);
    ret = utils.ValidatePath(path);
    if (!ret) return false;

    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << path << std::endl;
        return false;
    }

    return utils.validateContext(ifs, "global");
}

bool ConfigParser::setValue(char **argv, Config &config)
{
    (void)argv;
    (void)config;
    bool ret = true;

    return (ret);
}
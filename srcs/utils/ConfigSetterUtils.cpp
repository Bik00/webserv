#include "../../includes/utils/ConfigSetterUtils.hpp"
#include "../../includes/utils/GeneralParseUtils.hpp"

ConfigSetterUtils::ConfigSetterUtils(void)
{
}

ConfigSetterUtils::~ConfigSetterUtils(void)
{
}

ConfigSetterUtils::ConfigSetterUtils(const ConfigSetterUtils &ref)
{
	(void)ref;
}

ConfigSetterUtils &ConfigSetterUtils::operator=(const ConfigSetterUtils &ref)
{
	(void)ref;
	return *this;
}

bool ConfigSetterUtils::setGlobalValue(std::istream &is, Config &config)
{
    GeneralParseUtils gparse;
    std::string line;
    size_t lineno = 0;
    int workers = 1;
    bool ret = true;

    while (std::getline(is, line))
    {
        ++lineno;
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;

        // block header: ends with '{'
        std::string blockName;
        if (gparse.ParseBlockHeader(s, blockName))
        {
            if (blockName == "event")
            {
                ret = setEventBlock(is, config);
            }
            else if (blockName == "http")
            {
                ret = setHttpBlock(is, config);
            }
            else
            {
                ret = false;
            }
            if (!ret) break;
            continue;
        }

        if (s == "{" )
        {
            std::cerr << "Unexpected '{' alone at global:" << lineno << std::endl;
            ret = false;
            break;
        }

        if (s == "}")
        {
            // end of a higher-level block; in global context treat as error
            std::cerr << "Unexpected '}' at global:" << lineno << std::endl;
            ret = false;
            break;
        }

        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue; // not a directive
        if (key == "worker_processes")
        {
            int v = 0;
            if (!gparse.ParsePositiveInt(val, v))
            {
                std::cerr << "Invalid worker_processes value at global:" << lineno << std::endl;
                ret = false;
                break;
            }
            workers = v;
        }
    }

    if (ret)
        config.setWorkerProcesses(workers);
    return ret;
}

bool ConfigSetterUtils::setEventBlock(std::istream &is, Config &config)
{
    GeneralParseUtils gparse;
    std::string body;

    if (!gparse.ReadBlockBody(is, body))
    {
        std::cerr << "Unbalanced braces in event block" << std::endl;
        return false;
    }
    
    std::istringstream inner(body);
    std::string line;
    size_t lineno = 0;
    EventBlock eb;
    while (std::getline(inner, line))
    {
        ++lineno;
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;
        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue;
        if (key == "worker_connections")
        {
            int v = 0;
            if (!gparse.ParsePositiveInt(val, v))
            {
                std::cerr << "Invalid worker_connections value in event block at line " << lineno << std::endl;
                return false;
            }
            eb.setWorkerConnections(v);
        }
    }
    config.addEventBlock(eb);
    return true;
}

bool ConfigSetterUtils::setHttpBlock(std::istream &is, Config &config)
{
    (void)config;
    // consume block body until matching '}' using brace balance
    char c;
    int brace_balance = 1;
    while (is.get(c))
    {
        if (c == '{') ++brace_balance;
        else if (c == '}')
        {
            --brace_balance;
            if (brace_balance == 0) break;
        }
    }
    if (brace_balance != 0)
    {
        std::cerr << "Unbalanced braces in http block" << std::endl;
        return false;
    }
    return true;
}

bool ConfigSetterUtils::SetValue(char **argv, Config &config)
{
    (void)argv;
    const std::string path = config.getConfigPath();
    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << path << std::endl;
        return false;
    }

    bool ret = setGlobalValue(ifs, config);
    return ret;
}


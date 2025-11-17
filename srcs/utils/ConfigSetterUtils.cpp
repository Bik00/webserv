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
                if (!setEventBlock(is, config)) return false;
            }
            else if (blockName == "http")
            {
                if (!setHttpBlock(is, config)) return false;
            }
            else
            {
                if (!setHttpBlock(is, config)) return false;
            }
            continue;
        }

        if (s == "{" )
        {
            std::cerr << "Unexpected '{' alone at global:" << lineno << std::endl;
            return false;
        }

        if (s == "}")
        {
            // end of a higher-level block; in global context treat as error
            std::cerr << "Unexpected '}' at global:" << lineno << std::endl;
            return false;
        }

        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue; // not a directive
        if (key == "worker_processes")
        {
            int v = 0;
            {
                std::istringstream iss(val);
                if (!(iss >> v))
                {
                    std::cerr << "Invalid worker_processes value at global:" << lineno << std::endl;
                    return false;
                }
            }
            if (v <= 0)
            {
                std::cerr << "worker_processes must be > 0 at global:" << lineno << std::endl;
                return false;
            }
            workers = v;
        }
    }

    config.setWorkerProcesses(workers);
    return true;
}

bool ConfigSetterUtils::setEventBlock(std::istream &is, Config &config)
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
        std::cerr << "Unbalanced braces in event block" << std::endl;
        return false;
    }
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


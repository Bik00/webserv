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
        if (s.size() >= 1 && s[s.size() - 1] == '{')
        {
            std::string header = s.substr(0, s.size() - 1);
            // trim header
            size_t h1 = 0;
            while (h1 < header.size() && isspace(static_cast<unsigned char>(header[h1]))) ++h1;
            size_t h2 = header.size();
            while (h2 > h1 && isspace(static_cast<unsigned char>(header[h2-1]))) --h2;
            if (h1 >= h2)
            {
                std::cerr << "Empty block header before '{' at global:" << lineno << std::endl;
                return false;
            }
            size_t p = h1;
            while (p < h2 && !isspace(static_cast<unsigned char>(header[p]))) ++p;
            std::string blockName = header.substr(h1, p - h1);
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
                // unknown block: delegate to http setter as fallback or skip
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

        // directive: strip trailing ';' if present
        if (!s.empty() && s[s.size() - 1] == ';') s.erase(s.size() - 1);

        // split key and value
        size_t i = 0;
        while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t j = s.size();
        while (j > i && isspace(static_cast<unsigned char>(s[j-1]))) --j;
        if (i >= j) continue;
        size_t sp = i;
        while (sp < j && !isspace(static_cast<unsigned char>(s[sp]))) ++sp;
        std::string key = s.substr(i, sp - i);
        size_t k = sp;
        while (k < j && isspace(static_cast<unsigned char>(s[k]))) ++k;
        std::string val = (k < j) ? s.substr(k, j - k) : std::string();

        if (key == "worker_processes")
        {
            int v = 0;
            try { v = std::stoi(val); } catch (...) {
                std::cerr << "Invalid worker_processes value at global:" << lineno << std::endl;
                return false;
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


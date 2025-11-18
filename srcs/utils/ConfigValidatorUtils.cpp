#include "../../includes/utils/ConfigValidatorUtils.hpp"
#include "../../includes/utils/GeneralParseUtils.hpp"

ConfigValidatorUtils::ConfigValidatorUtils(void)
{
}

ConfigValidatorUtils::~ConfigValidatorUtils(void)
{
}

ConfigValidatorUtils::ConfigValidatorUtils(const ConfigValidatorUtils &ref)
{
    (void)ref;
}

ConfigValidatorUtils &ConfigValidatorUtils::operator=(const ConfigValidatorUtils &ref)
{
    (void)ref;
    return *this;
}

bool ConfigValidatorUtils::Validate(int argc, char **argv, Config &config)
{
    std::string path = checkArgc(argc, argv);
    config.setConfigPath(path);
    if (!validatePath(path)) return false;
    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << path << std::endl;
        return false;
    }
    return validateContext(ifs, "global");
}

std::string ConfigValidatorUtils::checkArgc(int argc, char **argv)
{
    std::string configPath;

    if (argc > 2)
    {
        throw std::runtime_error("Too many arguments. Usage: ./webserv <config_file>");
    }
    else if (argc == 1)
    {
        configPath = DEFAULT_CONFIG_PATH;
    }
    else
    {
        configPath = argv[1];
    }

    return (configPath);
}

bool ConfigValidatorUtils::validatePath(const std::string &configPath)
{
    // copy from previous implementation
    std::vector<std::string> comps;
    size_t start = 0;
    bool absolute = (!configPath.empty() && configPath[0] == '/');
    if (absolute) start = 1;
    size_t pos = start;
    while (pos <= configPath.size())
    {
        size_t slash = configPath.find('/', pos);
        if (slash == std::string::npos) slash = configPath.size();
        std::string token = configPath.substr(pos, slash - pos);
        if (!token.empty()) comps.push_back(token);
        pos = slash + 1;
    }

    if (comps.empty())
    {
        std::cerr << "Invalid config path: " << configPath << std::endl;
        return false;
    }

    std::string cur;
    if (absolute) cur = "/";
    for (size_t i = 0; i + 1 < comps.size(); ++i)
    {
        if (cur == "/") cur += comps[i];
        else if (cur.empty()) cur = comps[i];
        else cur += std::string("/") + comps[i];

        struct stat st;
        if (stat(cur.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
        {
            std::cerr << "Directory not found: " << cur << std::endl;
            return false;
        }
    }

    std::string filePath;
    if (absolute) filePath = "/" + comps[0];
    else filePath = comps[0];
    for (size_t i = 1; i < comps.size(); ++i) filePath += std::string("/") + comps[i];

    std::string last = comps.back();
    if (last.size() < 5 || last.substr(last.size() - 5) != ".conf")
    {
        std::cerr << "Config file must end with .conf: " << last << std::endl;
        return false;
    }

    struct stat fst;
    if (stat(filePath.c_str(), &fst) != 0 || !S_ISREG(fst.st_mode))
    {
        std::cerr << "Config file not found: " << filePath << std::endl;
        return false;
    }
    return true;
}

// recursive validators moved here
bool ConfigValidatorUtils::validateContext(std::istream &is, const std::string &contextName)
{
    std::string line;
    size_t lineno = 0;
    std::set<std::string> seenKeys;
    // keys allowed to appear multiple times within the same block
    std::set<std::string> multiKeys;
    multiKeys.insert(std::string("index"));
    multiKeys.insert(std::string("error_page"));
    multiKeys.insert(std::string("listen"));
    multiKeys.insert(std::string("server_name"));
    GeneralParseUtils gparse;

    while (std::getline(is, line))
    {
        ++lineno;
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;

        std::string blockName;
        if (gparse.ParseBlockHeader(s, blockName))
        {
            if (!validateBlock(is, blockName))
                return false;
            continue;
        }

        if (s == "{")
        {
            std::cerr << "Unexpected '{' alone at " << contextName << ":" << lineno << std::endl;
            return false;
        }

        if (s == "}")
        {
            return true;
        }

        std::string key, value;
        if (!gparse.ParseDirective(s, key, value))
        {
            std::cerr << "Invalid directive at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        if (seenKeys.find(key) != seenKeys.end())
        {
            if (multiKeys.find(key) == multiKeys.end())
            {
                std::cerr << "Duplicate directive key '" << key << "' at " << contextName << ":" << lineno << std::endl;
                return false;
            }
            // allowed to repeat; do not treat as error
        }
        seenKeys.insert(key);
    }
    return true;
}

bool ConfigValidatorUtils::validateBlock(std::istream &is, const std::string &blockName)
{
    std::string body;
    char c;
    int brace_balance = 1;
    while (is.get(c))
    {
        if (c == '{')
        {
            ++brace_balance;
            body.push_back(c);
        }
        else if (c == '}')
        {
            --brace_balance;
            if (brace_balance == 0)
                break;
            body.push_back(c);
        }
        else
        {
            body.push_back(c);
        }
    }
    if (brace_balance != 0)
    {
        std::cerr << "Unbalanced braces in block '" << blockName << "'" << std::endl;
        return false;
    }
    std::istringstream inner(body);
    if (!validateContext(inner, blockName))
        return false;
    return true;
}

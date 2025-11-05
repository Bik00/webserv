# include "../../includes/utils/GeneralParseUtils.hpp"

GeneralParseUtils::GeneralParseUtils(void)
{
}

GeneralParseUtils::~GeneralParseUtils(void)
{
}

GeneralParseUtils::GeneralParseUtils(const GeneralParseUtils &ref)
{
    (void)ref;
}

GeneralParseUtils &GeneralParseUtils::operator=(const GeneralParseUtils &ref)
{
    (void)ref;
    return (*this);
}

std::string GeneralParseUtils::CheckArgc(int argc, char **argv)
{
    std::string configPath;

    if (argc > 2)
        throw std::runtime_error("Too many arguments. Usage: ./webserv <config_file>");
    else if (argc == 1)
        configPath = DEFAULT_CONFIG_PATH;
    else
        configPath = argv[1];

    return (configPath);
}

bool GeneralParseUtils::ValidatePath(const std::string &path)
{
    // split path into components
    std::vector<std::string> comps;
    size_t start = 0;
    // handle leading slash
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

    // build and check each directory component except last
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

    // check final component is a .conf file and exists
    std::string filePath;
    if (absolute) filePath = "/" + comps[0];
    else filePath = comps[0];
    for (size_t i = 1; i < comps.size(); ++i) filePath += std::string("/") + comps[i];

    // ensure last component ends with .conf
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
}
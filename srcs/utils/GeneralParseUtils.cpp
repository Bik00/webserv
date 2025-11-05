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

bool GeneralParseUtils::ValidatePath(const std::string &configPath)
{
    // split configPath into components
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
    return true;
}

bool GeneralParseUtils::CheckSimpleDirectives(const std::string &configPath)
{
    // 파일 읽어들이기 및 simple directive 검사
    std::ifstream ifs(configPath.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << configPath << std::endl;
        return false;
    }

    std::string line;
    size_t lineno = 0;
    std::set<std::string> seenKeys;

    while (std::getline(ifs, line))
    {
        ++lineno;
        // trim leading/trailing whitespace
        size_t a = 0;
        while (a < line.size() && isspace(static_cast<unsigned char>(line[a]))) ++a;
        size_t b = line.size();
        while (b > a && isspace(static_cast<unsigned char>(line[b-1]))) --b;
        if (a >= b) continue; // empty line
        std::string s = line.substr(a, b - a);
        // skip comments
        if (s.size() >= 1 && s[0] == '#') continue;
        // skip block open/close
        if (s == "{" || s == "}") continue;

        // simple directive should end with semicolon
        if (s.empty() || s[s.size() - 1] != ';')
        {
            std::cerr << "Syntax error (missing ';') at line " << lineno << std::endl;
            return false;
        }

        // remove trailing semicolon and trim again
        s.erase(s.size() - 1);
        size_t i = 0;
        while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t j = s.size();
        while (j > i && isspace(static_cast<unsigned char>(s[j-1]))) --j;
        if (i >= j)
        {
            std::cerr << "Empty directive at line " << lineno << std::endl;
            return false;
        }

        // now find first whitespace separating key and value
        size_t sp = i;
        while (sp < j && !isspace(static_cast<unsigned char>(s[sp]))) ++sp;
        if (sp == j)
        {
            std::cerr << "Directive has no value at line " << lineno << std::endl;
            return false;
        }

        // ensure at least one space between key and value and value non-empty
        size_t k = sp;
        while (k < j && isspace(static_cast<unsigned char>(s[k]))) ++k;
        if (k >= j)
        {
            std::cerr << "Directive has empty value at line " << lineno << std::endl;
            return false;
        }

        // success: key = s.substr(i, sp-i), value = s.substr(k, j-k)
        std::string key = s.substr(i, sp - i);
        // check duplicate key
        if (seenKeys.find(key) != seenKeys.end())
        {
            std::cerr << "Duplicate directive key '" << key << "' at line " << lineno << std::endl;
            return false;
        }
        seenKeys.insert(key);
        // we don't store values here; blockParse will handle storing
    }
    return true;
}
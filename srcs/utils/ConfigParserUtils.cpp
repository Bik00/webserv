#include "../includes/utils/ConfigParserUtils.hpp"

ConfigParserUtils::ConfigParserUtils(void)
{
}

ConfigParserUtils::~ConfigParserUtils(void)
{
}

ConfigParserUtils::ConfigParserUtils(const ConfigParserUtils &ref)
{
    (void)ref;
}

ConfigParserUtils &ConfigParserUtils::operator=(const ConfigParserUtils &ref)
{
    (void)ref;
    return *this;
}

std::string ConfigParserUtils::CheckArgc(int argc, char **argv)
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

bool ConfigParserUtils::ValidatePath(const std::string &configPath)
{
    // 1. `configPath`를 '/'로 구분하여 각 구성 요소(디렉토리 이름 또는 파일명)로 분리합니다.
    std::vector<std::string> comps;
    size_t start = 0;
    // 선행 슬래시('/') 처리: 경로가 절대경로(앞에 '/')이면 그 사실을 반영하여 분해를 시작합니다.
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

    // 2. 마지막 요소(파일명)를 제외한 각 구성 요소들을 차례로 조합하여 해당 경로가 실제 디렉토리인지 `stat`으로 확인합니다.
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

    // 3. 마지막 구성 요소가 `.conf` 확장자를 가진 파일인지, 그리고 그 파일이 실제로 존재하는지 검사합니다.
    std::string filePath;
    if (absolute) filePath = "/" + comps[0];
    else filePath = comps[0];
    for (size_t i = 1; i < comps.size(); ++i) filePath += std::string("/") + comps[i];

    // 4. 마지막 구성 요소의 이름이 반드시 `.conf`로 끝나는지 확인합니다.
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

bool ConfigParserUtils::validateContext(std::istream &is, const std::string &contextName)
{
    std::string line;
    size_t lineno = 0;
    std::set<std::string> seenKeys;

    while (std::getline(is, line))
    {
        ++lineno;
        // trim
        size_t a = 0;
        while (a < line.size() && isspace(static_cast<unsigned char>(line[a]))) ++a;
        size_t b = line.size();
        while (b > a && isspace(static_cast<unsigned char>(line[b-1]))) --b;
        if (a >= b) continue; // empty
        std::string s = line.substr(a, b - a);
        if (s.size() > 0 && s[0] == '#') continue; // comment

        // block start on same line: ends with '{'
        if (s.size() >= 1 && s[s.size() - 1] == '{')
        {
            // header before '{'
            std::string header = s.substr(0, s.size() - 1);
            // trim header
            size_t h1 = 0;
            while (h1 < header.size() && isspace(static_cast<unsigned char>(header[h1]))) ++h1;
            size_t h2 = header.size();
            while (h2 > h1 && isspace(static_cast<unsigned char>(header[h2-1]))) --h2;
            if (h1 >= h2)
            {
                std::cerr << "Empty block header before '{' at " << contextName << ":" << lineno << std::endl;
                return false;
            }
            // first token in header is block name
            size_t p = h1;
            while (p < h2 && !isspace(static_cast<unsigned char>(header[p]))) ++p;
            std::string blockName = header.substr(h1, p - h1);
            // call validateBlock which will consume until matching closing '}'
            if (!validateBlock(is, blockName))
                return false;
            // after validateBlock returns, continue reading next lines
            continue;
        }

        // block opener on its own line
        if (s == "{")
        {
            std::cerr << "Unexpected '{' alone at " << contextName << ":" << lineno << std::endl;
            return false;
        }

        // block close for this context
        if (s == "}")
        {
            return true;
        }

        // simple directive must end with ';'
        if (s[s.size() - 1] != ';')
        {
            std::cerr << "Syntax error (missing ';') at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        // remove trailing semicolon
        s.erase(s.size() - 1);
        // trim again
        size_t i = 0;
        while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t j = s.size();
        while (j > i && isspace(static_cast<unsigned char>(s[j-1]))) --j;
        if (i >= j)
        {
            std::cerr << "Empty directive at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        // find first whitespace between key and value
        size_t sp = i;
        while (sp < j && !isspace(static_cast<unsigned char>(s[sp]))) ++sp;
        if (sp == j)
        {
            std::cerr << "Directive has no value at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        size_t k = sp;
        while (k < j && isspace(static_cast<unsigned char>(s[k]))) ++k;
        if (k >= j)
        {
            std::cerr << "Directive has empty value at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        std::string key = s.substr(i, sp - i);
        // duplicate key check in this context
        if (seenKeys.find(key) != seenKeys.end())
        {
            std::cerr << "Duplicate directive key '" << key << "' at " << contextName << ":" << lineno << std::endl;
            return false;
        }
        seenKeys.insert(key);
        // value not stored here; setValue will handle storing
    }
    // EOF reached for top-level; valid if not expecting a closing brace
    return true;
}

bool ConfigParserUtils::validateBlock(std::istream &is, const std::string &blockName)
{
    // Collect block body until matching '}' using brace counting, then
    // validate the inner content recursively.
    std::string body;
    char c;
    int brace_balance = 1; // we're called after seeing the opening '{'
    // consume characters until balance == 0
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
                break; // don't include the closing '}'
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

    // validate inner content using an istringstream
    std::istringstream inner(body);
    if (!validateContext(inner, blockName))
        return false;
    return true;
}
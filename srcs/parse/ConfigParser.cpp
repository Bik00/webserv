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

    // start recursive validation at global context
    return validateContext(ifs, "global");
}

bool ConfigParser::setValue(char **argv, Config &config)
{
    (void)argv;
    (void)config;
    bool ret = true;

    return (ret);
}

bool ConfigParser::validateContext(std::istream &is, const std::string &contextName)
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
            // call ValidateBlock which will consume until matching closing '}'
            if (!validateBlock(is, blockName))
                return false;
            // after ValidateBlock returns, continue reading next lines
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

bool ConfigParser::validateBlock(std::istream &is, const std::string &blockName)
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
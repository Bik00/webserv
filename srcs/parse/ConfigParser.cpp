#include "../includes/Header.hpp"

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

bool ConfigParser::parseConfigFile(HttpConfig **config, const std::string &path)
{
    if (!config)
        return false;

    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        throw std::runtime_error(std::string("Could not open config file: ") + path);
    }

    char ch;
    bool found = false;
    while (ifs.get(ch))
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        throw std::runtime_error(std::string("Configuration file is empty or contains only whitespace: ") + path);
    }

    // simple validation: check brace balance and presence of at least one 'server' token
    ifs.clear();
    ifs.seekg(0);

    int brace_balance = 0;
    size_t server_count = 0;
    std::string token;

    // naive tokenizer: split by whitespace but keep braces and semicolons as tokens
    while (ifs >> token)
    {
        for (size_t i = 0; i < token.size(); ++i)
        {
            char c = token[i];
            if (c == '{') ++brace_balance;
            else if (c == '}') --brace_balance;
        }

        // detect server token (token may include braces, so strip them)
        std::string t = token;
        // remove leading/trailing braces/semicolons
        while (!t.empty() && (t.front() == '{' || t.front() == '}' || t.front() == ';')) t.erase(t.begin());
        while (!t.empty() && (t.back() == '{' || t.back() == '}' || t.back() == ';')) t.pop_back();
        if (t == "server") ++server_count;
    }

    if (brace_balance != 0)
    {
        throw std::runtime_error(std::string("Unmatched braces in config file: ") + path);
    }

    if (server_count == 0)
    {
        // allow zero servers? disallow and report
        throw std::runtime_error(std::string("No 'server' block found in config file: ") + path);
    }

    ifs.close();

    HttpConfig *hc = new HttpConfig();
    if (!hc->loadFromFile(path))
    {
        delete hc;
        *config = NULL;
        throw std::runtime_error(std::string("Failed to parse config file: ") + path);
    }

    *config = hc;
    return true;
}
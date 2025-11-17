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

std::string GeneralParseUtils::ParseContext(const std::string &line)
{
    // trim leading/trailing whitespace
    size_t a = 0;
    while (a < line.size() && isspace(static_cast<unsigned char>(line[a]))) ++a;
    size_t b = line.size();
    while (b > a && isspace(static_cast<unsigned char>(line[b-1]))) --b;
    if (a >= b) return std::string();
    std::string s = line.substr(a, b - a);

    // if whole line is a comment
    if (s.size() >= 1 && s[0] == '#') return std::string();

    // strip inline comments '#' unless inside quotes
    bool in_quote = false;
    for (size_t idx = 0; idx < s.size(); ++idx)
    {
        char c = s[idx];
        if (c == '"') in_quote = !in_quote;
        if (!in_quote && c == '#')
        {
            s.erase(idx);
            break;
        }
    }

    // final trim after removing comment
    a = 0;
    while (a < s.size() && isspace(static_cast<unsigned char>(s[a]))) ++a;
    b = s.size();
    while (b > a && isspace(static_cast<unsigned char>(s[b-1]))) --b;
    if (a >= b) return std::string();
    return s.substr(a, b - a);
}
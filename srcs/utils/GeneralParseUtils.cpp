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

bool GeneralParseUtils::ParseDirective(const std::string &line, std::string &key, std::string &value)
{
    // line is expected to be already processed by ParseContext (no comments, trimmed)
    if (line.empty()) return false;
    std::string s = line;
    // strip trailing ';' if present
    if (!s.empty() && s[s.size() - 1] == ';') s.erase(s.size() - 1);

    size_t i = 0;
    while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
    size_t j = s.size();
    while (j > i && isspace(static_cast<unsigned char>(s[j-1]))) --j;
    if (i >= j) return false;

    size_t sp = i;
    while (sp < j && !isspace(static_cast<unsigned char>(s[sp]))) ++sp;
    if (sp == j) return false; // no value
    key = s.substr(i, sp - i);

    size_t k = sp;
    while (k < j && isspace(static_cast<unsigned char>(s[k]))) ++k;
    if (k >= j) return false;
    value = s.substr(k, j - k);
    return true;
}

bool GeneralParseUtils::ParseBlockHeader(const std::string &line, std::string &blockName)
{
    // line expected to be processed by ParseContext
    if (line.empty()) return false;
    // must end with '{'
    size_t n = line.size();
    if (line[n - 1] != '{') return false;
    std::string header = line.substr(0, n - 1);
    // trim
    size_t h1 = 0;
    while (h1 < header.size() && isspace(static_cast<unsigned char>(header[h1]))) ++h1;
    size_t h2 = header.size();
    while (h2 > h1 && isspace(static_cast<unsigned char>(header[h2-1]))) --h2;
    if (h1 >= h2) return false;
    size_t p = h1;
    while (p < h2 && !isspace(static_cast<unsigned char>(header[p]))) ++p;
    blockName = header.substr(h1, p - h1);
    return true;
}

bool GeneralParseUtils::ReadBlockBody(std::istream &is, std::string &body)
{
    body.clear();
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
                return true;
            body.push_back(c);
        }
        else
        {
            body.push_back(c);
        }
    }
    return false; // unbalanced
}

bool GeneralParseUtils::ParsePositiveInt(const std::string &val, int &out)
{
    std::istringstream iss(val);
    if (!(iss >> out)) return false;
    if (out <= 0) return false;
    return true;
}

bool GeneralParseUtils::ParseListen(const std::string &token, std::string &outHost, int &outPort)
{
    outHost.clear();
    outPort = 0;
    if (token.empty()) return false;

    // (IPv6 bracket syntax intentionally not supported here)

    // all digits -> port only
    bool allDigits = true;
    for (size_t i = 0; i < token.size(); ++i) if (!isdigit(static_cast<unsigned char>(token[i]))) { allDigits = false; break; }
    if (allDigits)
    {
        int p = 0;
        if (!ParsePositiveInt(token, p)) return false;
        if (p <= 0 || p > 65535) return false;
        outPort = p;
        return true;
    }

    // host:port (single colon) or host-only
    size_t colon = token.find(':');
    if (colon != std::string::npos)
    {
        // reject ambiguous IPv6 without brackets
        if (token.find(':', colon + 1) != std::string::npos) return false;
        std::string hostPart = token.substr(0, colon);
        std::string portPart = token.substr(colon + 1);
        int p = 0;
        if (!ParsePositiveInt(portPart, p)) return false;
        if (p <= 0 || p > 65535) return false;
        // try IPv4 validation, otherwise accept as hostname
        struct in_addr a4;
        if (inet_pton(AF_INET, hostPart.c_str(), &a4) == 1)
        {
            outHost = hostPart;
            outPort = p;
            return true;
        }
        // accept domain-like host
        outHost = hostPart;
        outPort = p;
        return true;
    }

    // host-only: validate IPv4 or treat as hostname
    struct in_addr a4;
    if (inet_pton(AF_INET, token.c_str(), &a4) == 1)
    {
        outHost = token;
        outPort = 0;
        return true;
    }
    // if contains ':' assume invalid IPv6 literal without brackets
    if (token.find(':') != std::string::npos) return false;
    outHost = token; // domain name
    outPort = 0;
    return true;
}

// parse strings like "100", "10k", "5M", "1G" into bytes (base 1024).
// returns -1 on error, otherwise number of bytes as long long.
long long GeneralParseUtils::CalcClientMaxBodySize(const std::string &s)
{
    if (s.empty()) return -1;
    // trim
    size_t a = 0;
    while (a < s.size() && isspace(static_cast<unsigned char>(s[a]))) ++a;
    size_t b = s.size();
    while (b > a && isspace(static_cast<unsigned char>(s[b-1]))) --b;
    if (a >= b) return -1;
    std::string t = s.substr(a, b - a);

    // optional suffix
    char suffix = '\0';
    if (!t.empty())
    {
        char last = t[t.size() - 1];
        if (last == 'k' || last == 'K' || last == 'm' || last == 'M' || last == 'g' || last == 'G')
        {
            suffix = last;
            t = t.substr(0, t.size() - 1);
            // trim again
            size_t ta = 0;
            while (ta < t.size() && isspace(static_cast<unsigned char>(t[ta]))) ++ta;
            size_t tb = t.size();
            while (tb > ta && isspace(static_cast<unsigned char>(t[tb-1]))) --tb;
            if (ta >= tb) return -1;
            t = t.substr(ta, tb - ta);
        }
    }

    long long n = 0;
    std::istringstream iss(t);
    if (!(iss >> n)) return -1;
    if (n <= 0) return -1;

    long long mul = 1;
    if (suffix != '\0')
    {
        if (suffix == 'k' || suffix == 'K') mul = 1024LL;
        else if (suffix == 'm' || suffix == 'M') mul = 1024LL * 1024LL;
        else if (suffix == 'g' || suffix == 'G') mul = 1024LL * 1024LL * 1024LL;
    }

    long long total = n * mul;
    if (total <= 0) return -1;
    return total;
}
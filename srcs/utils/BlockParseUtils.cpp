#include "../../includes/utils/BlockParseUtils.hpp"

BlockParseUtils::BlockParseUtils(void)
{
}

BlockParseUtils::~BlockParseUtils(void)
{
}

BlockParseUtils::BlockParseUtils(const BlockParseUtils &ref)
{
    (void)ref;
}

BlockParseUtils &BlockParseUtils::operator=(const BlockParseUtils &ref)
{
    (void)ref;
    return (*this);
}

bool BlockParseUtils::CheckHttpBlock(char **argv, Config &config)
{
    (void)argv;
    (void)config;
    return (true);
}

bool BlockParseUtils::checkBlockContext(std::string key, std::ifstream &ifs)
{
    // after reading the key token, verify the next characters on the same line are exactly " {"
    std::string rest;
    if (!std::getline(ifs, rest))
    {
        std::cerr << "Expected '{' after " << key << std::endl;
        return false;
    }

    // rest contains the remainder of the line (may start with spaces)
    // require exactly one space followed by '{' as the first non-newline characters
    size_t p = 0;
    // count leading spaces
    size_t spaceCount = 0;
    while (p < rest.size() && rest[p] == ' ') { ++spaceCount; ++p; }
    if (spaceCount != 1 || p >= rest.size() || rest[p] != '{')
    {
        std::cerr << "Expected format: '" << key << " {' on a single line" << std::endl;
        return false;
    }

    // scan after the '{' in this line for additional braces
    int brace_balance = 1;
    ++p; // move past '{'
    for (; p < rest.size(); ++p)
    {
        if (rest[p] == '{') ++brace_balance;
        else if (rest[p] == '}') --brace_balance;
    }

    // now continue reading lines until brace_balance becomes 0
    std::string line;
    while (brace_balance > 0 && std::getline(ifs, line))
    {
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '{') ++brace_balance;
            else if (line[i] == '}') --brace_balance;
        }
    }

    if (brace_balance != 0)
    {
        std::cerr << "Block for key '" << key << "' not properly closed with '}'" << std::endl;
        return false;
    }

    return true;
}

bool BlockParseUtils::CheckEventBlock(char **argv,  Config &config)
{
    (void)config; // currently config is unused here
    std::ifstream ifs(argv[1]);
    std::string token;
    int eventsCount = 0;

    while (ifs >> token)
    {
        if (token == "events")
        {
            // call checkBlockContext which consumes until the matching closing brace
            if (!checkBlockContext("events", ifs))
                return false;
            ++eventsCount;
        }
    }

    if (eventsCount == 0)
    {
        std::cerr << "No events block found" << std::endl;
        return false;
    }
    if (eventsCount > 1)
    {
        std::cerr << "Multiple events blocks found" << std::endl;
        return false;
    }
    return true;
}
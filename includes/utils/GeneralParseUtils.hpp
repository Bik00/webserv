#ifndef GENERAL_PARSE_UTILS_HPP
# define GENERAL_PARSE_UTILS_HPP

# include "../parse/ConfigParser.hpp"

class GeneralParseUtils
{
public:
    GeneralParseUtils(void);
    ~GeneralParseUtils(void);
    GeneralParseUtils(const GeneralParseUtils &ref);
    GeneralParseUtils &operator=(const GeneralParseUtils &ref);

    std::string ParseContext(const std::string &line);
    bool ParseDirective(const std::string &line, std::string &key, std::string &value);
    bool ParseBlockHeader(const std::string &line, std::string &blockName);
};

#endif /* GENERAL_PARSE_UTILS_HPP */
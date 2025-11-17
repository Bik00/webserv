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
};

#endif /* GENERAL_PARSE_UTILS_HPP */
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

    std::string CheckArgc(int argc, char **argv);
    bool ValidatePath(const std::string &path);
};

#endif /* GENERAL_PARSE_UTILS_HPP */
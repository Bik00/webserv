#ifndef CONFIG_PARSER_UTILS_HPP
# define CONFIG_PARSER_UTILS_HPP

# include "../libs/Libs.hpp"

class ConfigParserUtils
{
public:
    ConfigParserUtils(void);
    ~ConfigParserUtils(void);
    ConfigParserUtils(const ConfigParserUtils &ref);
    ConfigParserUtils &operator=(const ConfigParserUtils &ref);

    std::string CheckArgc(int argc, char **argv);
    bool ValidatePath(const std::string &configPath);
    // recursive validators for contexts/blocks
    bool validateContext(std::istream &is, const std::string &contextName);
    bool validateBlock(std::istream &is, const std::string &blockName);
};

#endif /* CONFIG_PARSER_UTILS_HPP */
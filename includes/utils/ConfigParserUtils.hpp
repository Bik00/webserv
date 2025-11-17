#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "../parse/ConfigParser.hpp"

class ConfigParserUtils
{
public:
    ConfigParserUtils(void);
    ~ConfigParserUtils(void);
    ConfigParserUtils(const ConfigParserUtils &ref);
    ConfigParserUtils &operator=(const ConfigParserUtils &ref);

    std::string CheckArgc(int argc, char **argv);
    bool ValidatePath(const std::string &configPath);
}

#endif /* CONFIG_PARSER_HPP */
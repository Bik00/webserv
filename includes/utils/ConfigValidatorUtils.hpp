#ifndef CONFIG_VALIDATOR_UTILS_HPP
# define CONFIG_VALIDATOR_UTILS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class ConfigValidatorUtils
{
private:
    std::string checkArgc(int argc, char **argv);
    bool validatePath(const std::string &configPath);
    bool validateContext(std::istream &is, const std::string &contextName);
    bool validateBlock(std::istream &is, const std::string &blockName);

public:
    ConfigValidatorUtils(void);
    ~ConfigValidatorUtils(void);
    ConfigValidatorUtils(const ConfigValidatorUtils &ref);
    ConfigValidatorUtils &operator=(const ConfigValidatorUtils &ref);

    bool Validate(int argc, char **argv, Config &config);
};

#endif /* CONFIG_VALIDATOR_UTILS_HPP */
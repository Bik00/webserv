#ifndef VALIDATE_CONTEXT_UTILS_HPP
# define VALIDATE_CONTEXT_UTILS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class ValidateContextUtils
{
private:
    std::string checkArgc(int argc, char **argv);
    bool validatePath(const std::string &configPath);
    bool validateLine(std::istream &is, const std::string &contextName);
    bool validateBlock(std::istream &is, const std::string &blockName);
public:
    ValidateContextUtils(void);
    ~ValidateContextUtils(void);
    ValidateContextUtils(const ValidateContextUtils &ref);
    ValidateContextUtils &operator=(const ValidateContextUtils &ref);
    bool ValidateContext(int argc, char **argv, Config &config);
};

#endif /* VALIDATE_CONTEXT_UTILS_HPP */


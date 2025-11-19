#ifndef CONFIG_SETTER_UTILS_HPP
#define CONFIG_SETTER_UTILS_HPP

#include "../config/Config.hpp"

class ConfigSetterUtils
{
private:
    bool hasError; // track if any parsing error has occurred
    bool setGlobalValue(std::istream &is, Config &config);
    bool setEventBlock(std::istream &is, Config &config);
    bool setHttpBlock(std::istream &is, Config &config);
    bool setServerBlock(std::istream &is, HttpBlock &httpBlock);
    bool setLocationBlock(std::istream &is, ServerBlock &serverBlock, const std::string &path);
    // centralize parsing of BaseBlock-level directives (root, index, error_page, client_max_body_size, autoindex)
    bool setBaseBlock(const std::string &key, const std::string &val, BaseBlock &bb);
public:
    ConfigSetterUtils(void);
    ~ConfigSetterUtils(void);
    ConfigSetterUtils(const ConfigSetterUtils &ref);
    ConfigSetterUtils &operator=(const ConfigSetterUtils &ref);

    bool SetValue(char **argv, Config &config);
};

#endif // CONFIG_SETTER_UTILS_HPP
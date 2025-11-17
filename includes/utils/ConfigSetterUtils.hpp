#ifndef CONFIG_SETTER_UTILS_HPP
#define CONFIG_SETTER_UTILS_HPP

#include "../config/Config.hpp"

class ConfigSetterUtils
{
private:
    bool setGlobalValue(std::istream &is, Config &config);
    bool setEventBlock(std::istream &is, Config &config);
    bool setHttpBlock(std::istream &is, Config &config);
    bool setServerBlock(std::istream &is, HttpBlock &httpBlock);
    bool setLocationBlock(std::istream &is, ServerBlock &serverBlock);
public:
    ConfigSetterUtils(void);
    ~ConfigSetterUtils(void);
    ConfigSetterUtils(const ConfigSetterUtils &ref);
    ConfigSetterUtils &operator=(const ConfigSetterUtils &ref);

    bool SetValue(char **argv, Config &config);
};

#endif // CONFIG_SETTER_UTILS_HPP
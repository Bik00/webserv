#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../utils/ConfigParserUtils.hpp"

class ConfigParser
{
private:
	bool validate(int argc, char **argv, Config &config);
	bool validateGeneral(int argc, char **argv, Config &config);
	bool validateBlocks(char **argv, Config &config);
	bool setGeneral(char **argv, Config &config);
	bool setBlocks(char **argv, Config &config);
	bool setValue(char **argv, Config &config);
	bool validateContext(std::istream &is, const std::string &contextName);
	bool validateBlock(std::istream &is, const std::string &blockName);
public:
	ConfigParser(void);
	~ConfigParser(void);
	ConfigParser(const ConfigParser &ref);
	ConfigParser &operator=(const ConfigParser &ref);
	bool Parse(int argc, char **argv, Config &config);
};

#endif /* CONFIG_PARSER_HPP */
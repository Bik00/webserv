#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../utils/GeneralParseUtils.hpp"
# include "../utils/BlockParseUtils.hpp"

class ConfigParser
{
private:
	bool generalParse(int argc, char **argv);
	bool blockParse(Config &config);
public:
	ConfigParser(void);
	~ConfigParser(void);
	ConfigParser(const ConfigParser &ref);
	ConfigParser &operator=(const ConfigParser &ref);

	bool Parse(int argc, char **argv, Config &config);
};

#endif /* CONFIG_PARSER_HPP */
#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "../libs/Libs.hpp"

class ConfigParser
{
public:
	ConfigParser(void);
	~ConfigParser(void);
	ConfigParser(const ConfigParser &ref);
	ConfigParser &operator=(const ConfigParser &ref);
};

#endif /* CONFIG_PARSER_HPP */
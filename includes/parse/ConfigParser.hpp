#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "../Header.hpp"

class ConfigParser
{
public:
	ConfigParser(void);
	~ConfigParser(void);
	ConfigParser(const ConfigParser &ref);
	ConfigParser &operator=(const ConfigParser &ref);

	bool parseConfigFile(HttpConfig **config, const std::string &path);
};

#endif /* CONFIG_PARSER_HPP */
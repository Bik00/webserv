#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include "../libs/Libs.hpp"
# include "./block/HttpBlock.hpp"
# include "./block/EventBlock.hpp"

class Config
{
private:
	std::vector<std::map<std::string, std::string> >	simpleDirectives;
	std::vector<HttpBlock>							httpBlocks;
	std::vector<EventBlock>							eventBlocks;
	std::string										configPath;

public:
	Config(void);
	~Config(void);
	Config(const Config &ref);
	Config &operator=(const Config &ref);
	void setConfigPath(const std::string &path);
	const std::string &getConfigPath() const;
};

#endif /* SERVER_CONFIG_HPP */

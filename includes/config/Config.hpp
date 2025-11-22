#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include "../libs/Libs.hpp"
# include "./block/HttpBlock.hpp"

class Config
{
private:
	std::string	configPath;
	HttpBlock	httpBlock;
public:
	Config(void);
	~Config(void);
	Config(const Config &ref);
	Config &operator=(const Config &ref);

	void setConfigPath(const std::string &path);
	void addHttpBlock(const HttpBlock &httpBlock);

	const HttpBlock &getHttpBlock() const;
	const std::string &getConfigPath() const;
	void printConfig() const;
};

#endif /* SERVER_CONFIG_HPP */

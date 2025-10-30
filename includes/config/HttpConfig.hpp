#ifndef HTTP_CONFIG_HPP
# define HTTP_CONFIG_HPP

# include "../libs/Libs.hpp"
# include "ServerConfig.hpp"

class HttpConfig
{
private:
	std::vector<ServerConfig> servers;
public:
	HttpConfig(void);
	~HttpConfig(void);
	HttpConfig(const HttpConfig &ref);
	HttpConfig &operator=(const HttpConfig &ref);

	// load configuration from file
	bool loadFromFile(const std::string &path);

	// debug: print loaded servers
	void getInfo(void) const;

	// number of parsed servers
	size_t getServerCount(void) const;

	// get server config by index
	const ServerConfig &getServer(size_t idx) const;
};

#endif /* HTTP_CONFIG_HPP */
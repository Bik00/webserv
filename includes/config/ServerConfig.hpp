#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include "../libs/Libs.hpp"
# include "LocationConfig.hpp"

class ServerConfig
{
private:
    std::string address; // example: 'localhost' 
    unsigned int port;

    std::string root;
    std::string index;

    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    std::vector<LocationConfig> locations;

public:
	ServerConfig(void);
	~ServerConfig(void);
	ServerConfig(const ServerConfig &ref);
	ServerConfig &operator=(const ServerConfig &ref);

    std::string getHost(void) const;
    unsigned int getPort(void) const;
    std::string getRoot(void) const;
    std::string getIndex(void) const;
};

#endif /* SERVER_CONFIG_HPP */

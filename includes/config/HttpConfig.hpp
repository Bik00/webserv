#ifndef HTTP_CONFIG_HPP
# define HTTP_CONFIG_HPP

# include "../libs/Libs.hpp"

class HttpConfig
{
private:
    unsigned int    port;
    std::string     address;
public:
	HttpConfig(void);
	HttpConfig(unsigned int port, std::string address);
	~HttpConfig(void);
	HttpConfig(const HttpConfig &ref);
	HttpConfig &operator=(const HttpConfig &ref);
    void    getInfo(void);
};

#endif /* HTTP_CONFIG_HPP */
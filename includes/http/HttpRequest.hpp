#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../libs/Libs.hpp"

class HttpRequest
{
public:
    HttpRequest(void);
    ~HttpRequest(void);
    HttpRequest(const HttpRequest &ref);
    HttpRequest &operator=(const HttpRequest &ref);
};

#endif /* HTTP_REQUEST_HPP */
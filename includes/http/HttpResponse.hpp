#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include "../libs/Libs.hpp"

class HttpResponse
{
public:
    HttpResponse(void);
    ~HttpResponse(void);
    HttpResponse(const HttpResponse &ref);
    HttpResponse &operator=(const HttpResponse &ref);
};

#endif /* HTTP_RESPONSE_HPP */
#ifndef HTTP_TRANSACTION_HPP
# define HTTP_TRANSACTION_HPP

# include "../libs/Libs.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class HttpTransaction
{
private:
    HttpRequest request;
    HttpResponse response;
public:
    HttpTransaction(void);
    ~HttpTransaction(void);
    HttpTransaction(const HttpTransaction &ref);
    HttpTransaction &operator=(const HttpTransaction &ref);
};

#endif /* HTTP_TRANSACTION_HPP */
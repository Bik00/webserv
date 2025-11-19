#include "../../includes/http/HttpRequest.hpp"

HttpRequest::HttpRequest(void)
{
}

HttpRequest::~HttpRequest(void)
{
}

HttpRequest::HttpRequest(const HttpRequest &ref)
{
    (void)ref;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &ref)
{
    (void)ref;
    return (*this);
}

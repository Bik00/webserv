#include "../../includes/http/HttpResponse.hpp"

HttpResponse::HttpResponse(void)
{
}

HttpResponse::~HttpResponse(void)
{
}

HttpResponse::HttpResponse(const HttpResponse &ref)
{
    (void)ref;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &ref)
{
    (void)ref;
    return (*this);
}

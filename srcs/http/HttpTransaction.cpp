#include "../../includes/http/HttpTransaction.hpp"

HttpTransaction::HttpTransaction(void)
{
}

HttpTransaction::~HttpTransaction(void)
{
}

HttpTransaction::HttpTransaction(const HttpTransaction &ref)
{
    (void)ref;
}

HttpTransaction &HttpTransaction::operator=(const HttpTransaction &ref)
{
    (void)ref;
    return (*this);
}

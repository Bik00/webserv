#include "../../includes/http/HttpResponse.hpp"

HttpResponse::HttpResponse(void)
    : httpVersion("HTTP/1.1"), statusCode(200), reasonPhrase("OK"), isBuilt(false)
{
}

HttpResponse::~HttpResponse(void)
{
}

HttpResponse::HttpResponse(const HttpResponse &ref)
    : httpVersion(ref.httpVersion), statusCode(ref.statusCode),
      reasonPhrase(ref.reasonPhrase), headers(ref.headers), body(ref.body),
      isBuilt(ref.isBuilt), rawResponse(ref.rawResponse)
{
}

HttpResponse &HttpResponse::operator=(const HttpResponse &ref)
{
    if (this != &ref)
    {
        httpVersion = ref.httpVersion;
        statusCode = ref.statusCode;
        reasonPhrase = ref.reasonPhrase;
        headers = ref.headers;
        body = ref.body;
        isBuilt = ref.isBuilt;
        rawResponse = ref.rawResponse;
    }
    return (*this);
}

// Building response
void HttpResponse::setStatus(int code, const std::string &phrase)
{
    statusCode = code;
    if (phrase.empty())
        reasonPhrase = getDefaultReasonPhrase(code);
    else
        reasonPhrase = phrase;
    isBuilt = false;
}

void HttpResponse::setHttpVersion(const std::string &version)
{
    httpVersion = version;
    isBuilt = false;
}

void HttpResponse::setBody(const std::string &b)
{
    body = b;
    isBuilt = false;
}

void HttpResponse::addHeader(const std::string &name, const std::string &value)
{
    headers[name] = value;
    isBuilt = false;
}

void HttpResponse::setHeader(const std::string &name, const std::string &value)
{
    headers[name] = value;
    isBuilt = false;
}

// Auto-set common headers
void HttpResponse::setContentType(const std::string &type)
{
    setHeader("Content-Type", type);
}

void HttpResponse::setContentLength(size_t length)
{
    std::ostringstream oss;
    oss << length;
    setHeader("Content-Length", oss.str());
}

void HttpResponse::setConnection(const std::string &conn)
{
    setHeader("Connection", conn);
}

// Build final response
std::string HttpResponse::build(void)
{
    if (isBuilt)
        return rawResponse;
    
    std::ostringstream oss;
    
    // Status-Line
    oss << httpVersion << " " << statusCode << " " << reasonPhrase << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it)
    {
        oss << it->first << ": " << it->second << "\r\n";
    }
    
    // Empty line separating headers from body
    oss << "\r\n";
    
    // Body
    oss << body;
    
    rawResponse = oss.str();
    isBuilt = true;
    return rawResponse;
}

const std::string &HttpResponse::getRawResponse(void) const
{
    return rawResponse;
}

// Getters
int HttpResponse::getStatusCode(void) const { return statusCode; }
const std::string &HttpResponse::getReasonPhrase(void) const { return reasonPhrase; }
const std::string &HttpResponse::getHttpVersion(void) const { return httpVersion; }
const std::string &HttpResponse::getBody(void) const { return body; }
const std::map<std::string, std::string> &HttpResponse::getHeaders(void) const { return headers; }

std::string HttpResponse::getHeader(const std::string &name) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(name);
    if (it != headers.end())
        return it->second;
    return "";
}

bool HttpResponse::hasHeader(const std::string &name) const
{
    return headers.find(name) != headers.end();
}

// Utility
void HttpResponse::reset(void)
{
    httpVersion = "HTTP/1.1";
    statusCode = 200;
    reasonPhrase = "OK";
    headers.clear();
    body.clear();
    isBuilt = false;
    rawResponse.clear();
}

// Static helper for default reason phrases (RFC 7231)
std::string HttpResponse::getDefaultReasonPhrase(int code)
{
    switch (code)
    {
        // 2xx Success
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        
        // 3xx Redirection
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";
        
        // 4xx Client Error
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 411: return "Length Required";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        
        // 5xx Server Error
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        
        default: return "Unknown";
    }
}

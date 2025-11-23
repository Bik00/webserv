#include "../../includes/http/HttpRequest.hpp"

HttpRequest::HttpRequest(void)
    : contentLength(0), chunked(false), state(PARSE_REQUEST_LINE),
      parsedBytes(0), isValid(true), errorCode(0)
{
}

HttpRequest::~HttpRequest(void)
{
}

HttpRequest::HttpRequest(const HttpRequest &ref)
    : method(ref.method), uri(ref.uri), path(ref.path), query(ref.query),
      httpVersion(ref.httpVersion), headers(ref.headers), body(ref.body),
      contentLength(ref.contentLength), chunked(ref.chunked), state(ref.state),
      rawRequest(ref.rawRequest), parsedBytes(ref.parsedBytes),
      isValid(ref.isValid), errorMsg(ref.errorMsg), errorCode(ref.errorCode)
{
}

HttpRequest &HttpRequest::operator=(const HttpRequest &ref)
{
    if (this != &ref)
    {
        method = ref.method;
        uri = ref.uri;
        path = ref.path;
        query = ref.query;
        httpVersion = ref.httpVersion;
        headers = ref.headers;
        body = ref.body;
        contentLength = ref.contentLength;
        chunked = ref.chunked;
        state = ref.state;
        rawRequest = ref.rawRequest;
        parsedBytes = ref.parsedBytes;
        isValid = ref.isValid;
        errorMsg = ref.errorMsg;
        errorCode = ref.errorCode;
    }
    return (*this);
}

// Parsing methods
bool HttpRequest::parse(const std::string &data)
{
    rawRequest += data;
    
    while (state != PARSE_COMPLETE && state != PARSE_ERROR)
    {
        if (state == PARSE_REQUEST_LINE)
        {
            if (!parseRequestLine())
                return false;
        }
        else if (state == PARSE_HEADERS)
        {
            if (!parseHeaders())
                return false;
        }
        else if (state == PARSE_BODY)
        {
            if (!parseBody())
                return false;
        }
    }
    
    return state == PARSE_COMPLETE;
}

// Parse Request-Line: Method SP Request-URI SP HTTP-Version CRLF
bool HttpRequest::parseRequestLine()
{
    // Find end of request line (CRLF)
    size_t crlfPos = rawRequest.find("\r\n", parsedBytes);
    if (crlfPos == std::string::npos)
    {
        // Need more data
        return true;
    }
    
    // Extract request line
    std::string requestLine = rawRequest.substr(parsedBytes, crlfPos - parsedBytes);
    parsedBytes = crlfPos + 2; // Skip CRLF
    
    // Parse: Method SP URI SP Version
    std::istringstream iss(requestLine);
    std::string version;
    
    if (!(iss >> method >> uri >> version))
    {
        isValid = false;
        errorCode = 400;
        errorMsg = "Invalid request line format";
        state = PARSE_ERROR;
        return false;
    }
    
    // Validate HTTP version
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
    {
        isValid = false;
        errorCode = 505;
        errorMsg = "HTTP Version Not Supported";
        state = PARSE_ERROR;
        return false;
    }
    httpVersion = version;
    
    // Validate method (basic validation)
    if (method != "GET" && method != "POST" && method != "DELETE" &&
        method != "PUT" && method != "HEAD" && method != "OPTIONS")
    {
        isValid = false;
        errorCode = 501;
        errorMsg = "Method Not Implemented";
        state = PARSE_ERROR;
        return false;
    }
    
    // Parse URI into path and query
    parseUri();
    
    // Check URI length (RFC 7230 recommends at least 8000 octets)
    if (uri.length() > 8000)
    {
        isValid = false;
        errorCode = 414;
        errorMsg = "URI Too Long";
        state = PARSE_ERROR;
        return false;
    }
    
    state = PARSE_HEADERS;
    return true;
}

// Parse URI into path and query components
void HttpRequest::parseUri()
{
    size_t queryPos = uri.find('?');
    if (queryPos != std::string::npos)
    {
        path = uri.substr(0, queryPos);
        query = uri.substr(queryPos + 1);
    }
    else
    {
        path = uri;
        query = "";
    }
    
    // Decode path (basic implementation - can be enhanced)
    // For now, just use it as-is
}

// Parse Headers: field-name ":" OWS field-value OWS CRLF
bool HttpRequest::parseHeaders()
{
    while (true)
    {
        // Find end of header line
        size_t crlfPos = rawRequest.find("\r\n", parsedBytes);
        if (crlfPos == std::string::npos)
        {
            // Need more data
            return true;
        }
        
        // Empty line means end of headers
        if (crlfPos == parsedBytes)
        {
            parsedBytes += 2; // Skip CRLF
            
            // Check if we need to parse body
            if (hasHeader("Content-Length") || hasHeader("Transfer-Encoding"))
            {
                // Extract Content-Length
                std::string clStr = getHeader("content-length");
                if (!clStr.empty())
                {
                    std::istringstream iss(clStr);
                    long cl = 0;
                    iss >> cl;
                    
                    // Validate content length
                    if (iss.fail() || cl < 0)
                    {
                        isValid = false;
                        errorCode = 400;
                        errorMsg = "Invalid Content-Length";
                        state = PARSE_ERROR;
                        return false;
                    }
                    contentLength = static_cast<size_t>(cl);
                }
                
                // Check for chunked encoding
                std::string te = getHeader("transfer-encoding");
                if (te.find("chunked") != std::string::npos)
                {
                    chunked = true;
                }
                
                state = PARSE_BODY;
            }
            else
            {
                // No body
                state = PARSE_COMPLETE;
            }
            return true;
        }
        
        // Extract header line
        std::string headerLine = rawRequest.substr(parsedBytes, crlfPos - parsedBytes);
        parsedBytes = crlfPos + 2;
        
        // Parse header: name: value
        size_t colonPos = headerLine.find(':');
        if (colonPos == std::string::npos)
        {
            isValid = false;
            errorCode = 400;
            errorMsg = "Invalid header format";
            state = PARSE_ERROR;
            return false;
        }
        
        std::string name = headerLine.substr(0, colonPos);
        std::string value = headerLine.substr(colonPos + 1);
        
        // Trim whitespace from value (OWS - optional whitespace)
        size_t start = value.find_first_not_of(" \t");
        size_t end = value.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos)
        {
            value = value.substr(start, end - start + 1);
        }
        else
        {
            value = "";
        }
        
        // Convert header name to lowercase for case-insensitive comparison
        // (HTTP header names are case-insensitive per RFC 7230)
        std::string lowerName = name;
        for (size_t i = 0; i < lowerName.length(); ++i)
        {
            lowerName[i] = std::tolower(static_cast<unsigned char>(lowerName[i]));
        }
        
        headers[lowerName] = value;
    }
}

// Parse Body
bool HttpRequest::parseBody()
{
    if (chunked)
    {
        // TODO: Implement chunked transfer encoding
        // For now, just mark as error
        isValid = false;
        errorCode = 501;
        errorMsg = "Chunked transfer encoding not yet implemented";
        state = PARSE_ERROR;
        return false;
    }
    
    // Check if we have enough data
    size_t remainingData = rawRequest.length() - parsedBytes;
    if (remainingData < contentLength)
    {
        // Need more data
        return true;
    }
    
    // Extract body
    body = rawRequest.substr(parsedBytes, contentLength);
    parsedBytes += contentLength;
    
    state = PARSE_COMPLETE;
    return true;
}

bool HttpRequest::isComplete(void) const
{
    return state == PARSE_COMPLETE || state == PARSE_ERROR;
}

ParseState HttpRequest::getState(void) const
{
    return state;
}

// Getters
const std::string &HttpRequest::getMethod(void) const { return method; }
const std::string &HttpRequest::getUri(void) const { return uri; }
const std::string &HttpRequest::getPath(void) const { return path; }
const std::string &HttpRequest::getQuery(void) const { return query; }
const std::string &HttpRequest::getHttpVersion(void) const { return httpVersion; }
const std::string &HttpRequest::getBody(void) const { return body; }
const std::map<std::string, std::string> &HttpRequest::getHeaders(void) const { return headers; }

std::string HttpRequest::getHeader(const std::string &name) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(name);
    if (it != headers.end())
        return it->second;
    return "";
}

bool HttpRequest::hasHeader(const std::string &name) const
{
    return headers.find(name) != headers.end();
}

size_t HttpRequest::getContentLength(void) const { return contentLength; }
bool HttpRequest::isChunked(void) const { return chunked; }

// Validation
bool HttpRequest::isRequestValid(void) const { return isValid; }
int HttpRequest::getErrorCode(void) const { return errorCode; }
const std::string &HttpRequest::getErrorMsg(void) const { return errorMsg; }

// Setters
void HttpRequest::setMethod(const std::string &m) { method = m; }
void HttpRequest::setUri(const std::string &u) { uri = u; }
void HttpRequest::setPath(const std::string &p) { path = p; }
void HttpRequest::setQuery(const std::string &q) { query = q; }
void HttpRequest::setHttpVersion(const std::string &v) { httpVersion = v; }
void HttpRequest::setBody(const std::string &b) { body = b; }

void HttpRequest::addHeader(const std::string &name, const std::string &value)
{
    headers[name] = value;
}

// Utility
void HttpRequest::reset(void)
{
    method.clear();
    uri.clear();
    path.clear();
    query.clear();
    httpVersion.clear();
    headers.clear();
    body.clear();
    contentLength = 0;
    chunked = false;
    state = PARSE_REQUEST_LINE;
    rawRequest.clear();
    parsedBytes = 0;
    isValid = true;
    errorMsg.clear();
    errorCode = 0;
}

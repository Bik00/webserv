#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../libs/Libs.hpp"

// HTTP Request parsing states
enum ParseState
{
    PARSE_REQUEST_LINE,
    PARSE_HEADERS,
    PARSE_BODY,
    PARSE_COMPLETE,
    PARSE_ERROR
};

class HttpRequest
{
private:
    // Request-Line: Method SP Request-URI SP HTTP-Version CRLF
    std::string method;           // GET, POST, DELETE, etc.
    std::string uri;              // Request-URI (path + query)
    std::string path;             // Decoded path component
    std::string query;            // Query string (after '?')
    std::string httpVersion;      // HTTP/1.1, HTTP/1.0
    
    // Headers: field-name ":" OWS field-value OWS
    std::map<std::string, std::string> headers;
    
    // Message Body
    std::string body;
    size_t contentLength;         // Content-Length header value
    bool chunked;                 // Transfer-Encoding: chunked
    
    // Parsing state
    ParseState state;
    std::string rawRequest;       // Raw received data
    size_t parsedBytes;           // How many bytes have been parsed
    
    // Request validation
    bool isValid;
    std::string errorMsg;
    int errorCode;                // HTTP error code (400, 413, etc.)
    
    // Private parsing helpers
    bool parseRequestLine();
    bool parseHeaders();
    bool parseBody();
    void parseUri();

public:
    HttpRequest(void);
    ~HttpRequest(void);
    HttpRequest(const HttpRequest &ref);
    HttpRequest &operator=(const HttpRequest &ref);
    
    // Parsing
    bool parse(const std::string &data);
    bool isComplete(void) const;
    ParseState getState(void) const;
    
    // Getters
    const std::string &getMethod(void) const;
    const std::string &getUri(void) const;
    const std::string &getPath(void) const;
    const std::string &getQuery(void) const;
    const std::string &getHttpVersion(void) const;
    const std::string &getBody(void) const;
    const std::map<std::string, std::string> &getHeaders(void) const;
    std::string getHeader(const std::string &name) const;
    bool hasHeader(const std::string &name) const;
    size_t getContentLength(void) const;
    bool isChunked(void) const;
    
    // Validation
    bool isRequestValid(void) const;
    int getErrorCode(void) const;
    const std::string &getErrorMsg(void) const;
    
    // Setters (for manual construction or testing)
    void setMethod(const std::string &m);
    void setUri(const std::string &u);
    void setPath(const std::string &p);
    void setQuery(const std::string &q);
    void setHttpVersion(const std::string &v);
    void setBody(const std::string &b);
    void addHeader(const std::string &name, const std::string &value);
    
    // Utility
    void reset(void);
};

#endif /* HTTP_REQUEST_HPP */
#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include "../libs/Libs.hpp"

class HttpResponse
{
private:
    // Status-Line: HTTP-Version SP Status-Code SP Reason-Phrase CRLF
    std::string httpVersion;      // HTTP/1.1
    int statusCode;               // 200, 404, 500, etc.
    std::string reasonPhrase;     // OK, Not Found, Internal Server Error, etc.
    
    // Headers: field-name ":" OWS field-value OWS
    std::map<std::string, std::string> headers;
    
    // Message Body
    std::string body;
    
    // Response state
    bool isBuilt;                 // Whether response has been built
    std::string rawResponse;      // Complete HTTP response (headers + body)

public:
    HttpResponse(void);
    ~HttpResponse(void);
    HttpResponse(const HttpResponse &ref);
    HttpResponse &operator=(const HttpResponse &ref);
    
    // Building response
    void setStatus(int code, const std::string &phrase = "");
    void setHttpVersion(const std::string &version);
    void setBody(const std::string &b);
    void addHeader(const std::string &name, const std::string &value);
    void setHeader(const std::string &name, const std::string &value);
    
    // Auto-set common headers
    void setContentType(const std::string &type);
    void setContentLength(size_t length);
    void setConnection(const std::string &conn); // close, keep-alive
    
    // Build final response
    std::string build(void);
    const std::string &getRawResponse(void) const;
    
    // Getters
    int getStatusCode(void) const;
    const std::string &getReasonPhrase(void) const;
    const std::string &getHttpVersion(void) const;
    const std::string &getBody(void) const;
    const std::map<std::string, std::string> &getHeaders(void) const;
    std::string getHeader(const std::string &name) const;
    bool hasHeader(const std::string &name) const;
    
    // Utility
    void reset(void);
    static std::string getDefaultReasonPhrase(int code);
};

#endif /* HTTP_RESPONSE_HPP */
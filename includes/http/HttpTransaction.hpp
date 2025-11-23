#ifndef HTTP_TRANSACTION_HPP
# define HTTP_TRANSACTION_HPP

# include "../libs/Libs.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "FileUploadHandler.hpp"
# include "StaticFileHandler.hpp"

// Forward declarations
class ServerBlock;
class LocationBlock;

// Transaction states
enum TransactionState
{
    TRANS_READING_REQUEST,
    TRANS_PROCESSING,
    TRANS_SENDING_RESPONSE,
    TRANS_COMPLETE,
    TRANS_ERROR
};

class HttpTransaction
{
private:
    HttpRequest request;
    HttpResponse response;
    TransactionState state;
    time_t startTime;
    time_t lastActivityTime;
    const ServerBlock *serverBlock;
    const LocationBlock *locationBlock;

public:
    HttpTransaction(void);
    ~HttpTransaction(void);
    HttpTransaction(const HttpTransaction &ref);
    HttpTransaction &operator=(const HttpTransaction &ref);
    
    // Request handling
    bool appendRequestData(const std::string &data);
    bool isRequestComplete(void) const;
    
    // Response handling
    void buildResponse(const ServerBlock *server = NULL, const LocationBlock *location = NULL);
    const std::string &getResponseData(void) const;
    bool isResponseComplete(void) const;
    
    // State management
    TransactionState getState(void) const;
    void setState(TransactionState s);
    
    // Getters
    HttpRequest &getRequest(void);
    HttpResponse &getResponse(void);
    const HttpRequest &getRequest(void) const;
    const HttpResponse &getResponse(void) const;
    
    // Timing
    time_t getStartTime(void) const;
    time_t getLastActivityTime(void) const;
    void touch(void);
    
    // Utility
    void reset(void);
};

#endif /* HTTP_TRANSACTION_HPP */
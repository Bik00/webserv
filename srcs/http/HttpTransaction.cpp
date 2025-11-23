#include "../../includes/http/HttpTransaction.hpp"
#include "../../includes/config/block/ServerBlock.hpp"
#include "../../includes/config/block/LocationBlock.hpp"
#include "../../includes/cgi/CgiHandler.hpp"

HttpTransaction::HttpTransaction(void)
    : state(TRANS_READING_REQUEST), startTime(time(NULL)), lastActivityTime(time(NULL)),
      serverBlock(NULL), locationBlock(NULL)
{
}

HttpTransaction::~HttpTransaction(void)
{
}

HttpTransaction::HttpTransaction(const HttpTransaction &ref)
    : request(ref.request), response(ref.response), state(ref.state),
      startTime(ref.startTime), lastActivityTime(ref.lastActivityTime),
      serverBlock(ref.serverBlock), locationBlock(ref.locationBlock)
{
}

HttpTransaction &HttpTransaction::operator=(const HttpTransaction &ref)
{
    if (this != &ref)
    {
        request = ref.request;
        response = ref.response;
        state = ref.state;
        startTime = ref.startTime;
        lastActivityTime = ref.lastActivityTime;
        serverBlock = ref.serverBlock;
        locationBlock = ref.locationBlock;
    }
    return (*this);
}

// Request handling
bool HttpTransaction::appendRequestData(const std::string &data)
{
    touch();
    return request.parse(data);
}

bool HttpTransaction::isRequestComplete(void) const
{
    return request.isComplete();
}

// Response handling
void HttpTransaction::buildResponse(const ServerBlock *server, const LocationBlock *location)
{
    serverBlock = server;
    locationBlock = location;
    
    // Check if this is a CGI request
    if (locationBlock && locationBlock->hasCgi() && serverBlock)
    {
        CgiHandler cgiHandler(request, *serverBlock, locationBlock);
        
        if (cgiHandler.isCgiRequest())
        {
            std::cout << "[CGI] Executing script: " << request.getPath() << std::endl;
            
            // Execute CGI script
            if (cgiHandler.execute())
            {
                // Build response from CGI output
                cgiHandler.buildResponse(response);
                std::cout << "[CGI] Execution successful" << std::endl;
                state = TRANS_SENDING_RESPONSE;
                return;
            }
            else
            {
                std::cout << "[CGI] Execution failed - returning 500" << std::endl;
                response.setStatus(500);
                response.setBody("Internal Server Error: CGI execution failed");
                response.setContentType("text/plain");
                response.setContentLength(response.getBody().size());
                response.setConnection("close");
                response.build();
                state = TRANS_SENDING_RESPONSE;
                return;
            }
        }
    }
    
    // Default static response
    response.setStatus(200);
    response.setBody("Hello, World!");
    response.setContentType("text/plain");
    response.setContentLength(response.getBody().size());
    response.setConnection("close");
    response.build();
    
    state = TRANS_SENDING_RESPONSE;
}

const std::string &HttpTransaction::getResponseData(void) const
{
    return response.getRawResponse();
}

bool HttpTransaction::isResponseComplete(void) const
{
    return state == TRANS_COMPLETE;
}

// State management
TransactionState HttpTransaction::getState(void) const
{
    return state;
}

void HttpTransaction::setState(TransactionState s)
{
    state = s;
}

// Getters
HttpRequest &HttpTransaction::getRequest(void)
{
    return request;
}

HttpResponse &HttpTransaction::getResponse(void)
{
    return response;
}

const HttpRequest &HttpTransaction::getRequest(void) const
{
    return request;
}

const HttpResponse &HttpTransaction::getResponse(void) const
{
    return response;
}

// Timing
time_t HttpTransaction::getStartTime(void) const
{
    return startTime;
}

time_t HttpTransaction::getLastActivityTime(void) const
{
    return lastActivityTime;
}

void HttpTransaction::touch(void)
{
    lastActivityTime = time(NULL);
}

// Utility
void HttpTransaction::reset(void)
{
    request.reset();
    response.reset();
    state = TRANS_READING_REQUEST;
    startTime = time(NULL);
    lastActivityTime = time(NULL);
}

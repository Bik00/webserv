#ifndef STATIC_FILE_HANDLER_HPP
# define STATIC_FILE_HANDLER_HPP

# include "../libs/Libs.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

// Forward declarations
class ServerBlock;
class LocationBlock;

class StaticFileHandler
{
private:
    std::string getMimeType(const std::string &path);
    bool fileExists(const std::string &path);
    std::string readFile(const std::string &path);
    std::string resolveFilePath(const HttpRequest &request, const ServerBlock *server, const LocationBlock *location);
    std::string tryIndexFiles(const std::string &dirPath, const LocationBlock *location);

public:
    StaticFileHandler(void);
    ~StaticFileHandler(void);
    
    bool handleRequest(const HttpRequest &request, HttpResponse &response, const ServerBlock *server, const LocationBlock *location);
};

#endif /* STATIC_FILE_HANDLER_HPP */

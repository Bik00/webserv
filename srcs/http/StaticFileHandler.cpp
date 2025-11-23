#include "../../includes/http/StaticFileHandler.hpp"
#include "../../includes/config/block/ServerBlock.hpp"
#include "../../includes/config/block/LocationBlock.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

StaticFileHandler::StaticFileHandler(void)
{
}

StaticFileHandler::~StaticFileHandler(void)
{
}

// Get MIME type based on file extension
std::string StaticFileHandler::getMimeType(const std::string &path)
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos)
        return "application/octet-stream";
    
    std::string ext = path.substr(dotPos);
    
    // Convert to lowercase
    for (size_t i = 0; i < ext.length(); ++i)
        ext[i] = std::tolower(static_cast<unsigned char>(ext[i]));
    
    // Common MIME types
    if (ext == ".html" || ext == ".htm")
        return "text/html";
    else if (ext == ".css")
        return "text/css";
    else if (ext == ".js")
        return "application/javascript";
    else if (ext == ".json")
        return "application/json";
    else if (ext == ".xml")
        return "application/xml";
    else if (ext == ".txt")
        return "text/plain";
    else if (ext == ".jpg" || ext == ".jpeg")
        return "image/jpeg";
    else if (ext == ".png")
        return "image/png";
    else if (ext == ".gif")
        return "image/gif";
    else if (ext == ".svg")
        return "image/svg+xml";
    else if (ext == ".ico")
        return "image/x-icon";
    else if (ext == ".pdf")
        return "application/pdf";
    else if (ext == ".zip")
        return "application/zip";
    else
        return "application/octet-stream";
}

// Check if file exists and is readable
bool StaticFileHandler::fileExists(const std::string &path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == 0)
    {
        return S_ISREG(st.st_mode);  // Is regular file
    }
    return false;
}

// Read entire file content
std::string StaticFileHandler::readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        return "";
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

// Try index files in order
std::string StaticFileHandler::tryIndexFiles(const std::string &dirPath, const LocationBlock *location)
{
    if (!location)
        return "";
    
    const std::vector<std::string> &indexes = location->getIndexFiles();
    
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        std::string indexPath = dirPath + "/" + indexes[i];
        if (fileExists(indexPath))
        {
            std::cout << "[STATIC] Found index file: " << indexPath << std::endl;
            return indexPath;
        }
    }
    
    return "";
}

// Resolve file path based on request URI and location config
std::string StaticFileHandler::resolveFilePath(const HttpRequest &request, const ServerBlock *server, const LocationBlock *location)
{
    std::string path = request.getPath();
    std::string root;
    
    // Get root directory
    if (location && !location->getRoot().empty())
    {
        root = location->getRoot();
    }
    else if (server && !server->getRoot().empty())
    {
        root = server->getRoot();
    }
    else
    {
        root = ".";
    }
    
    // Remove location prefix from path
    if (location)
    {
        std::string locPath = location->getPath();
        if (path.find(locPath) == 0)
        {
            path = path.substr(locPath.length());
            if (path.empty() || path[0] != '/')
                path = "/" + path;
        }
    }
    
    // Build full file path
    std::string fullPath = root + path;
    
    std::cout << "[STATIC] Resolved path: " << fullPath << std::endl;
    
    // Check if it's a directory
    struct stat st;
    if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
    {
        std::cout << "[STATIC] Path is directory, trying index files" << std::endl;
        
        // Try index files
        std::string indexFile = tryIndexFiles(fullPath, location);
        if (!indexFile.empty())
            return indexFile;
        
        // If autoindex is enabled, generate directory listing
        // TODO: Implement autoindex
        
        return "";
    }
    
    return fullPath;
}

// Handle static file request
bool StaticFileHandler::handleRequest(const HttpRequest &request, HttpResponse &response, const ServerBlock *server, const LocationBlock *location)
{
    // Only handle GET requests
    if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
    {
        response.setStatus(405, "Method Not Allowed");
        response.setHeader("Allow", "GET, HEAD");
        response.setBody("Method Not Allowed");
        response.setContentType("text/plain");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Resolve file path
    std::string filePath = resolveFilePath(request, server, location);
    
    if (filePath.empty() || !fileExists(filePath))
    {
        std::cout << "[STATIC] File not found: " << filePath << std::endl;
        response.setStatus(404, "Not Found");
        response.setBody("<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested file was not found.</p></body></html>");
        response.setContentType("text/html");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Read file content
    std::string content = readFile(filePath);
    if (content.empty() && fileExists(filePath))
    {
        // File exists but is empty or unreadable
        std::cout << "[STATIC] File exists but cannot be read: " << filePath << std::endl;
        response.setStatus(403, "Forbidden");
        response.setBody("Forbidden");
        response.setContentType("text/plain");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Get MIME type
    std::string mimeType = getMimeType(filePath);
    
    // Build response
    response.setStatus(200, "OK");
    response.setContentType(mimeType);
    
    // For HEAD requests, don't include body
    if (request.getMethod() == "GET")
        response.setBody(content);
    
    response.setContentLength(content.size());
    response.build();
    
    std::cout << "[STATIC] Serving file: " << filePath << " (" << content.size() << " bytes, " << mimeType << ")" << std::endl;
    return true;
}

#include "utils/ValidateSemanticUtils.hpp"
#include <sstream>
#include <set>
#include <cctype>

ValidateSemanticUtils::ValidateSemanticUtils(void)
{
}

ValidateSemanticUtils::~ValidateSemanticUtils()
{
}

ValidateSemanticUtils::ValidateSemanticUtils(const ValidateSemanticUtils &ref)
{
    (void)ref;
}

ValidateSemanticUtils &ValidateSemanticUtils::operator=(const ValidateSemanticUtils &ref)
{
    (void)ref;
    return *this;
}

// Helper: Parse port string to integer and validate range
int ValidateSemanticUtils::parsePort(const std::string &portStr)
{
    if (portStr.empty())
        throw std::runtime_error("Empty port string");
    
    // Check all digits
    for (size_t i = 0; i < portStr.size(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(portStr[i])))
            throw std::runtime_error(std::string("Invalid port (non-digit): ") + portStr);
    }
    
    std::istringstream iss(portStr);
    int port;
    iss >> port;
    
    if (port < 1 || port > 65535)
    {
        std::ostringstream err;
        err << "Port out of range (1-65535): " << port;
        throw std::runtime_error(err.str());
    }
    
    return port;
}

// Helper: Check if method is valid
bool ValidateSemanticUtils::isValidMethod(const std::string &method)
{
    std::string m = method;
    // Convert to uppercase for comparison
    for (size_t i = 0; i < m.size(); ++i)
        m[i] = static_cast<char>(toupper(static_cast<unsigned char>(m[i])));
    
    return (m == "GET" || m == "POST" || m == "PUT" || 
            m == "DELETE" || m == "HEAD" || m == "OPTIONS" ||
            m == "PATCH" || m == "CONNECT" || m == "TRACE");
}

// Helper: Check location path syntax
bool ValidateSemanticUtils::checkPathSyntax(const std::string &path)
{
    if (path.empty())
        return false;
    
    // Must start with / or be a modifier (=, ~, ~*, ^~)
    if (path[0] == '/')
        return true;
    
    // Check for modifiers (nginx-style)
    if (path[0] == '=' || path[0] == '~' || path[0] == '^')
        return true;
    
    return false;
}

// Validate worker_processes
void ValidateSemanticUtils::validateWorkerProcesses(const Config &config)
{
    int wp = config.getWorkerProcesses();
    if (wp <= 0)
    {
        std::ostringstream err;
        err << "Invalid worker_processes: " << wp << " (must be > 0)";
        throw std::runtime_error(err.str());
    }
}

// Validate HTTP block exists
void ValidateSemanticUtils::validateHttpBlock(const Config &config)
{
    const HttpBlock &http = config.getHttpBlock();
    const std::vector<ServerBlock> &servers = http.getServerBlocks();
    
    if (servers.empty())
    {
        throw std::runtime_error("Configuration must have at least one server block in http");
    }
}

// Validate listen entries for a server
void ValidateSemanticUtils::validateListenEntries(const ServerBlock &server, size_t serverIndex)
{
    const std::vector<ListenAddr> &listens = server.getListenAddrs();
    
    if (listens.empty())
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << " has no listen directives";
        throw std::runtime_error(err.str());
    }
    
    for (size_t i = 0; i < listens.size(); ++i)
    {
        const ListenAddr &la = listens[i];
        
        // Validate port range
        if (la.port < 1 || la.port > 65535)
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " listen port out of range: " << la.port;
            throw std::runtime_error(err.str());
        }
        
        // Validate host is not empty
        if (la.host.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " listen host is empty";
            throw std::runtime_error(err.str());
        }
    }
}

// Validate location block
void ValidateSemanticUtils::validateLocationBlock(const LocationBlock &location, size_t serverIndex, size_t locIndex)
{
    // Check location path is not empty and has valid syntax
    const std::string &path = location.getPath();
    if (path.empty())
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << " location #" << locIndex << ": path is empty";
        throw std::runtime_error(err.str());
    }
    
    if (!checkPathSyntax(path))
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << " location #" << locIndex 
            << ": invalid path syntax '" << path << "'";
        throw std::runtime_error(err.str());
    }
    
    // Validate client_max_body_size
    size_t maxBody = location.getClientMaxBodySize();
    if (maxBody > MAX_CLIENT_MAX_BODY_SIZE)
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << " location '" << path 
            << "': client_max_body_size (" << maxBody << ") exceeds MAX (" 
            << MAX_CLIENT_MAX_BODY_SIZE << ")";
        throw std::runtime_error(err.str());
    }
    
    // Validate allowedMethods
    const std::vector<std::string> &methods = location.getAllowedMethods();
    for (size_t m = 0; m < methods.size(); ++m)
    {
        if (!isValidMethod(methods[m]))
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': invalid HTTP method '" << methods[m] << "'";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate error_page codes and paths
    const std::map<int, std::string> &errPages = location.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = errPages.begin(); 
         it != errPages.end(); ++it)
    {
        if (it->first < 100 || it->first > 599)
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': invalid error_page code " << it->first;
            throw std::runtime_error(err.str());
        }
        
        if (it->second.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': error_page code " << it->first << " has empty path";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate return directive if present
    if (location.getHasRedirect())
    {
        int code = location.getRedirectCode();
        const std::string &target = location.getRedirectTarget();
        
        if (code < 100 || code > 599)
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': invalid return code " << code;
            throw std::runtime_error(err.str());
        }
        
        if (target.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': return directive has empty target";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate upload_store if enabled
    if (location.isUploadEnabled())
    {
        const std::string &uploadStore = location.getUploadStore();
        if (uploadStore.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': upload enabled but upload_store is empty";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate cgi_pass if CGI extensions are set
    if (!location.getCgiExtensions().empty())
    {
        const std::string &cgiPath = location.getCgiPath();
        if (cgiPath.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << " location '" << path 
                << "': CGI extensions set but cgi_pass is empty";
            throw std::runtime_error(err.str());
        }
    }
}

// Validate server block
void ValidateSemanticUtils::validateServerBlock(const ServerBlock &server, size_t serverIndex)
{
    // Validate listen entries
    validateListenEntries(server, serverIndex);
    
    // Validate server_name or listen must exist (at least one identification)
    const std::vector<std::string> &names = server.getServerNames();
    const std::vector<ListenAddr> &listens = server.getListenAddrs();
    
    if (names.empty() && listens.empty())
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << " has neither server_name nor listen directives";
        throw std::runtime_error(err.str());
    }
    
    // Validate client_max_body_size
    size_t maxBody = server.getClientMaxBodySize();
    if (maxBody > MAX_CLIENT_MAX_BODY_SIZE)
    {
        std::ostringstream err;
        err << "Server #" << serverIndex << ": client_max_body_size (" 
            << maxBody << ") exceeds MAX (" << MAX_CLIENT_MAX_BODY_SIZE << ")";
        throw std::runtime_error(err.str());
    }
    
    // Validate allowedMethods
    const std::vector<std::string> &methods = server.getAllowedMethods();
    for (size_t m = 0; m < methods.size(); ++m)
    {
        if (!isValidMethod(methods[m]))
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << ": invalid HTTP method '" << methods[m] << "'";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate error_page codes and paths
    const std::map<int, std::string> &errPages = server.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = errPages.begin(); 
         it != errPages.end(); ++it)
    {
        if (it->first < 100 || it->first > 599)
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << ": invalid error_page code " << it->first;
            throw std::runtime_error(err.str());
        }
        
        if (it->second.empty())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << ": error_page code " 
                << it->first << " has empty path";
            throw std::runtime_error(err.str());
        }
    }
    
    // Validate each location
    const std::vector<LocationBlock> &locations = server.getLocationBlocks();
    std::set<std::string> seenPaths;
    
    for (size_t i = 0; i < locations.size(); ++i)
    {
        validateLocationBlock(locations[i], serverIndex, i);
        
        // Check for duplicate location paths
        const std::string &path = locations[i].getPath();
        if (seenPaths.find(path) != seenPaths.end())
        {
            std::ostringstream err;
            err << "Server #" << serverIndex << ": duplicate location path '" << path << "'";
            throw std::runtime_error(err.str());
        }
        seenPaths.insert(path);
    }
}

// Validate cross-bindings (duplicate listen/default_server conflicts)
void ValidateSemanticUtils::validateCrossBindings(const Config &config)
{
    const HttpBlock &http = config.getHttpBlock();
    const std::vector<ServerBlock> &servers = http.getServerBlocks();
    
    // Track host:port -> list of server indices with default_server flag
    std::map<std::string, std::vector<size_t> > defaultServers;
    
    for (size_t si = 0; si < servers.size(); ++si)
    {
        const std::vector<ListenAddr> &listens = servers[si].getListenAddrs();
        
        for (size_t li = 0; li < listens.size(); ++li)
        {
            const ListenAddr &la = listens[li];
            
            if (la.defaultServerFlag)
            {
                std::ostringstream key;
                key << la.host << ":" << la.port;
                defaultServers[key.str()].push_back(si);
            }
        }
    }
    
    // Check for multiple default_server on same host:port
    for (std::map<std::string, std::vector<size_t> >::const_iterator it = defaultServers.begin();
         it != defaultServers.end(); ++it)
    {
        if (it->second.size() > 1)
        {
            std::ostringstream err;
            err << "Multiple default_server for " << it->first << " (servers: ";
            for (size_t i = 0; i < it->second.size(); ++i)
            {
                if (i > 0) err << ", ";
                err << "#" << it->second[i];
            }
            err << ")";
            throw std::runtime_error(err.str());
        }
    }
}

// Main validation entry point
bool ValidateSemanticUtils::ValidateSemantic(Config &config)
{
    try
    {
        // Priority A validations
        validateWorkerProcesses(config);
        validateHttpBlock(config);
        
        const HttpBlock &http = config.getHttpBlock();
        const std::vector<ServerBlock> &servers = http.getServerBlocks();
        
        // Validate each server block
        for (size_t i = 0; i < servers.size(); ++i)
        {
            validateServerBlock(servers[i], i);
        }
        
        // Priority B: Cross-binding validations
        validateCrossBindings(config);
        
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Semantic validation error: " << e.what() << std::endl;
        return false;
    }
}


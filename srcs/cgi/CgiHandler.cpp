#include "../../includes/cgi/CgiHandler.hpp"

CgiHandler::CgiHandler(const HttpRequest &req, const ServerBlock &server, const LocationBlock *location)
    : request(req), serverBlock(server), locationBlock(location), childPid(-1)
{
    pipeFd[0] = -1;
    pipeFd[1] = -1;
}

CgiHandler::~CgiHandler(void)
{
    if (pipeFd[0] >= 0)
        close(pipeFd[0]);
    if (pipeFd[1] >= 0)
        close(pipeFd[1]);
}

CgiHandler::CgiHandler(const CgiHandler &ref)
    : request(ref.request), serverBlock(ref.serverBlock), locationBlock(ref.locationBlock),
      scriptPath(ref.scriptPath), cgiExecutable(ref.cgiExecutable), envVars(ref.envVars),
      childPid(ref.childPid), cgiOutput(ref.cgiOutput)
{
    pipeFd[0] = ref.pipeFd[0];
    pipeFd[1] = ref.pipeFd[1];
}

CgiHandler &CgiHandler::operator=(const CgiHandler &ref)
{
    if (this != &ref)
    {
        // Note: Can't reassign const references, but we can copy other members
        scriptPath = ref.scriptPath;
        cgiExecutable = ref.cgiExecutable;
        envVars = ref.envVars;
        pipeFd[0] = ref.pipeFd[0];
        pipeFd[1] = ref.pipeFd[1];
        childPid = ref.childPid;
        cgiOutput = ref.cgiOutput;
    }
    return *this;
}

// Check if this is a CGI request
bool CgiHandler::isCgiRequest(void) const
{
    if (!locationBlock)
        return false;
    
    if (!locationBlock->hasCgi())
        return false;
    
    // Check if file extension matches CGI extensions
    std::string ext = getFileExtension(request.getPath());
    const std::vector<std::string> &cgiExts = locationBlock->getCgiExtensions();
    
    for (size_t i = 0; i < cgiExts.size(); ++i)
    {
        if (cgiExts[i] == ext)
            return true;
    }
    
    return false;
}

// Get file extension from path
std::string CgiHandler::getFileExtension(const std::string &path) const
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < path.length() - 1)
    {
        return path.substr(dotPos); // Include the dot: ".py", ".php"
    }
    return "";
}

// Validate script path
bool CgiHandler::validateScriptPath(void)
{
    if (!locationBlock)
        return false;
    
    // Build full script path: root + request path
    std::string root = locationBlock->getRoot();
    if (root.empty())
        root = serverBlock.getRoot();
    
    scriptPath = root + request.getPath();
    
    // Check if file exists and is executable
    struct stat st;
    if (stat(scriptPath.c_str(), &st) != 0)
    {
        return false;
    }
    
    // Check if it's a regular file
    if (!S_ISREG(st.st_mode))
    {
        return false;
    }
    
    // Get CGI executable (interpreter)
    cgiExecutable = locationBlock->getCgiPath();
    if (cgiExecutable.empty())
    {
        // Try to use script as executable directly
        if (!(st.st_mode & S_IXUSR))
        {
            return false;
        }
        cgiExecutable = scriptPath;
    }
    
    return true;
}

// Setup CGI environment variables (RFC 3CGI)
void CgiHandler::setupEnvironment(void)
{
    // Meta-variables
    setEnvVar("REQUEST_METHOD", request.getMethod());
    setEnvVar("SCRIPT_NAME", request.getPath());
    setEnvVar("SCRIPT_FILENAME", scriptPath);
    setEnvVar("QUERY_STRING", request.getQuery());
    setEnvVar("SERVER_PROTOCOL", request.getHttpVersion());
    
    // Server information
    std::ostringstream portStr;
    portStr << serverBlock.getListenAddrs()[0].port;
    setEnvVar("SERVER_PORT", portStr.str());
    
    if (!serverBlock.getServerNames().empty())
        setEnvVar("SERVER_NAME", serverBlock.getServerNames()[0]);
    
    // Content metadata
    if (request.hasHeader("content-type"))
        setEnvVar("CONTENT_TYPE", request.getHeader("content-type"));
    
    if (request.hasHeader("content-length"))
        setEnvVar("CONTENT_LENGTH", request.getHeader("content-length"));
    
    // HTTP headers as HTTP_* variables
    const std::map<std::string, std::string> &headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it)
    {
        std::string envName = "HTTP_";
        std::string headerName = it->first;
        
        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < headerName.length(); ++i)
        {
            char c = headerName[i];
            if (c == '-')
                envName += '_';
            else
                envName += std::toupper(static_cast<unsigned char>(c));
        }
        
        setEnvVar(envName, it->second);
    }
    
    // Path info (simplified)
    setEnvVar("PATH_INFO", "");
    setEnvVar("PATH_TRANSLATED", "");
    
    // Authentication (if implemented)
    setEnvVar("REMOTE_ADDR", "127.0.0.1");  // TODO: Get actual client IP
    setEnvVar("REMOTE_HOST", "");
    setEnvVar("REMOTE_IDENT", "");
    setEnvVar("REMOTE_USER", "");
    setEnvVar("AUTH_TYPE", "");
}

void CgiHandler::setEnvVar(const std::string &name, const std::string &value)
{
    envVars[name] = value;
}

// Execute CGI script
bool CgiHandler::execute(void)
{
    if (!isCgiRequest())
        return false;
    
    if (!validateScriptPath())
        return false;
    
    setupEnvironment();
    
    // Create pipe for CGI output
    if (pipe(pipeFd) < 0)
    {
        return false;
    }
    
    // Fork child process
    childPid = fork();
    
    if (childPid < 0)
    {
        // Fork failed
        close(pipeFd[0]);
        close(pipeFd[1]);
        return false;
    }
    
    if (childPid == 0)
    {
        // Child process
        
        // Redirect stdout to pipe
        dup2(pipeFd[1], STDOUT_FILENO);
        close(pipeFd[0]);
        close(pipeFd[1]);
        
        // Setup stdin for POST body
        if (request.getMethod() == "POST" && !request.getBody().empty())
        {
            // TODO: Implement stdin pipe for POST body
        }
        
        // Convert environment variables to char* array
        std::vector<std::string> envStrings;
        std::vector<char*> envArray;
        
        for (std::map<std::string, std::string>::const_iterator it = envVars.begin();
             it != envVars.end(); ++it)
        {
            std::string envStr = it->first + "=" + it->second;
            envStrings.push_back(envStr);
        }
        
        for (size_t i = 0; i < envStrings.size(); ++i)
        {
            envArray.push_back(const_cast<char*>(envStrings[i].c_str()));
        }
        envArray.push_back(NULL);
        
        // Prepare arguments
        char *args[3];
        args[0] = const_cast<char*>(cgiExecutable.c_str());
        args[1] = const_cast<char*>(scriptPath.c_str());
        args[2] = NULL;
        
        // Execute CGI script
        execve(cgiExecutable.c_str(), args, &envArray[0]);
        
        // If execve fails
        std::cerr << "CGI execve failed: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Parent process
    close(pipeFd[1]);  // Close write end
    pipeFd[1] = -1;
    
    // Read CGI output (non-blocking would be better for production)
    char buffer[4096];
    ssize_t n;
    
    while ((n = read(pipeFd[0], buffer, sizeof(buffer))) > 0)
    {
        cgiOutput.append(buffer, n);
    }
    
    close(pipeFd[0]);
    pipeFd[0] = -1;
    
    // Wait for child to finish
    int status;
    waitpid(childPid, &status, 0);
    
    return true;
}

// Get CGI output
const std::string &CgiHandler::getOutput(void) const
{
    return cgiOutput;
}

// Build HTTP response from CGI output
void CgiHandler::buildResponse(HttpResponse &response)
{
    if (cgiOutput.empty())
    {
        response.setStatus(500, "Internal Server Error");
        response.setBody("CGI script produced no output");
        response.setContentType("text/plain");
        response.setContentLength(response.getBody().size());
        return;
    }
    
    // Parse CGI output: headers + body
    size_t headerEndPos = cgiOutput.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
    {
        headerEndPos = cgiOutput.find("\n\n");
        if (headerEndPos != std::string::npos)
            headerEndPos += 2;
    }
    else
    {
        headerEndPos += 4;
    }
    
    if (headerEndPos == std::string::npos)
    {
        // No headers, treat all as body
        response.setStatus(200);
        response.setBody(cgiOutput);
        response.setContentType("text/html");
        response.setContentLength(cgiOutput.size());
        return;
    }
    
    // Extract headers
    std::string headersStr = cgiOutput.substr(0, headerEndPos);
    std::string body = cgiOutput.substr(headerEndPos);
    
    // Parse CGI headers
    std::istringstream headerStream(headersStr);
    std::string line;
    bool hasStatus = false;
    
    while (std::getline(headerStream, line))
    {
        if (line.empty() || line == "\r")
            continue;
        
        // Remove trailing \r
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string name = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            size_t start = value.find_first_not_of(" \t");
            if (start != std::string::npos)
                value = value.substr(start);
            
            // Check for Status header
            if (name == "Status")
            {
                int code;
                std::istringstream(value) >> code;
                response.setStatus(code);
                hasStatus = true;
            }
            else
            {
                response.addHeader(name, value);
            }
        }
    }
    
    if (!hasStatus)
        response.setStatus(200);
    
    response.setBody(body);
    
    // Set Content-Length if not already set by CGI
    if (!response.hasHeader("Content-Length"))
        response.setContentLength(body.size());
}

bool CgiHandler::isComplete(void) const
{
    return childPid >= 0 && !cgiOutput.empty();
}

int CgiHandler::getExitStatus(void) const
{
    // TODO: Store exit status from waitpid
    return 0;
}

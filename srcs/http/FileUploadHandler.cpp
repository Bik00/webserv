#include "../../includes/http/FileUploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

FileUploadHandler::FileUploadHandler(void)
    : uploadDir("./uploads"), maxFileSize(10 * 1024 * 1024) // 10MB default
{
}

FileUploadHandler::FileUploadHandler(const std::string &uploadDirectory)
    : uploadDir(uploadDirectory), maxFileSize(10 * 1024 * 1024)
{
}

FileUploadHandler::~FileUploadHandler(void)
{
}

void FileUploadHandler::setUploadDir(const std::string &dir)
{
    uploadDir = dir;
}

void FileUploadHandler::setMaxFileSize(size_t size)
{
    maxFileSize = size;
}

const std::vector<UploadedFile> &FileUploadHandler::getUploadedFiles(void) const
{
    return files;
}

// Parse boundary from Content-Type header
bool FileUploadHandler::parseBoundary(const std::string &contentType)
{
    // Content-Type: multipart/form-data; boundary=----WebKitFormBoundary...
    size_t boundaryPos = contentType.find("boundary=");
    if (boundaryPos == std::string::npos)
        return false;
    
    boundary = contentType.substr(boundaryPos + 9);
    
    // Remove quotes if present
    if (!boundary.empty() && boundary[0] == '"')
    {
        size_t endQuote = boundary.find('"', 1);
        if (endQuote != std::string::npos)
            boundary = boundary.substr(1, endQuote - 1);
    }
    
    return !boundary.empty();
}

// Sanitize filename to prevent directory traversal
std::string FileUploadHandler::sanitizeFilename(const std::string &filename)
{
    std::string sanitized;
    
    // Remove path components
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        sanitized = filename.substr(lastSlash + 1);
    else
        sanitized = filename;
    
    // Remove dangerous characters
    std::string safe;
    for (size_t i = 0; i < sanitized.length(); ++i)
    {
        char c = sanitized[i];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '_' || c == '-')
            safe += c;
        else
            safe += '_';
    }
    
    // Add timestamp to make unique
    std::ostringstream oss;
    oss << std::time(NULL) << "_" << safe;
    
    return oss.str();
}

// Parse part headers (Content-Disposition, Content-Type, etc.)
bool FileUploadHandler::parsePartHeaders(const std::string &headers, UploadedFile &file)
{
    std::istringstream stream(headers);
    std::string line;
    
    while (std::getline(stream, line))
    {
        if (line.empty() || line == "\r")
            continue;
        
        // Remove trailing \r
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);
        
        // Content-Disposition: form-data; name="file"; filename="test.jpg"
        if (line.find("Content-Disposition:") == 0)
        {
            size_t namePos = line.find("name=\"");
            if (namePos != std::string::npos)
            {
                namePos += 6;
                size_t nameEnd = line.find("\"", namePos);
                if (nameEnd != std::string::npos)
                    file.fieldName = line.substr(namePos, nameEnd - namePos);
            }
            
            size_t filenamePos = line.find("filename=\"");
            if (filenamePos != std::string::npos)
            {
                filenamePos += 10;
                size_t filenameEnd = line.find("\"", filenamePos);
                if (filenameEnd != std::string::npos)
                    file.filename = line.substr(filenamePos, filenameEnd - filenamePos);
            }
        }
        // Content-Type: image/jpeg
        else if (line.find("Content-Type:") == 0)
        {
            size_t valuePos = line.find(":");
            if (valuePos != std::string::npos)
            {
                std::string value = line.substr(valuePos + 1);
                // Trim whitespace
                size_t start = value.find_first_not_of(" \t");
                if (start != std::string::npos)
                    file.contentType = value.substr(start);
            }
        }
    }
    
    return !file.filename.empty();
}

// Parse multipart/form-data body
bool FileUploadHandler::parseMultipartBody(const std::string &body)
{
    std::string delimiter = "--" + boundary;
    std::string endDelimiter = "--" + boundary + "--";
    
    size_t pos = 0;
    
    while (pos < body.length())
    {
        // Find next boundary
        size_t boundaryStart = body.find(delimiter, pos);
        if (boundaryStart == std::string::npos)
            break;
        
        // Skip past boundary
        boundaryStart += delimiter.length();
        
        // Check if this is the end delimiter
        if (body.substr(boundaryStart, 2) == "--")
            break;
        
        // Skip CRLF after boundary
        if (body.substr(boundaryStart, 2) == "\r\n")
            boundaryStart += 2;
        else if (body[boundaryStart] == '\n')
            boundaryStart += 1;
        
        // Find headers end (empty line)
        size_t headersEnd = body.find("\r\n\r\n", boundaryStart);
        if (headersEnd == std::string::npos)
            headersEnd = body.find("\n\n", boundaryStart);
        
        if (headersEnd == std::string::npos)
            break;
        
        // Extract headers
        std::string headers = body.substr(boundaryStart, headersEnd - boundaryStart);
        
        // Skip headers separator
        size_t dataStart = headersEnd;
        if (body.substr(headersEnd, 4) == "\r\n\r\n")
            dataStart += 4;
        else if (body.substr(headersEnd, 2) == "\n\n")
            dataStart += 2;
        
        // Find next boundary
        size_t dataEnd = body.find(delimiter, dataStart);
        if (dataEnd == std::string::npos)
            break;
        
        // Remove trailing CRLF before boundary
        if (dataEnd >= 2 && body.substr(dataEnd - 2, 2) == "\r\n")
            dataEnd -= 2;
        else if (dataEnd >= 1 && body[dataEnd - 1] == '\n')
            dataEnd -= 1;
        
        // Extract data
        std::string data = body.substr(dataStart, dataEnd - dataStart);
        
        // Parse headers and create file
        UploadedFile file;
        if (parsePartHeaders(headers, file))
        {
            file.data = data;
            file.size = data.length();
            
            if (file.size > maxFileSize)
            {
                std::cerr << "File too large: " << file.filename 
                          << " (" << file.size << " bytes)" << std::endl;
            }
            else
            {
                files.push_back(file);
            }
        }
        
        pos = dataEnd;
    }
    
    return !files.empty();
}

// Save file to disk
bool FileUploadHandler::saveFile(const UploadedFile &file, std::string &savedPath)
{
    // Create upload directory if it doesn't exist
    mkdir(uploadDir.c_str(), 0755);
    
    // Generate safe filename
    std::string safeFilename = sanitizeFilename(file.filename);
    savedPath = uploadDir + "/" + safeFilename;
    
    // Write file
    std::ofstream ofs(savedPath.c_str(), std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Failed to open file for writing: " << savedPath << std::endl;
        return false;
    }
    
    ofs.write(file.data.c_str(), file.data.length());
    ofs.close();
    
    std::cout << "Saved file: " << savedPath << " (" << file.size << " bytes)" << std::endl;
    return true;
}

// Main upload handler
bool FileUploadHandler::handleUpload(const HttpRequest &request, HttpResponse &response)
{
    // Check if this is a POST request
    if (request.getMethod() != "POST")
    {
        response.setStatus(405, "Method Not Allowed");
        response.setBody("Only POST method is allowed for uploads");
        response.setContentType("text/plain");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Get Content-Type header
    std::string contentType = request.getHeader("content-type");
    if (contentType.empty())
    {
        response.setStatus(400, "Bad Request");
        response.setBody("Missing Content-Type header");
        response.setContentType("text/plain");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Check if multipart/form-data
    if (contentType.find("multipart/form-data") == std::string::npos)
    {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"success\":false,\"error\":\"Content-Type must be multipart/form-data\"}");
        response.setContentType("application/json");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Parse boundary
    if (!parseBoundary(contentType))
    {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"success\":false,\"error\":\"Missing or invalid boundary in Content-Type\"}");
        response.setContentType("application/json");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Parse multipart body
    const std::string &body = request.getBody();
    if (body.empty())
    {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"success\":false,\"error\":\"Empty request body\"}");
        response.setContentType("application/json");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    if (!parseMultipartBody(body))
    {
        response.setStatus(400, "Bad Request");
        response.setBody("{\"success\":false,\"error\":\"Failed to parse multipart body or no files found\"}");
        response.setContentType("application/json");
        response.setContentLength(response.getBody().size());
        response.build();
        return false;
    }
    
    // Save all files
    std::vector<std::string> savedPaths;
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string savedPath;
        if (saveFile(files[i], savedPath))
            savedPaths.push_back(savedPath);
    }
    
    // Build JSON success response
    std::ostringstream bodyOss;
    bodyOss << "{\"success\":true,\"count\":" << savedPaths.size() << ",\"files\":[";
    for (size_t i = 0; i < savedPaths.size(); ++i)
    {
        if (i > 0)
            bodyOss << ",";
        bodyOss << "{\"path\":\"" << savedPaths[i] << "\",\"size\":" << files[i].size << "}";
    }
    bodyOss << "]}";
    
    response.setStatus(200, "OK");
    response.setBody(bodyOss.str());
    response.setContentType("application/json");
    response.setContentLength(response.getBody().size());
    response.build();
    
    return true;
}

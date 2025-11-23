#ifndef FILE_UPLOAD_HANDLER_HPP
# define FILE_UPLOAD_HANDLER_HPP

# include "../libs/Libs.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

// Uploaded file information
struct UploadedFile
{
    std::string fieldName;      // Form field name
    std::string filename;       // Original filename
    std::string contentType;    // Content-Type of file
    std::string data;           // File content
    size_t size;                // File size in bytes
    
    UploadedFile() : size(0) {}
};

class FileUploadHandler
{
private:
    std::string boundary;
    std::vector<UploadedFile> files;
    std::string uploadDir;
    size_t maxFileSize;
    
    // Parsing helpers
    bool parseBoundary(const std::string &contentType);
    bool parseMultipartBody(const std::string &body);
    bool parsePartHeaders(const std::string &headers, UploadedFile &file);
    bool saveFile(const UploadedFile &file, std::string &savedPath);
    std::string sanitizeFilename(const std::string &filename);

public:
    FileUploadHandler(void);
    FileUploadHandler(const std::string &uploadDirectory);
    ~FileUploadHandler(void);
    
    // Process upload
    bool handleUpload(const HttpRequest &request, HttpResponse &response);
    
    // Getters
    const std::vector<UploadedFile> &getUploadedFiles(void) const;
    void setUploadDir(const std::string &dir);
    void setMaxFileSize(size_t size);
};

#endif /* FILE_UPLOAD_HANDLER_HPP */

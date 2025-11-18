#ifndef BASE_BLOCK_HPP
# define BASE_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include <string>
# include <vector>
# include <map>

class BaseBlock
{
protected:
    std::string root;
    std::vector<std::string> indexFiles;
    std::map<int, std::string> errorPages;
    size_t clientMaxBodySize;
    bool autoindex;
    // If empty, all methods are allowed; otherwise only the listed methods are allowed
    std::vector<std::string> allowedMethods;

public:
    BaseBlock(void);
    virtual ~BaseBlock(void);
    BaseBlock(const BaseBlock &ref);
    BaseBlock &operator=(const BaseBlock &ref);

    void addIndexFile(const std::string &f);
    void addErrorPage(int code, const std::string &path);
    void setRoot(const std::string &r);
    void setIndexFiles(const std::vector<std::string> &files);
    void setClientMaxBodySize(size_t size);
    void setAutoindex(bool on);
    void setAllowedMethods(const std::vector<std::string> &methods);
    void addAllowedMethod(const std::string &method);

    const std::map<int, std::string> &getErrorPages() const;
    const std::vector<std::string> &getIndexFiles() const;
    const std::string &getRoot() const;
    size_t getClientMaxBodySize() const;
    bool getAutoindex() const;
    const std::vector<std::string> &getAllowedMethods() const;
};

#endif /* BASE_BLOCK_HPP */

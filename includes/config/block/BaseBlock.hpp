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

    const std::map<int, std::string> &getErrorPages() const;
    const std::vector<std::string> &getIndexFiles() const;
    const std::string &getRoot() const;
    size_t getClientMaxBodySize() const;
    bool getAutoindex() const;
};

#endif /* BASE_BLOCK_HPP */

#include "../../../includes/config/block/BaseBlock.hpp"

BaseBlock::BaseBlock(void)
{
    root = std::string(DEFAULT_ROOT);
    indexFiles.clear();
    indexFiles.push_back(std::string(DEFAULT_INDEX_FILE));
    errorPages.clear();
    clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;
    autoindex = false;
    allowedMethods.clear();
}

BaseBlock::~BaseBlock(void)
{
}

BaseBlock::BaseBlock(const BaseBlock &ref)
{
    *this = ref;
}

BaseBlock &BaseBlock::operator=(const BaseBlock &ref)
{
    if (this != &ref)
    {
        this->root = ref.root;
        this->indexFiles = ref.indexFiles;
        this->errorPages = ref.errorPages;
        this->clientMaxBodySize = ref.clientMaxBodySize;
        this->autoindex = ref.autoindex;
    this->allowedMethods = ref.allowedMethods;
    }
    return *this;
}

void BaseBlock::addIndexFile(const std::string &f)
{
    this->indexFiles.push_back(f);
}

void BaseBlock::addErrorPage(int code, const std::string &path)
{
    this->errorPages[code] = path;
}

void BaseBlock::setRoot(const std::string &r)
{
    this->root = r;
}

void BaseBlock::setIndexFiles(const std::vector<std::string> &files)
{
    this->indexFiles = files;
}

void BaseBlock::setClientMaxBodySize(size_t size)
{
    this->clientMaxBodySize = size;
}

void BaseBlock::setAutoindex(bool on)
{
    this->autoindex = on;
}

void BaseBlock::setAllowedMethods(const std::vector<std::string> &methods)
{
    this->allowedMethods = methods;
}

void BaseBlock::addAllowedMethod(const std::string &method)
{
    this->allowedMethods.push_back(method);
}

const std::map<int, std::string> &BaseBlock::getErrorPages() const
{
    return this->errorPages;
}

const std::vector<std::string> &BaseBlock::getIndexFiles() const
{
    return this->indexFiles;
}

const std::string &BaseBlock::getRoot() const
{
    return this->root;
}

size_t BaseBlock::getClientMaxBodySize() const
{
    return this->clientMaxBodySize;
}

bool BaseBlock::getAutoindex() const
{
    return this->autoindex;
}

const std::vector<std::string> &BaseBlock::getAllowedMethods() const
{
    return this->allowedMethods;
}

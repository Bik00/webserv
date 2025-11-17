#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "../../libs/Libs.hpp"

class LocationBlock
{
private:
    std::string                 path;
    std::vector<std::string>    methods;
    bool                        hasRedirect;
    int                         redirectCode;
    std::string                 redirectTarget;
    std::string                 root;
    bool                        autoindex;
    std::vector<std::string>    indexFiles;
    std::vector<std::string>    cgiExtensions;
    std::string                 cgiPath;
    bool                        uploadEnable;
    std::string                 uploadStore;
    size_t                      clientMaxBodySize;
public:
    LocationBlock(void);
    ~LocationBlock(void);
    LocationBlock(const LocationBlock &ref);
    LocationBlock &operator=(const LocationBlock &ref);
};

#endif /* LOCATION_BLOCK_HPP */
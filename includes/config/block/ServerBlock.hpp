#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./LocationBlock.hpp"

class ServerBlock
{
private:
    std::vector<LocationBlock>  locationBlocks;
public:
    ServerBlock(void);
    ~ServerBlock(void);
    ServerBlock(const ServerBlock &ref);
    ServerBlock &operator=(const ServerBlock &ref);
    void addLocationBlock(const LocationBlock &lb);
};

#endif /* SERVER_BLOCK_HPP */
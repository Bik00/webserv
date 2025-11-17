#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./ServerBlock.hpp"

class HttpBlock
{
private:
    std::vector<ServerBlock>  serverBlocks;
public:
    HttpBlock(void);
    ~HttpBlock(void);
    HttpBlock(const HttpBlock &ref);
    HttpBlock &operator=(const HttpBlock &ref);
    void addServerBlock(const ServerBlock &sb);
};

#endif /* HTTP_BLOCK_HPP */
#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./ServerBlock.hpp"
# include "./BaseBlock.hpp"

class HttpBlock : public BaseBlock
{
private:
    std::vector<ServerBlock>  serverBlocks;
public:
    HttpBlock(void);
    ~HttpBlock(void);
    HttpBlock(const HttpBlock &ref);
    HttpBlock &operator=(const HttpBlock &ref);
    bool addServerBlock(const ServerBlock &sb);
    const std::vector<ServerBlock> &getServerBlocks() const;
    // BaseBlock provides addErrorPage/getErrorPages
};

#endif /* HTTP_BLOCK_HPP */
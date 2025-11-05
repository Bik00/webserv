#ifndef BLOCK_PARSE_UTILS_HPP
# define BLOCK_PARSE_UTILS_HPP

# include "../parse/ConfigParser.hpp"

class BlockParseUtils
{
public:
    BlockParseUtils(void);
    ~BlockParseUtils(void);
    BlockParseUtils(const BlockParseUtils &ref);
    BlockParseUtils &operator=(const BlockParseUtils &ref);
};

#endif /* BLOCK_PARSE_UTILS_HPP */
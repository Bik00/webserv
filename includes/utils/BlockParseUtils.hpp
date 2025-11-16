#ifndef BLOCK_PARSE_UTILS_HPP
# define BLOCK_PARSE_UTILS_HPP

# include "../parse/ConfigParser.hpp"

class BlockParseUtils
{
private:
    bool    checkBlockContext(std::string key, std::ifstream &ifs);
public:
    BlockParseUtils(void);
    ~BlockParseUtils(void);
    BlockParseUtils(const BlockParseUtils &ref);
    BlockParseUtils &operator=(const BlockParseUtils &ref);

    bool    CheckHttpBlock(char **argv);
    bool    CheckEventBlock(char **argv);
};

#endif /* BLOCK_PARSE_UTILS_HPP */
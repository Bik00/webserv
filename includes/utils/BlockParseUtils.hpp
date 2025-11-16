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

    bool    CheckHttpBlock(char **argv, Config &config);
    bool    CheckEventBlock(char **argv, Config &config);
};

#endif /* BLOCK_PARSE_UTILS_HPP */
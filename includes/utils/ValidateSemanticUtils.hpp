#ifndef VALIDATE_SEMANTIC_UTILS_HPP
# define VALIDATE_SEMANTIC_UTILS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class ValidateSemanticUtils
{
public:
    ValidateSemanticUtils(void);
    ~ValidateSemanticUtils();
    ValidateSemanticUtils(const ValidateSemanticUtils &ref);
    ValidateSemanticUtils &operator=(const ValidateSemanticUtils &ref);

    bool ValidateSemantic(Config &config);
};

#endif /* VALIDATE_SEMANTIC_UTILS_HPP */
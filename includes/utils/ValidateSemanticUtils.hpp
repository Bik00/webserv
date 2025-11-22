#ifndef VALIDATE_SEMANTIC_UTILS_HPP
# define VALIDATE_SEMANTIC_UTILS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class ValidateSemanticUtils
{
private:
    // Helper functions
    int parsePort(const std::string &portStr);
    bool isValidMethod(const std::string &method);
    bool checkPathSyntax(const std::string &path);
    
    // Validation functions
    void validateHttpBlock(const Config &config);
    void validateServerBlock(const ServerBlock &server, size_t serverIndex);
    void validateLocationBlock(const LocationBlock &location, size_t serverIndex, size_t locIndex);
    void validateListenEntries(const ServerBlock &server, size_t serverIndex);
    void validateCrossBindings(const Config &config);
    void validateErrorPageConflicts(const ServerBlock &server, size_t serverIndex);
    void validateListenConflicts(const Config &config);
    void validateVirtualHosting(const Config &config);

public:
    ValidateSemanticUtils(void);
    ~ValidateSemanticUtils();
    ValidateSemanticUtils(const ValidateSemanticUtils &ref);
    ValidateSemanticUtils &operator=(const ValidateSemanticUtils &ref);

    bool ValidateSemantic(Config &config);
};

#endif /* VALIDATE_SEMANTIC_UTILS_HPP */
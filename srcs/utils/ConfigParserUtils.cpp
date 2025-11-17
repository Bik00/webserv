#include "../includes/utils/ConfigParserUtils.hpp"

ConfigParserUtils::ConfigParserUtils(void)
{
}

ConfigParserUtils::~ConfigParserUtils(void)
{
}

ConfigParserUtils::ConfigParserUtils(const ConfigParserUtils &ref)
{
}

ConfigParserUtils &ConfigParserUtils::operator=(const ConfigParserUtils &ref)
{
    return *this;
}

std::string ConfigParserUtils::CheckArgc(int argc, char **argv)
{
    std::string configPath;

    if (argc > 2)
    {
        throw std::runtime_error("Too many arguments. Usage: ./webserv <config_file>");
    }
    else if (argc == 1)
    {
        configPath = DEFAULT_CONFIG_PATH;
    }
    else
    {
        configPath = argv[1];
    }

    return (configPath);
}

bool ConfigParserUtils::ValidatePath(const std::string &configPath)
{
    // 1. `configPath`를 '/'로 구분하여 각 구성 요소(디렉토리 이름 또는 파일명)로 분리합니다.
    std::vector<std::string> comps;
    size_t start = 0;
    // 선행 슬래시('/') 처리: 경로가 절대경로(앞에 '/')이면 그 사실을 반영하여 분해를 시작합니다.
    bool absolute = (!configPath.empty() && configPath[0] == '/');
    if (absolute) start = 1;
    size_t pos = start;
    while (pos <= configPath.size())
    {
        size_t slash = configPath.find('/', pos);
        if (slash == std::string::npos) slash = configPath.size();
        std::string token = configPath.substr(pos, slash - pos);
        if (!token.empty()) comps.push_back(token);
        pos = slash + 1;
    }

    if (comps.empty())
    {
        std::cerr << "Invalid config path: " << configPath << std::endl;
        return false;
    }

    // 2. 마지막 요소(파일명)를 제외한 각 구성 요소들을 차례로 조합하여 해당 경로가 실제 디렉토리인지 `stat`으로 확인합니다.
    std::string cur;
    if (absolute) cur = "/";
    for (size_t i = 0; i + 1 < comps.size(); ++i)
    {
        if (cur == "/") cur += comps[i];
        else if (cur.empty()) cur = comps[i];
        else cur += std::string("/") + comps[i];

        struct stat st;
        if (stat(cur.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
        {
            std::cerr << "Directory not found: " << cur << std::endl;
            return false;
        }
    }

    // 3. 마지막 구성 요소가 `.conf` 확장자를 가진 파일인지, 그리고 그 파일이 실제로 존재하는지 검사합니다.
    std::string filePath;
    if (absolute) filePath = "/" + comps[0];
    else filePath = comps[0];
    for (size_t i = 1; i < comps.size(); ++i) filePath += std::string("/") + comps[i];

    // 4. 마지막 구성 요소의 이름이 반드시 `.conf`로 끝나는지 확인합니다.
    std::string last = comps.back();
    if (last.size() < 5 || last.substr(last.size() - 5) != ".conf")
    {
        std::cerr << "Config file must end with .conf: " << last << std::endl;
        return false;
    }

    struct stat fst;
    if (stat(filePath.c_str(), &fst) != 0 || !S_ISREG(fst.st_mode))
    {
        std::cerr << "Config file not found: " << filePath << std::endl;
        return false;
    }
    return true;
}
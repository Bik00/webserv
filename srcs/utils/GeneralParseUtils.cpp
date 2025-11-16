# include "../../includes/utils/GeneralParseUtils.hpp"

GeneralParseUtils::GeneralParseUtils(void)
{
}

GeneralParseUtils::~GeneralParseUtils(void)
{
}

GeneralParseUtils::GeneralParseUtils(const GeneralParseUtils &ref)
{
    (void)ref;
}

GeneralParseUtils &GeneralParseUtils::operator=(const GeneralParseUtils &ref)
{
    (void)ref;
    return (*this);
}

std::string GeneralParseUtils::CheckArgc(int argc, char **argv)
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

bool GeneralParseUtils::ValidatePath(const std::string &configPath)
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

bool GeneralParseUtils::CheckSimpleDirectives(const std::string &configPath)
{
    // 파일 읽어들이기 및 simple directive 검사
    std::ifstream ifs(configPath.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << configPath << std::endl;
        return false;
    }

    std::string line;
    size_t lineno = 0;
    // 블록 내부 내용은 검사에서 제외합니다. 중첩 깊이를 추적합니다.
    // depth == 0일 때만 simple directive 검사를 수행합니다.
    int skipDepth = 0;
    // 전역(파일) 스코프에서 사용된 키를 추적
    std::set<std::string> globalKeys;

    while (std::getline(ifs, line))
    {
        ++lineno;
        // trim leading/trailing whitespace
        size_t a = 0;
        while (a < line.size() && isspace(static_cast<unsigned char>(line[a]))) ++a;
        size_t b = line.size();
        while (b > a && isspace(static_cast<unsigned char>(line[b-1]))) --b;
        if (a >= b) continue; // empty line
        std::string s = line.substr(a, b - a);
        // skip comments
        if (s.size() >= 1 && s[0] == '#') continue;
    // 단독 줄의 '{' 또는 '}'는 중첩 깊이를 조정합니다.
        if (s == "{")
        {
            ++skipDepth;
            continue;
        }
        if (s == "}")
        {
            if (skipDepth <= 0)
            {
                std::cerr << "Unmatched '}' at line " << lineno << std::endl;
                return false;
            }
            --skipDepth;
            continue;
        }

        // 블록 내부이면 해당 라인을 검사하지 않고 건너뜁니다.
        if (skipDepth > 0) continue;

        // 인라인 주석('#'로 시작)을 허용하되, 따옴표 안의 '#'은 주석으로 처리하지 않습니다.
        // 또한 지시문 끝을 표시하는 세미콜론은 따옴표 밖에 있어야 합니다.
        bool in_quote = false;
        for (size_t idx = 0; idx < s.size(); ++idx)
        {
            char c = s[idx];
            if (c == '"') in_quote = !in_quote;
            if (!in_quote && c == '#')
            {
                // strip inline comment (drop '#' and everything after)
                s.erase(idx);
                break;
            }
        }

    // 주석을 제거한 뒤 다시 트리밍합니다
        size_t i = 0;
        while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t j = s.size();
        while (j > i && isspace(static_cast<unsigned char>(s[j-1]))) --j;
        if (i >= j) continue; // line became empty after comment

    // 지시문이 따옴표 밖에서 세미콜론으로 끝나는지 확인합니다
        in_quote = false;
        bool semicolon_found = false;
        for (size_t idx = i; idx < j; ++idx)
        {
            char c = s[idx];
            if (c == '"') in_quote = !in_quote;
            if (!in_quote && c == ';')
            {
                // 세미콜론은 마지막 비공백 문자여야 합니다
                size_t rest = idx + 1;
                while (rest < j && isspace(static_cast<unsigned char>(s[rest]))) ++rest;
                if (rest != j)
                {
                    std::cerr << "Unexpected token after ';' at line " << lineno << std::endl;
                    return false;
                }
                semicolon_found = true;
                // 세미콜론을 제외하도록 끝 위치를 조정합니다
                j = idx;
                break;
            }
        }
        if (!semicolon_found)
        {
            std::cerr << "Syntax error (missing ';') at line " << lineno << std::endl;
            return false;
        }
        if (i >= j)
        {
            std::cerr << "Empty directive at line " << lineno << std::endl;
            return false;
        }

    // 이제 키와 값을 추출합니다. 값은 따옴표로 묶여 공백을 포함할 수 있습니다.
    // 키 다음의 첫 공백 위치를 찾습니다
        size_t sp = i;
        while (sp < j && !isspace(static_cast<unsigned char>(s[sp]))) ++sp;
        if (sp == j)
        {
            std::cerr << "Directive has no value at line " << lineno << std::endl;
            return false;
        }

    // 값 시작까지 공백을 건너뜁니다
        size_t k = sp;
        while (k < j && isspace(static_cast<unsigned char>(s[k]))) ++k;
        if (k >= j)
        {
            std::cerr << "Directive has empty value at line " << lineno << std::endl;
            return false;
        }

        std::string key = s.substr(i, sp - i);
        std::string value;
        if (s[k] == '"')
        {
            // 따옴표로 묶인 값: 대응되는 닫는 따옴표까지 읽습니다
            // (세미콜론이 따옴표 밖에 있다는 것을 이미 확인했습니다)
            size_t qend = k + 1;
            while (qend < j && s[qend] != '"') ++qend;
            if (qend >= j || s[qend] != '"')
            {
                std::cerr << "Unterminated quoted value at line " << lineno << std::endl;
                return false;
            }
            value = s.substr(k + 1, qend - (k + 1));
            // 닫는 따옴표 뒤에 비공백 문자가 없는지 확인합니다
            size_t after = qend + 1;
            while (after < j && isspace(static_cast<unsigned char>(s[after]))) ++after;
            if (after != j)
            {
                std::cerr << "Unexpected token after quoted value at line " << lineno << std::endl;
                return false;
            }
        }
        else
        {
            // 따옴표가 아닌 값: k..j 범위 전체를 값으로 사용합니다
            value = s.substr(k, j - k);
        }
        // top-level(global)에서의 중복만 검사합니다.
        if (globalKeys.find(key) != globalKeys.end())
        {
            std::cerr << "Duplicate top-level directive key '" << key << "' at line " << lineno << std::endl;
            return false;
        }
        globalKeys.insert(key);
    // we don't store values here; blockParse will handle storing
    }
    if (skipDepth != 0)
    {
        std::cerr << "Unclosed '{' in configuration file" << std::endl;
        return false;
    }
    return true;
}
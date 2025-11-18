#include "../../includes/utils/ConfigSetterUtils.hpp"
#include "../../includes/utils/GeneralParseUtils.hpp"
#include "../../includes/libs/Libs.hpp"

ConfigSetterUtils::ConfigSetterUtils(void)
{
}

ConfigSetterUtils::~ConfigSetterUtils(void)
{
}

ConfigSetterUtils::ConfigSetterUtils(const ConfigSetterUtils &ref)
{
	(void)ref;
}

ConfigSetterUtils &ConfigSetterUtils::operator=(const ConfigSetterUtils &ref)
{
	(void)ref;
	return *this;
}

bool ConfigSetterUtils::setBaseBlock(const std::string &key, const std::string &val, BaseBlock &block)
{
    GeneralParseUtils gparse;
    
    if (key == "root")
    {
        block.setRoot(val);
        return true;
    }
    else if (key == "index")
    {
        // index may contain multiple files
        std::istringstream iss(val);
        std::vector<std::string> files;
        std::string f;
        while (iss >> f) files.push_back(f);
        if (!files.empty()) block.setIndexFiles(files);
        return true;
    }
    else if (key == "error_page")
    {
        // syntax: error_page <code> <code> ... <path>;
        std::istringstream iss(val);
        std::string token;
        std::vector<int> codes;
        std::string pathv;
        while (iss >> token)
        {
            bool isNumber = !token.empty();
            for (size_t i = 0; i < token.size() && isNumber; ++i)
                if (!std::isdigit(static_cast<unsigned char>(token[i]))) isNumber = false;
            if (isNumber)
            {
                int code = 0;
                std::istringstream ts(token);
                ts >> code;
                codes.push_back(code);
            }
            else
            {
                pathv = token;
                break;
            }
        }
        if (codes.empty() || pathv.empty())
        {
            std::cerr << "Invalid error_page directive: need one or more codes and a path" << std::endl;
            return false;
        }
        // validate codes and add mappings
        for (size_t i = 0; i < codes.size(); ++i)
        {
            int c = codes[i];
            if (c < 400 || c > 599)
            {
                std::cerr << "Invalid error code in error_page: " << c << std::endl;
                return false;
            }
            block.addErrorPage(c, pathv);
        }
        return true;
    }
    else if (key == "client_max_body_size")
    {
        // accept integer with optional k/K, m/M, g/G suffixes (base 1024)
        long long parsed = gparse.CalcClientMaxBodySize(val);
        if (parsed < 0)
        {
            std::cerr << "Invalid client_max_body_size value: " << val << std::endl;
            return false;
        }
        if (parsed > MAX_CLIENT_MAX_BODY_SIZE)
        {
            std::cerr << "client_max_body_size exceeds maximum allowed (" << MAX_CLIENT_MAX_BODY_SIZE << "): " << val << std::endl;
            return false;
        }
        block.setClientMaxBodySize(static_cast<size_t>(parsed));
        return true;
    }
    else if (key == "autoindex")
    {
        if (val == "on")
            block.setAutoindex(true);
        else if (val == "off")
            block.setAutoindex(false);
        else
        {
            std::cerr << "Invalid autoindex value: '" << val << "' (must be 'on' or 'off')" << std::endl;
            return false;
        }
        return true;
    }
    
    // not a BaseBlock directive
    return false;
}

bool ConfigSetterUtils::setGlobalValue(std::istream &is, Config &config)
{
    GeneralParseUtils gparse;
    std::string line;
    size_t lineno = 0;
    int workers = DEFAULT_WORKER_PROCESSES;
    bool ret = true;

    while (std::getline(is, line))
    {
        ++lineno;
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;

        // block header: ends with '{'
        std::string blockName;
        if (gparse.ParseBlockHeader(s, blockName))
        {
            if (blockName == "event")
            {
                ret = setEventBlock(is, config);
            }
            else if (blockName == "http")
            {
                ret = setHttpBlock(is, config);
            }
            else
            {
                ret = false;
            }
            if (!ret) break;
            continue;
        }

        if (s == "{" )
        {
            std::cerr << "Unexpected '{' alone at global:" << lineno << std::endl;
            ret = false;
            break;
        }

        if (s == "}")
        {
            // end of a higher-level block; in global context treat as error
            std::cerr << "Unexpected '}' at global:" << lineno << std::endl;
            ret = false;
            break;
        }

        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue; // not a directive
        if (key == "worker_processes")
        {
            int v = 0;
            if (!gparse.ParsePositiveInt(val, v))
            {
                std::cerr << "Invalid worker_processes value at global:" << lineno << std::endl;
                ret = false;
                break;
            }
            workers = v;
        }
    }

    if (ret)
        config.setWorkerProcesses(workers);
    return ret;
}

bool ConfigSetterUtils::setEventBlock(std::istream &is, Config &config)
{
    GeneralParseUtils gparse;
    std::string body;
    EventBlock eventBlock; // eventBlock initialized with DEFAULT_WORKER_CONNECTIONS

    if (!gparse.ReadBlockBody(is, body))
    {
        std::cerr << "Unbalanced braces in event block" << std::endl;
        return false;
    }

    std::istringstream inner(body);
    std::string line;
    size_t lineno = 0;

    while (std::getline(inner, line))
    {
        ++lineno;
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;
        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue;
        if (key == "worker_connections")
        {
            int v = 0;
            if (!gparse.ParsePositiveInt(val, v))
            {
                std::cerr << "Invalid worker_connections value in event block at line " << lineno << std::endl;
                return false;
            }
            eventBlock.setWorkerConnections(v);
        }
    }
    // If parser never set workerConnections, EventBlock constructor already set DEFAULT_WORKER_CONNECTIONS
    config.addEventBlock(eventBlock);
    return true;
}

bool ConfigSetterUtils::setHttpBlock(std::istream &is, Config &config)
{
    GeneralParseUtils gparse;
    std::string body;
    if (!gparse.ReadBlockBody(is, body))
    {
        std::cerr << "Unbalanced braces in http block" << std::endl;
        return false;
    }
    std::istringstream inner(body);
    std::string line;
    HttpBlock hb;
    while (std::getline(inner, line))
    {
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;
        std::string blockName;
        if (gparse.ParseBlockHeader(s, blockName))
        {
            if (blockName == "server")
            {
                if (!setServerBlock(inner, hb)) return false;
            }
            else
            {
                // unknown nested block inside http
                return false;
            }
        }
        else
        {
            std::string key, val;
            if (!gparse.ParseDirective(s, key, val)) continue;
            // try to apply BaseBlock directives first
            if (setBaseBlock(key, val, hb)) continue;
            // other http-level directives currently ignored
            continue;
        }
    }
    config.addHttpBlock(hb);
    return true;
}

bool ConfigSetterUtils::setServerBlock(std::istream &is, HttpBlock &httpBlock)
{
    GeneralParseUtils gparse;
    ServerBlock sb;
    std::string body;
    std::string line;

    if (!gparse.ReadBlockBody(is, body))
    {
        std::cerr << "Unbalanced braces in server block" << std::endl;
        return false;
    }

    std::istringstream inner(body);

    while (std::getline(inner, line))
    {
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;
        std::string blockName;
        if (gparse.ParseBlockHeader(s, blockName))
        {
            if (blockName == "location")
            {
                if (!setLocationBlock(inner, sb)) return false;
            }
            else
            {
                // unknown nested block inside server
                return false;
            }
        }
        else
        {
            std::string key, val;
            if (!gparse.ParseDirective(s, key, val)) continue;
            if (key == "listen")
            {
                // listen may contain multiple tokens (e.g. "80 127.0.0.1:8080")
                std::istringstream lss(val);
                std::string token;
                bool defFlag = false;
                std::vector<std::pair<std::string,int> > parsed;
                while (lss >> token)
                {
                    // token could be 'default_server' as flag
                    if (token == "default_server") { defFlag = true; continue; }
                    std::string host;
                    int port = 0;
                    if (!gparse.ParseListen(token, host, port)) { std::cerr << "Invalid listen token: " << token << std::endl; return false; }
                    parsed.push_back(std::make_pair(host, port));
                }
                for (size_t i = 0; i < parsed.size(); ++i)
                {
                    std::string h = parsed[i].first;
                    int p = parsed[i].second;
                    sb.addListen(h, p, defFlag);
                }
                if (defFlag) sb.setDefaultServer(true);
            }
            else if (key == "server_name")
            {
                // server_name can have multiple names separated by spaces
                std::istringstream iss(val);
                std::string name;
                while (iss >> name) sb.addServerName(name);
            }
            else
            {
                // try to apply BaseBlock directives (root, index, error_page, client_max_body_size, autoindex)
                if (setBaseBlock(key, val, sb)) continue;
                // unknown directive at server level -- ignore or extend later
                continue;
            }
        }
    }
    try {
        if (!httpBlock.addServerBlock(sb)) return false;
    } catch (const std::exception &e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool ConfigSetterUtils::setLocationBlock(std::istream &is, ServerBlock &serverBlock)
{
    GeneralParseUtils gparse;
    std::string body;
    if (!gparse.ReadBlockBody(is, body))
    {
        std::cerr << "Unbalanced braces in location block" << std::endl;
        return false;
    }
    std::istringstream inner(body);
    std::string line;
    LocationBlock lb;
    
    while (std::getline(inner, line))
    {
        std::string s = gparse.ParseContext(line);
        if (s.empty()) continue;
        
        std::string key, val;
        if (!gparse.ParseDirective(s, key, val)) continue;
        
        // try to apply BaseBlock directives first
        if (setBaseBlock(key, val, lb)) continue;
        
        // location-specific directives (methods, cgi, upload, redirect, etc.) can be added here later
        // for now, ignore unknown directives
        continue;
    }
    
    serverBlock.addLocationBlock(lb);
    return true;
}

bool ConfigSetterUtils::SetValue(char **argv, Config &config)
{
    (void)argv;
    const std::string path = config.getConfigPath();
    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << path << std::endl;
        return false;
    }

    bool ret = setGlobalValue(ifs, config);
    return ret;
}


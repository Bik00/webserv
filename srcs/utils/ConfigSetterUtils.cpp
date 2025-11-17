#include "../../includes/utils/ConfigSetterUtils.hpp"
#include "../../includes/utils/GeneralParseUtils.hpp"

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
            else if (key == "root")
            {
                sb.setRoot(val);
            }
            else if (key == "index")
            {
                // index may contain multiple files
                std::istringstream iss(val);
                std::vector<std::string> files;
                std::string f;
                while (iss >> f) files.push_back(f);
                if (!files.empty()) sb.setIndexFiles(files);
            }
            else if (key == "error_page")
            {
                // syntax: error_page <code> <path>;
                std::istringstream iss(val);
                int code;
                std::string pathv;
                if (!(iss >> code >> pathv)) return false;
                sb.addErrorPage(code, pathv);
            }
            else if (key == "client_max_body_size")
            {
                // accept plain integer bytes for now
                int v = 0;
                if (!gparse.ParsePositiveInt(val, v)) return false;
                sb.setClientMaxBodySize(static_cast<size_t>(v));
            }
            else if (key == "autoindex")
            {
                if (val == "on") sb.setAutoindex(true);
                else sb.setAutoindex(false);
            }
            
            else
            {
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
    // For now we don't parse location directives; just attach the location block
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


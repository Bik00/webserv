#include "../../includes/Header.hpp"

HttpConfig::HttpConfig(void)
{
}

HttpConfig::~HttpConfig(void)
{
}

HttpConfig::HttpConfig(const HttpConfig &ref)
{
    *this = ref;
}

HttpConfig &HttpConfig::operator=(const HttpConfig &ref)
{
    if (this != &ref)
    {
        this->servers = ref.servers;
    }
    return *this;
}

bool HttpConfig::loadFromFile(const std::string &path)
{
    std::ifstream ifs(path.c_str());
    if (!ifs)
    {
        std::cerr << "Could not open config file: " << path << std::endl;
        return false;
    }

    std::string token;
    std::vector<std::string> toks;
    // naive tokenizer: split by whitespace but keep braces and semicolons as tokens
    while (ifs >> token)
    {
        size_t pos = 0;
        while (pos < token.size())
        {
            if (token[pos] == '{' || token[pos] == '}' || token[pos] == ';')
            {
                toks.push_back(std::string(1, token[pos]));
                pos++;
            }
            else
            {
                size_t start = pos;
                while (pos < token.size() && token[pos] != '{' && token[pos] != '}' && token[pos] != ';') pos++;
                toks.push_back(token.substr(start, pos - start));
            }
        }
    }

    // parse tokens
    size_t i = 0;
    while (i < toks.size())
    {
        if (toks[i] == "server")
        {
            ++i;
            if (i >= toks.size() || toks[i] != "{") { std::cerr << "Expected '{' after server" << std::endl; return false; }
            ++i; // enter server block
            ServerConfig srv;
            while (i < toks.size() && toks[i] != "}")
            {
                std::string d = toks[i++];
                if (d == "listen")
                {
                    if (i >= toks.size()) break;
                    std::string listen_v = toks[i++];
                    // format host:port or port
                    size_t colon = listen_v.find(':');
                    if (colon != std::string::npos)
                    {
                        srv.setHost(listen_v.substr(0, colon));
                        srv.setPort((unsigned int)atoi(listen_v.substr(colon+1).c_str()));
                    }
                    else
                    {
                        srv.setHost("0.0.0.0");
                        srv.setPort((unsigned int)atoi(listen_v.c_str()));
                    }
                    // expect semicolon
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
                else if (d == "server_name")
                {
                    while (i < toks.size() && toks[i] != ";") { srv.addServerName(toks[i++]); }
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
                else if (d == "root")
                {
                    if (i < toks.size()) { srv.setRoot(toks[i++]); }
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
                else if (d == "index")
                {
                    if (i < toks.size()) { srv.setIndex(toks[i++]); }
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
                else if (d == "error_page")
                {
                    if (i+1 < toks.size())
                    {
                        int code = atoi(toks[i++].c_str());
                        std::string pathv = toks[i++];
                        srv.addErrorPage(code, pathv);
                    }
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
                else if (d == "location")
                {
                    // next token is path, then '{'
                    if (i >= toks.size()) break;
                    LocationConfig loc;
                    loc.path = toks[i++];
                    if (i < toks.size() && toks[i] == "{") ++i;
                    while (i < toks.size() && toks[i] != "}")
                    {
                        std::string ld = toks[i++];
                        if (ld == "try_files")
                        {
                            while (i < toks.size() && toks[i] != ";") { loc.try_files.push_back(toks[i++]); }
                            if (i < toks.size() && toks[i] == ";") ++i;
                        }
                        else if (ld == "root")
                        {
                            if (i < toks.size()) { loc.root = toks[i++]; }
                            if (i < toks.size() && toks[i] == ";") ++i;
                        }
                        else { /* skip unknowns */ if (i < toks.size() && toks[i] == ";") ++i; }
                    }
                    if (i < toks.size() && toks[i] == "}") ++i; // end location
                    srv.addLocation(loc);
                }
                else
                {
                    // skip unknown directive until semicolon
                    while (i < toks.size() && toks[i] != ";") ++i;
                    if (i < toks.size() && toks[i] == ";") ++i;
                }
            }
            if (i < toks.size() && toks[i] == "}") ++i; // end server
            this->servers.push_back(srv);
        }
        else { ++i; }
    }

    return true;
}

void    HttpConfig::getInfo(void) const
{
    std::cout << "HttpConfig: " << servers.size() << " servers loaded" << std::endl;
    for (size_t i = 0; i < servers.size(); ++i)
    {
        const ServerConfig &s = servers[i];
        std::cout << "Server " << i << ": " << s.getHost() << ":" << s.getPort() << std::endl;
        std::cout << "  root: " << s.getRoot() << " index: " << s.getIndex() << std::endl;
        std::cout << std::endl;
    }
}
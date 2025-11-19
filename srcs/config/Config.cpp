#include "../../includes/config/Config.hpp"

Config::Config(void)
{
    workerProcesses = DEFAULT_WORKER_PROCESSES;
}

Config::~Config(void)
{
}

Config::Config(const Config &ref)
{
    *this = ref;
}

Config &Config::operator=(const Config &ref)
{
    if (this != &ref)
    {
    this->httpBlock = ref.httpBlock;
    this->eventBlock = ref.eventBlock;
    this->configPath = ref.configPath;
    this->workerProcesses = ref.workerProcesses;
    }
    return *this;
}

void Config::setConfigPath(const std::string &path)
{
    this->configPath = path;
}

void Config::setWorkerProcesses(int num)
{
    this->workerProcesses = num;
}

void Config::addEventBlock(const EventBlock &eventBlock)
{
    this->eventBlock = eventBlock;
}

void Config::addHttpBlock(const HttpBlock &httpBlock)
{
    this->httpBlock = httpBlock;
}

const std::string &Config::getConfigPath() const
{
    return this->configPath;
}

int Config::getWorkerProcesses() const
{
    return this->workerProcesses;
}

const HttpBlock &Config::getHttpBlock() const
{
    return this->httpBlock;
}

static void printIndent(int depth)
{
    for (int i = 0; i < depth; ++i) std::cout << "  ";
}

void Config::printConfig() const
{
    std::cout << "Config: " << this->configPath << std::endl;
    std::cout << "Worker processes: " << this->workerProcesses << std::endl;

    // HTTP block
    printIndent(0);
    std::cout << "http:" << std::endl;

    const std::vector<ServerBlock> &servers = this->httpBlock.getServerBlocks();
    for (size_t si = 0; si < servers.size(); ++si)
    {
        const ServerBlock &sb = servers[si];
        printIndent(1); std::cout << "server #" << si << ":" << std::endl;

        // listen
        const std::vector<ListenAddr> &la = sb.getListenAddrs();
        for (size_t i = 0; i < la.size(); ++i)
        {
            printIndent(2);
            std::cout << "listen " << la[i].host << ":" << la[i].port;
            if (la[i].defaultServerFlag) std::cout << " (default)";
            std::cout << std::endl;
        }

        // server_names
        const std::vector<std::string> &names = sb.getServerNames();
        if (!names.empty())
        {
            printIndent(2); std::cout << "server_name: ";
            for (size_t n = 0; n < names.size(); ++n)
            {
                if (n) std::cout << ", ";
                std::cout << names[n];
            }
            std::cout << std::endl;
        }

        // base block fields
        if (!sb.getRoot().empty()) { printIndent(2); std::cout << "root: " << sb.getRoot() << std::endl; }
        const std::vector<std::string> &idx = sb.getIndexFiles();
        if (!idx.empty()) { printIndent(2); std::cout << "index: "; for (size_t k=0;k<idx.size();++k){ if(k) std::cout<<", "; std::cout<<idx[k]; } std::cout<<std::endl; }
        const std::map<int, std::string> &errs = sb.getErrorPages();
        if (!errs.empty()) { printIndent(2); std::cout << "error_page:" << std::endl; for (std::map<int,std::string>::const_iterator it=errs.begin(); it!=errs.end(); ++it){ printIndent(3); std::cout<<it->first<<" -> "<<it->second<<std::endl; } }
        if (sb.getClientMaxBodySize() > 0) { printIndent(2); std::cout << "client_max_body_size: " << sb.getClientMaxBodySize() << std::endl; }
        printIndent(2); std::cout << "autoindex: " << (sb.getAutoindex() ? "on" : "off") << std::endl;

        // allowed methods
        const std::vector<std::string> &methods = sb.getAllowedMethods();
        if (!methods.empty()) { printIndent(2); std::cout << "allow: "; for (size_t m=0;m<methods.size();++m){ if(m) std::cout<<", "; std::cout<<methods[m]; } std::cout<<std::endl; }

        // locations
        const std::vector<LocationBlock> &locs = sb.getLocationBlocks();
        for (size_t li = 0; li < locs.size(); ++li)
        {
            const LocationBlock &lb = locs[li];
            printIndent(2); std::cout << "location: " << lb.getPath() << std::endl;
            if (!lb.getRoot().empty()) { printIndent(3); std::cout << "root: " << lb.getRoot() << std::endl; }
            const std::vector<std::string> &lidx = lb.getIndexFiles();
            if (!lidx.empty()) { printIndent(3); std::cout << "index: "; for (size_t k=0;k<lidx.size();++k){ if(k) std::cout<<", "; std::cout<<lidx[k]; } std::cout<<std::endl; }
            const std::vector<std::string> &lmethods = lb.getAllowedMethods();
            if (!lmethods.empty()) { printIndent(3); std::cout << "allow: "; for (size_t m=0;m<lmethods.size();++m){ if(m) std::cout<<", "; std::cout<<lmethods[m]; } std::cout<<std::endl; }
                if (lb.getHasRedirect()) { printIndent(3); std::cout << "return: " << lb.getRedirectCode() << " " << lb.getRedirectTarget() << std::endl; }
            if (lb.isUploadEnabled()) { printIndent(3); std::cout << "upload_store: " << lb.getUploadStore() << std::endl; }
            if (!lb.getCgiExtensions().empty()) { printIndent(3); std::cout << "cgi_pass: " << lb.getCgiPath() << " for "; for (size_t e=0;e<lb.getCgiExtensions().size();++e){ if(e) std::cout<<", "; std::cout<<lb.getCgiExtensions()[e]; } std::cout<<std::endl; }
        }
    }
}

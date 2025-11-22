#include "../../includes/process/MasterProcess.hpp"

// Global flag for signal handling
static volatile sig_atomic_t g_shutdown = 0;

// Signal handlers
static void sigterm_handler(int sig)
{
    (void)sig;
    g_shutdown = 1;
}

MasterProcess::MasterProcess(void)
{
}

MasterProcess::~MasterProcess(void)
{
    // Cleanup: close and delete all server sockets
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        serverSockets[i]->setClose();
        delete serverSockets[i];
    }
    serverSockets.clear();
}

MasterProcess::MasterProcess(const MasterProcess &ref)
{
    (void)ref;
}

MasterProcess &MasterProcess::operator=(const MasterProcess &ref)
{
    (void)ref;
    return (*this);
}

void MasterProcess::Run(const Config &config)
{
    try
    {
        std::cout << "Master process starting..." << std::endl;
        
        // Step 1: Setup server sockets (BEFORE fork)
        setupServerSockets(config);
        std::cout << "Listening on " << serverSockets.size() << " address(es)" << std::endl;
        
        // Step 2: Install signal handlers
        installSignalHandlers();
        
        // Step 3: Fork worker processes
        forkWorkers(config);
        std::cout << "Forked " << workerPids.size() << " worker(s)" << std::endl;
        
        // Step 4: Monitor loop
        std::cout << "Master process entering monitor loop..." << std::endl;
        monitorLoop();
        
        std::cout << "Master process shutting down..." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Master process error: " << e.what() << std::endl;
        terminateAllWorkers();
        throw;
    }
}

void MasterProcess::setupServerSockets(const Config &config)
{
    const HttpBlock &http = config.getHttpBlock();
    const std::vector<ServerBlock> &servers = http.getServerBlocks();
    
    // Collect unique listen addresses
    std::set<std::string> uniqueListens;
    
    for (size_t si = 0; si < servers.size(); ++si)
    {
        const std::vector<ListenAddr> &listens = servers[si].getListenAddrs();
        
        for (size_t li = 0; li < listens.size(); ++li)
        {
            const ListenAddr &la = listens[li];
            std::ostringstream key;
            key << la.host << ":" << la.port;
            uniqueListens.insert(key.str());
        }
    }
    
    // Create, bind, and listen on each unique address
    for (std::set<std::string>::const_iterator it = uniqueListens.begin();
         it != uniqueListens.end(); ++it)
    {
        // Parse host:port from key
        std::string key = *it;
        size_t colonPos = key.find(':');
        if (colonPos == std::string::npos)
            continue;
        
        std::string host = key.substr(0, colonPos);
        std::string portStr = key.substr(colonPos + 1);
        int port = 0;
        std::istringstream(portStr) >> port;
        
        // Create and setup socket
        ServerSocket *sock = new ServerSocket(host, port);
        sock->setBind();
        sock->setListen();
        
        serverSockets.push_back(sock);
        
        std::cout << "Bound to " << host << ":" << port 
                  << " (fd=" << sock->getFd() << ")" << std::endl;
    }
    
    if (serverSockets.empty())
    {
        throw std::runtime_error("No server sockets created");
    }
}

void MasterProcess::installSignalHandlers()
{
    // Use the simple signal() API instead of sigaction
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
        throw std::runtime_error("Failed to install SIGTERM handler");

    if (signal(SIGINT, sigterm_handler) == SIG_ERR)
        throw std::runtime_error("Failed to install SIGINT handler");
}

void MasterProcess::forkWorkers(const Config &config)
{
    int nWorkers = config.getWorkerProcesses();
    std::vector<int> serverSocketFds = getServerSocketFds();
    
    for (int i = 0; i < nWorkers; ++i)
    {
        pid_t pid = fork();
        
        if (pid < 0)
        {
            // Fork failed
            std::cerr << "Fork failed for worker " << i << std::endl;
            throw std::runtime_error("fork() failed");
        }
        else if (pid == 0)
        {
            // Child process: become worker
            WorkerProcess worker;
            worker.Run(serverSocketFds);
            _exit(0);
        }
        else
        {
            // Parent: record worker PID
            addWorkerPid(pid);
            std::cout << "Forked worker " << i << " (PID: " << pid << ")" << std::endl;
        }
    }
}

std::vector<int> MasterProcess::getServerSocketFds() const
{
    std::vector<int> fds;
    for (size_t i = 0; i < serverSockets.size(); ++i)
    {
        fds.push_back(serverSockets[i]->getFd());
    }
    return fds;
}

void MasterProcess::addWorkerPid(pid_t pid)
{
    workerPids.push_back(pid);
}

void MasterProcess::handleWorkerExit(pid_t pid, int status)
{
    // Remove from worker list
    for (std::vector<pid_t>::iterator it = workerPids.begin(); 
         it != workerPids.end(); ++it)
    {
        if (*it == pid)
        {
            workerPids.erase(it);
            break;
        }
    }
    
    if (WIFEXITED(status))
    {
        std::cout << "Worker " << pid << " exited with status " 
                  << WEXITSTATUS(status) << std::endl;
    }
    else if (WIFSIGNALED(status))
    {
        std::cout << "Worker " << pid << " killed by signal " 
                  << WTERMSIG(status) << std::endl;
    }
    
    // TODO: Implement respawn policy if needed
}

void MasterProcess::terminateAllWorkers()
{
    std::cout << "Terminating all workers..." << std::endl;
    
    // Send SIGTERM to all workers
    for (size_t i = 0; i < workerPids.size(); ++i)
    {
        kill(workerPids[i], SIGTERM);
    }
    
    // Wait for workers to exit (with timeout)
    int waitCount = 0;
    const int maxWait = 5; // 5 seconds
    
    while (!workerPids.empty() && waitCount < maxWait)
    {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        
        if (pid > 0)
        {
            handleWorkerExit(pid, status);
        }
        else
        {
            sleep(1);
            waitCount++;
        }
    }
    
    // Force kill any remaining workers
    if (!workerPids.empty())
    {
        std::cout << "Force killing remaining workers..." << std::endl;
        for (size_t i = 0; i < workerPids.size(); ++i)
        {
            kill(workerPids[i], SIGKILL);
        }
        
        // Reap zombies
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;
    }
    
    workerPids.clear();
}

void MasterProcess::monitorLoop()
{
    while (!g_shutdown)
    {
        // Check for dead workers
        int status;
        pid_t pid;
        
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            handleWorkerExit(pid, status);
        }
        
        // If all workers died and we're not shutting down, exit
        if (workerPids.empty() && !g_shutdown)
        {
            std::cerr << "All workers died unexpectedly" << std::endl;
            break;
        }
        
        // Sleep briefly
        sleep(1);
    }
    
    // Shutdown requested
    terminateAllWorkers();
}
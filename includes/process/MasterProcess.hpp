#ifndef MASTER_PROCESS_HPP
# define MASTER_PROCESS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../socket/ServerSocket.hpp"
# include <vector>
# include <sys/types.h>

class MasterProcess
{
private:
    std::vector<ServerSocket*> serverSockets;
    std::vector<pid_t> workerPids;
    
    // Setup and initialization
    void setupServerSockets(const Config &config);
    void forkWorkers(const Config &config);
    
    // Signal handling
    void installSignalHandlers();
    
    // Worker management
    void addWorkerPid(pid_t pid);
    void handleWorkerExit(pid_t pid, int status);
    void terminateAllWorkers();
    
    // Main loop
    void monitorLoop();
    
public:
    MasterProcess(void);
    ~MasterProcess(void);
    MasterProcess(const MasterProcess &ref);
    MasterProcess &operator=(const MasterProcess &ref);
    void Start(const Config &config);
};

#endif /* MASTER_PROCESS_HPP */
#ifndef WORKER_PROCESS_HPP
# define WORKER_PROCESS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"
# include "../socket/ClientSocket.hpp"

class WorkerProcess
{
private:
    int epollFd;
    std::vector<int> listenFds;
    std::map<int, ClientSocket*> clients;
    
    void setupEpoll();
    void addListenSockets();
    void eventLoop();
    void handleListenEvent(int fd);
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
    void closeClient(int fd);
    
public:
    WorkerProcess(void);
    ~WorkerProcess(void);
    WorkerProcess(const WorkerProcess &ref);
    WorkerProcess &operator=(const WorkerProcess &ref);
    void Run(const std::vector<int> &listenFds);
};

#endif /* WORKER_PROCESS_HPP */
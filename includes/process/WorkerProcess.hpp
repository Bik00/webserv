#ifndef WORKER_PROCESS_HPP
# define WORKER_PROCESS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class WorkerProcess
{
public:
    WorkerProcess(void);
    ~WorkerProcess(void);
    WorkerProcess(const WorkerProcess &ref);
    WorkerProcess &operator=(const WorkerProcess &ref);
    void Start(const Config &config);
};

#endif /* WORKER_PROCESS_HPP */
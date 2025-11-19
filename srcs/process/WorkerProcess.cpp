#include "../../includes/process/WorkerProcess.hpp"

WorkerProcess::WorkerProcess(void)
{
}

WorkerProcess::~WorkerProcess(void)
{
}

WorkerProcess::WorkerProcess(const WorkerProcess &ref)
{
    (void)ref;
}

WorkerProcess &WorkerProcess::operator=(const WorkerProcess &ref)
{
    (void)ref;
    return (*this);
}

void WorkerProcess::Start(const Config &config)
{
    std::cout << "WorkerProcess started with config path: " << config.getConfigPath() << std::endl;
}
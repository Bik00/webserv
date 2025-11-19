#include "../../includes/master/MasterProcess.hpp"

MasterProcess::MasterProcess(void)
{
}

MasterProcess::~MasterProcess(void)
{
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

void MasterProcess::Start(const Config &config)
{
    
    std::cout << "MasterProcess started with config path: " << config.getConfigPath() << std::endl;
}
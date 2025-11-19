#ifndef MASTER_PROCESS_HPP
# define MASTER_PROCESS_HPP

# include "../libs/Libs.hpp"
# include "../config/Config.hpp"

class MasterProcess
{
public:
    MasterProcess(void);
    ~MasterProcess(void);
    MasterProcess(const MasterProcess &ref);
    MasterProcess &operator=(const MasterProcess &ref);
    void Start(const Config &config);
};

#endif /* MASTER_PROCESS_HPP */
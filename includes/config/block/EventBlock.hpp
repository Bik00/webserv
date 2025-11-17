#ifndef EVENT_BLOCK_HPP
# define EVENT_BLOCK_HPP

# include "../../libs/Libs.hpp"

class EventBlock
{
private:
    int workerConnections;
public:
    EventBlock(void);
    ~EventBlock(void);
    EventBlock(const EventBlock &ref);
    EventBlock &operator=(const EventBlock &ref);

    void setWorkerConnections(int num);
    int getWorkerConnections() const;
};

#endif /* EVENT_BLOCK_HPP */
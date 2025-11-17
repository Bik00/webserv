#ifndef EVENT_BLOCK_HPP
# define EVENT_BLOCK_HPP

# include "../../libs/Libs.hpp"

class EventBlock
{
private:
    int workerConnections; // 이벤트 루프에서 허용할 최대 동시 연결 수
public:
    EventBlock(void);
    ~EventBlock(void);
    EventBlock(const EventBlock &ref);
    EventBlock &operator=(const EventBlock &ref);

    void setWorkerConnections(int num);
    int getWorkerConnections() const;
};

#endif /* EVENT_BLOCK_HPP */
#ifndef EVENT_BLOCK_HPP
# define EVENT_BLOCK_HPP

# include "../../libs/Libs.hpp"

class EventBlock
{
private:
public:
    EventBlock(void);
    ~EventBlock(void);
    EventBlock(const EventBlock &ref);
    EventBlock &operator=(const EventBlock &ref);
};

#endif /* EVENT_BLOCK_HPP */
#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "../../libs/Libs.hpp"

class LocationBlock
{
private:
public:
    LocationBlock(void);
    ~LocationBlock(void);
    LocationBlock(const LocationBlock &ref);
    LocationBlock &operator=(const LocationBlock &ref);
};

#endif /* LOCATION_BLOCK_HPP */
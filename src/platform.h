#pragma once

#include <limits>

inline int get_msb_pos(unsigned int data)
{
    if (!data)
    {
        return 0;
    }
    return std::numeric_limits<unsigned int>::digits - __builtin_clz(data);
}

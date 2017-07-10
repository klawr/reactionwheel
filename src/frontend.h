#pragma once

#include "curses.hpp"
#include "vector3.h"

class frontend 
{
public:
    frontend(curses::window &target);

    void update_accelleration(vector3i raw);
    void update_gyro(vector3i raw);
    void update_queue_load(int load);

private:
    inline void put_vec(int line, vector3i raw);

    curses::window &mWnd;
};

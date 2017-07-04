#pragma once

#include "curses.hpp"

class frontend 
{
public:
    frontend(curses::window &target);

private:
    curses::window &mWnd;
};

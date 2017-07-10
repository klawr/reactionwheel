#include "frontend.h"

namespace
{



}

inline void frontend::put_vec(int line, vector3i raw)
{
    auto put_comp = [&] (int pos, int comp)
    {
        auto str = std::to_string(comp);
        str = std::string(7-str.size(), ' ') + str;
        mWnd.write(line, pos, str);
    };
    put_comp(6, raw.x);
    put_comp(16, raw.y);
    put_comp(26, raw.z);
}

frontend::frontend(curses::window &target)
    : mWnd(target)
{
    auto put_xyz = [this] (int line)
    { 
        mWnd.write(line, 4, "x:");
        mWnd.write(line, 14, "y:");
        mWnd.write(line, 24, "z:");
    }; 

    mWnd.write(2, 2, "accel");
    put_xyz(3);
    mWnd.write(5, 2, "gyro");
    put_xyz(6);

    mWnd.write(8, 2, "queue load:");
}

void frontend::update_accelleration(vector3i raw)
{
    put_vec(3, raw);
}

void frontend::update_gyro(vector3i raw)
{
    put_vec(6, raw);
}

void frontend::update_queue_load(int load)
{
    auto str = std::to_string(load);
    str = std::string(5-str.size(), ' ') + str;
    mWnd.write(8, 13, str);
}

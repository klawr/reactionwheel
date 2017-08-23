#include "frontend.h"

namespace
{



}

using namespace curspp;

inline void frontend::put_vec(int line, vector3i raw)
{
    auto put_comp = [&] (int pos, int comp)
    {
        auto str = std::to_string(comp);
        str = std::string(7-str.size(), ' ') + str;
        mWnd.write({pos, line}, str);
    };
    put_comp(6, raw.x);
    put_comp(16, raw.y);
    put_comp(26, raw.z);
}

frontend::frontend(curspp::window &target)
    : mWnd(target)
{
    auto put_xyz = [this] (int line)
    {
        mWnd.write({4, line}, "x:");
        mWnd.write({14, line}, "y:");
        mWnd.write({24, line}, "z:");
    };

    mWnd.write({2, 2}, "accel");
    put_xyz(3);
    mWnd.write({2, 5}, "gyro");
    put_xyz(6);

    mWnd.write({2, 8}, "queue load:");
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
    mWnd.write({13, 8}, str);
}

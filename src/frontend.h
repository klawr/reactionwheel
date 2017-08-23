#pragma once

#include <cursespp/cursespp.hpp>
#include "vector3.h"

class frontend
{
public:
    frontend(curspp::window &target);

    void update_accelleration(vector3i raw);
    void update_gyro(vector3i raw);
    void update_queue_load(int load);

private:
    inline void put_vec(int line, vector3i raw);

    curspp::window &mWnd;
};

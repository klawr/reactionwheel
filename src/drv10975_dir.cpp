#include "precompiled.hpp"
#include "drv10975.hpp"

#include <wiringPi.h>

namespace reactionwheel
{


constexpr int dir_pin_id = 0;


void drv10975::init_dir_pin()
{
    pinMode(dir_pin_id, OUTPUT);
    set_dir_pin(false);
}

void drv10975::set_dir_pin(bool val)
{
    digitalWrite(dir_pin_id, val);
}


}

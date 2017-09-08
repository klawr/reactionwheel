#include "precompiled.hpp"
#include "thread_bus.h"

namespace reactionwheel
{

namespace
{

std::atomic<bool> stop_flag{false};

}

void stop()
{
    stop_flag = true;
}

bool stopped()
{
    return stop_flag;
}

std::tuple<message_port, message_port> message_port::create_pair()
{
    auto c = std::make_shared<channel>();
    return {
        message_port{ c, false },
        message_port{ c, true }
    };
}

}

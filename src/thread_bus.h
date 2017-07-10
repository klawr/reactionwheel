#pragma once

#include <memory>
#include <atomic>
#include <chrono>
#include <type_traits>

#include <boost/lockfree/spsc_queue.hpp>

#include "vector3.h"

namespace reactionwheel
{

using thread_bus_clock = std::conditional_t<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock
>;

class bus_message
{
    virtual ~bus_message() = default;

protected:
    bus_message() = default;
    bus_message(const bus_message &) = default;
    bus_message & operator=(const bus_message &) = default;
};

struct data_point_message
{
    thread_bus_clock::time_point post_time;
    vector3i raw_accelleration;
    vector3i raw_gyro;
};

class thread_bus
{
    using data_queue_t = boost::lockfree::spsc_queue<
        data_point_message, boost::lockfree::capacity<1024>
    >;

public:
    thread_bus();

    bool push_data(data_point_message &msg)
    {
        msg.post_time = thread_bus_clock::now();
        return mDataMsgs.push(msg);
    }
    bool pop_data(data_point_message &msg)
    {
        return mDataMsgs.pop(msg);
    }
    int queue_load()
    {
        return mDataMsgs.read_available();
    }

    bool stop_flag() const noexcept
    {
        return mStopFlag;
    }
    void stop()
    {
        mStopFlag = true;
    }

private:
    data_queue_t mDataMsgs;
    std::atomic<bool> mStopFlag;
};

}

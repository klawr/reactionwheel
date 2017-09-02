#pragma once

#include <memory>
#include <atomic>
#include <chrono>
#include <type_traits>

#include "readerwriterqueue.h"

#include "vector3.h"


namespace reactionwheel
{

using thread_bus_clock = std::conditional_t<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock
>;

void stop();
bool stopped();

struct bus_message
{
    thread_bus_clock::time_point post_time;

    virtual ~bus_message() = default;

protected:
    bus_message() = default;
    bus_message(const bus_message &) = default;
    bus_message & operator=(const bus_message &) = default;
};

struct sensor_data_message
    : public bus_message
{
    vector3i raw_accelleration;
    vector3i raw_gyro;
};

class message_port
{
    struct channel
    {
        using queue_t = moodycamel::ReaderWriterQueue<
            std::unique_ptr<bus_message>
        >;

        queue_t & in_queue(bool id)
        {
            return queues[!id];
        }
        queue_t & out_queue(bool id)
        {
            return queues[id];
        }

        queue_t queues[2];
    };

public:
    static std::tuple<message_port, message_port> create_pair();

    bool valid() const
    {
        return !!mChannel;
    }
    explicit operator bool() const
    {
        return this->valid();
    }

    int in_load_approx()
    {
        return static_cast<int>(in_queue().size_approx());
    }
    int out_load_approx()
    {
        return static_cast<int>(out_queue().size_approx());
    }

    void post(std::unique_ptr<bus_message> msg)
    {
        if (valid() && msg)
        {
            msg->post_time = thread_bus_clock::now();
            out_queue().enqueue(std::move(msg));
        }
    }
    std::unique_ptr<bus_message> try_pop()
    {
        std::unique_ptr<bus_message> result;
        if (valid())
        {
            in_queue().try_dequeue(result);
        }
        return std::move(result);
    }

    message_port(const message_port &) = delete;
    message_port(message_port &&other)
        : mChannel(std::move(other.mChannel))
        , mId(other.mId)
    {
    }
    message_port & operator=(const message_port &) = delete;
    message_port & operator=(message_port &&other)
    {
        mChannel = std::move(other.mChannel);
        mId = other.mId;
        return *this;
    }

private:

    explicit message_port(std::shared_ptr<channel> c, bool id)
        : mChannel(c)
        , mId(id)
    {
    }
    channel::queue_t & in_queue()
    {
        return mChannel->in_queue(mId);
    }
    channel::queue_t & out_queue()
    {
        return mChannel->out_queue(mId);
    }

    std::shared_ptr<channel> mChannel;
    bool mId;
};

}

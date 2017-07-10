#include "thread_bus.h"

namespace reactionwheel
{

thread_bus::thread_bus()
    : mDataMsgs()
    , mLastReadMessage(data_point_message{})
    , mStopFlag(false)
{
}



}

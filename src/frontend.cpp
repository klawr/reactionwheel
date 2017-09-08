#include "precompiled.hpp"
#include "frontend.h"

#include "thread_bus.h"

namespace
{

constexpr std::string_view to_string(bool value)
{
    return {value ? "true" : "false"};
}

}

using namespace curspp;
using namespace reactionwheel;

frontend::frontend(curspp::window &target)
    : mWnd(target)
    , mPage1(mWnd)
    , mDriverPage(mWnd)
{
}

bool frontend::live()
{
    update_stopped();
    mWnd.refresh();
    if (const auto [flag, value] = mWnd.try_read_key(); flag)
    {
        if (auto cptr = std::get_if<char32_t>(&value))
        {
            switch (*cptr)
            {
            case U'q':
                stop();
                return false;

            case U'1':
                mPage1.render(true);
                mDisplayedPage = page::p1;
                break;

            case U'2':
                mDriverPage.render(true);
                mDisplayedPage = page::stats;
                break;

            default:
                break;
            }
        }
    }
    //mWnd.refresh();
    return true;
}

void frontend::update_accelleration(vector3i raw)
{
    if (mDisplayedPage == page::p1)
    {
        mPage1.mAccelDp.value(raw);
        mPage1.mAccelDp.render(false);
    }
}

void frontend::update_gyro(vector3i raw)
{
    if (mDisplayedPage == page::p1)
    {
        mPage1.mGyroDp.value(raw);
        mPage1.mGyroDp.render(false);
    }
}

void frontend::update_stopped()
{
    std::string txt = std::string{to_string(reactionwheel::stopped())};

    mPage1.mAppStopped.value(txt);
    mPage1.mAppStopped.render(false);
    mDriverPage.mAppStopped.value(std::move(txt));
    mDriverPage.mAppStopped.render(false);
}

void frontend::update_queue_load(int load)
{
    auto str = std::to_string(load);
    str = std::string(5-str.size(), ' ') + str;
    mWnd.write({13, 8}, str);
}

void frontend::swallow(reactionwheel::driver_status_message &msg)
{
    if (mDisplayedPage == page::stats)
    {
        const std::string strYes { "yes" };
        const std::string strNo { "no" };

        mDriverPage.mMotorSpeed.value(std::to_string(msg.motor_speed / 10));
        mDriverPage.mMotorSpeed.render(false);
        mDriverPage.mMotorPeriod.value(std::to_string(msg.motor_period * 10));
        mDriverPage.mMotorPeriod.render(false);
        mDriverPage.mMotorKate.value(std::to_string(msg.motor_kate / 2080.0));
        mDriverPage.mMotorKate.render(false);

        double supplyV = msg.supply_voltage * 22.8 / 256.0;
        mDriverPage.mSupplyVoltage.value(std::to_string(supplyV));
        mDriverPage.mSupplyVoltage.render(false);

        double sCmd = static_cast<double>(msg.speed_cmd) / 255.0;
        mDriverPage.mSpeedCmd.value(std::to_string(sCmd));
        mDriverPage.mSpeedCmd.render(false);
        sCmd = static_cast<double>(msg.speed_cmd_buffer) / 255.0;
        mDriverPage.mSpeedCmdBuffer.value(std::to_string(sCmd));
        mDriverPage.mSpeedCmdBuffer.render(false);

        mDriverPage.mOverHeat.value(msg.driver_status & driver_status_code::over_heat ? strYes : strNo);
        mDriverPage.mOverHeat.render(false);
        mDriverPage.mMotorLock.value(msg.driver_status & driver_status_code::motor_locked ? strYes : strNo);
        mDriverPage.mMotorLock.render(false);
        mDriverPage.mOverCurrent.value(msg.driver_status & driver_status_code::over_current ? strYes : strNo);
        mDriverPage.mOverCurrent.render(false);

        mDriverPage.mFaultCurrentLimit.value(msg.motor_status & motor_fault_code::current_limit ? strYes : strNo);
        mDriverPage.mFaultCurrentLimit.render(false);
        mDriverPage.mFaultAbnormalSpeed.value(msg.motor_status & motor_fault_code::abnormal_speed ? strYes : strNo);
        mDriverPage.mFaultAbnormalSpeed.render(false);
        mDriverPage.mFaultAbnormalKate.value(msg.motor_status & motor_fault_code::abnormal_kate ? strYes : strNo);
        mDriverPage.mFaultAbnormalKate.render(false);
        mDriverPage.mFaultNoMotor.value(msg.motor_status & motor_fault_code::no_motor ? strYes : strNo);
        mDriverPage.mFaultNoMotor.render(false);
        mDriverPage.mFaultStuckOpenLoop.value(msg.motor_status & motor_fault_code::stuck_open_loop ? strYes : strNo);
        mDriverPage.mFaultStuckOpenLoop.render(false);
        mDriverPage.mFaultStuckClosedLoop.value(msg.motor_status & motor_fault_code::stuck_closed_loop ? strYes : strNo);
        mDriverPage.mFaultStuckClosedLoop.render(false);
    }
}


frontend::page1::page1(curspp::window &wnd)
    : mWnd(wnd)
    , mAccelDp(mWnd, {2, 4}, "accel")
    , mGyroDp(mWnd, {2, 7}, "gyro")
    , mAppStopped(mWnd, "app stopped:", {2, 2}, 12, 6)
{
    mAccelDp.render(true);
    mGyroDp.render(true);

    mAppStopped.value("false");
    mAppStopped.render(true);
}

void frontend::page1::render(bool full)
{
    mWnd.clear();

    mAccelDp.render(true);
    mGyroDp.render(true);
    mAppStopped.render(true);

    mWnd.refresh(full);
}

frontend::driver_stats_page::driver_stats_page(curspp::window &wnd)
    : mWnd(wnd)
    , mOverHeat(mWnd, "over heated:", {2, 4}, 20, 10)
    , mOverCurrent(mWnd, "over current:", {2, 5}, 20, 10)
    , mMotorLock(mWnd, "motor locked:", {2, 6}, 20, 10)
    , mMotorSpeed(mWnd, "motor speed:", {2, 8}, 20, 10)
    , mMotorPeriod(mWnd, "motor perdiod", {2, 9}, 20, 10)
    , mMotorKate(mWnd, "motor kate:", {2, 10}, 20, 10)
    , mSupplyVoltage(mWnd, "supply voltage:", {2, 11}, 20, 10)
    , mSpeedCmd(mWnd, "speed cmd:", {2, 12}, 20, 10)
    , mSpeedCmdBuffer(mWnd, "speed cmd buffer:", {2, 13}, 20, 10)
    , mFaultCurrentLimit(mWnd, "f current limit:", {2, 15}, 20, 10)
    , mFaultAbnormalSpeed(mWnd, "f abnormal speed:", {2, 16}, 20, 10)
    , mFaultAbnormalKate(mWnd, "f abnormal kate:", {2, 17}, 20, 10)
    , mFaultNoMotor(mWnd, "f no motor:", {2, 18}, 20, 10)
    , mFaultStuckOpenLoop(mWnd, "f stuck ol:", {2, 19}, 20, 10)
    , mFaultStuckClosedLoop(mWnd, "f stuck cl:", {2, 20}, 20, 10)
    , mAppStopped(mWnd, "app stopped:", {2, 2}, 20, 10)
{
}

void frontend::driver_stats_page::render(bool full)
{
    mWnd.clear();

    mOverHeat.render(true);
    mOverCurrent.render(true);
    mMotorLock.render(true);
    mMotorSpeed.render(true);
    mMotorPeriod.render(true);
    mMotorKate.render(true);
    mSupplyVoltage.render(true);
    mSpeedCmd.render(true);
    mSpeedCmdBuffer.render(true);
    mFaultCurrentLimit.render(true);
    mFaultAbnormalSpeed.render(true);
    mFaultAbnormalKate.render(true);
    mFaultNoMotor.render(true);
    mFaultStuckOpenLoop.render(true);
    mFaultStuckClosedLoop.render(true);
    mAppStopped.render(true);

    mWnd.refresh(full);
}

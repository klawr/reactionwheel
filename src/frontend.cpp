#include "precompiled.hpp"
#include "frontend.h"

#include <cmath>
#include <unordered_map>

#include <boost/math/constants/constants.hpp>

#include <ucmd-parser/all.hpp>

#include "drv10975.hpp"

namespace
{

constexpr std::string_view to_string(bool value)
{
    return {value ? "true" : "false"};
}

std::unordered_map<std::string, reactionwheel::i2c_register> drv10975_regmap {
    { "mparam1", reactionwheel::drv10975::motor_param1_rid },
    { "mparam2", reactionwheel::drv10975::motor_param2_rid },
    { "mparam3", reactionwheel::drv10975::motor_param3_rid },
    { "sopt1", reactionwheel::drv10975::sys_opt1_rid },
    { "sopt2", reactionwheel::drv10975::sys_opt2_rid },
    { "sopt3", reactionwheel::drv10975::sys_opt3_rid },
    { "sopt4", reactionwheel::drv10975::sys_opt4_rid },
    { "sopt5", reactionwheel::drv10975::sys_opt5_rid },
    { "sopt6", reactionwheel::drv10975::sys_opt6_rid },
    { "sopt7", reactionwheel::drv10975::sys_opt7_rid },
    { "sopt8", reactionwheel::drv10975::sys_opt8_rid },
    { "sopt9", reactionwheel::drv10975::sys_opt9_rid },
};

}

using namespace curspp;
using namespace reactionwheel;

frontend::frontend(curspp::window &target, message_port &motorPort)
    : mWnd(target)
    , mMotorPort(motorPort)
    , mLastUpdate(std::chrono::steady_clock::now())
    , mPage1(mWnd)
    , mDriverPage(mWnd)
    , mCmdInput(target, { 0, target.size().y - 1 }, target.size().x)
{
    //color_pair(1).value(color::green(), color::default_());
    mEval.insert("motor speed", ucmdp::make_command([this](int spdvalue)
    {
        auto msg = std::make_unique<change_speed_message>();
        msg->value = spdvalue;
        mMotorPort.post(std::move(msg));
    }));
    mEval.insert("set byte reg", ucmdp::make_command([this](std::string regName, std::byte value)
    {
        if (auto regIt = drv10975_regmap.find(regName);
            regIt != drv10975_regmap.end())
        {
            auto msg = std::make_unique<i2c_reg8_override_message>(
                std::tuple{regIt->second, value}
            );
            mMotorPort.post(std::move(msg));
        }
    }));
    mEval.insert("set word reg", ucmdp::make_command([this](std::string regName, std::byte v1, std::byte v2)
    {
        
    }));
    mEval.insert("stop", ucmdp::make_command([]() { stop(); }));
}

bool frontend::live()
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    auto now = steady_clock::now();
    if (now - mLastUpdate < 15ms)
    {
        return true;
    }
    mLastUpdate = now;

    update_stopped();
    mWnd.move_cursor({0, 0});
    mWnd.refresh();
    if (const auto [flag, value] = mWnd.try_read_key(); flag)
    {
        if (auto cptr = std::get_if<char32_t>(&value);
                cptr && *cptr > 0 && *cptr < 128)
        {
            auto c = static_cast<char>(*cptr);
            if (c == '\n')
            {
                try
                {
                    mEval(mCmdInput.content_view());
                }
                catch (...)
                {
                }
                mCmdInput.clear();
            }
            else
            {
                mCmdInput.append(c);
            }
        }
        else if (auto fptr = std::get_if<curspp::function_key>(&value))
        {
            auto fkey = *fptr;
            if (fkey == curspp::function_key::f1)
            {
                mPage1.render(true);
                mDisplayedPage = page::p1;
            }
            else if (fkey == curspp::function_key::f2)
            {
                mDriverPage.render(true);
                mDisplayedPage = page::stats;
            }
            else if (fkey == curspp::function_key::f9)
            {
                stop();
                return false;
            }
            else if (fkey == curspp::function_key::backspace)
            {
                mCmdInput.pop_back();
            }
            else if (fkey == curspp::function_key::enter)
            {

            }
        }
        mCmdInput.render();
    }
    return true;
}

void frontend::update_accelleration(vector3i raw)
{
    if (mDisplayedPage == page::p1)
    {
        mPage1.mAccelDp.value(raw);
        mPage1.mAccelDp.render(false);

        auto angleRad = std::atan(static_cast<double>(raw.x) / raw.y);
        auto angleDeg = angleRad * 180 / boost::math::double_constants::pi;
        mPage1.mAngle.value(std::to_string(angleDeg));
        mPage1.mAngle.render(false);
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

    if (mDisplayedPage == page::p1)
    {
        mPage1.mAppStopped.value(txt);
        mPage1.mAppStopped.render(false);
    }
    if (mDisplayedPage == page::stats)
    {
        mDriverPage.mAppStopped.value(std::move(txt));
        mDriverPage.mAppStopped.render(false);
    }
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
    , mAngle(mWnd, "current angle:", {2, 10}, 14, 7)
{
    mAccelDp.render(true);
    mGyroDp.render(true);
    mAngle.render(true);

    mAppStopped.render(true);
}

void frontend::page1::render(bool full)
{
    mWnd.clear();

    mAccelDp.render(true);
    mGyroDp.render(true);
    mAppStopped.render(true);
    mAngle.render(true);

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

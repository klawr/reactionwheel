#pragma once

#include "enum_bitset.hpp"
#include "platform.h"
#include "i2c.hpp"
#include "thread_bus.h"

namespace reactionwheel
{


enum class driver_status_code : std::uint8_t
{
    ok           = 0,
    over_heat    = 0b1000'0000,
    standby      = 0b0100'0000,
    over_current = 0b0010'0000,
    motor_locked = 0b0001'0000,
};
std::true_type allow_enum_bitset(driver_status_code &&);

enum class motor_fault_code : std::uint8_t
{
    none              = 0,
    current_limit     = 0b00'0001,
    abnormal_speed    = 0b00'0010,
    abnormal_kate     = 0b00'0100,
    no_motor          = 0b00'1000,
    stuck_open_loop   = 0b01'0000,
    stuck_closed_loop = 0b10'0000,
};
std::true_type allow_enum_bitset(motor_fault_code &&);


struct driver_status_message
    : public bus_message
{
    std::uint16_t motor_speed;
    std::uint16_t motor_period;
    std::uint16_t motor_kate;
    std::uint8_t supply_voltage;
    std::uint8_t speed_cmd;
    std::uint8_t speed_cmd_buffer;
    motor_fault_code motor_status;
    driver_status_code driver_status;

    std::chrono::microseconds total_cmd_time;
    int cmd_count;
};

struct driver_options_message
{
    // sys opt 1
    std::chrono::milliseconds ISD_threshold;
    int ipd_advance_angle;
    bool ISD_enabled;
    int reverse_drive_threshold;

    // sys opt 2
    int ol_current;
    int ol_current_rampup_rate;
    int break_done_threshold;

    // sys opt 3
    int ctr_coefficient;
    int ol_startup_accelerate_so;
    int ol_startup_accelerate_fo;

    // sys opt 5
    bool ipd_release_mode;
    bool avs_mode;
    bool avs_mech_enable;
    bool avs_induc_enable;
    bool lock_detection_enabled;
    bool abnormal_speed_enabled;
    bool abnormal_kate_enable;
    bool ignore_motor_faults;
};


class drv10975
{
public:
    struct motor_resistance_t
    {
        explicit motor_resistance_t(double rOhm);

        constexpr std::byte value() const
        {
            return mValue;
        }

    private:
        const std::byte mValue;
    };

    drv10975(std::string_view device);
    ~drv10975();

    short speed()
    {
        return static_cast<short>(mSpeedCtrl2 & 0b1) << 8 | mSpeedCtrl1;
    }
    void speed(short speed);

    std::unique_ptr<driver_status_message> read_driver_status();

private:
    static void init_dir_pin();
    static void set_dir_pin(bool val);

    void motor_param1(bool doubleFrequency, std::byte motor_resistance);
    void motor_param2(bool cycleAdjustmend, std::byte motor_velocity_constant);
    void motor_param3(bool ctrlAdvanceMode, std::byte delay);

    void sys_opt2(std::byte raw);
    std::byte sys_opt2();

    void cmd_begin()
    {
        mCmdBeginTime = thread_bus_clock::now();
    }
    void cmd_end()
    {
        auto deltaTime = thread_bus_clock::now() - mCmdBeginTime;
        mStatusMsg->total_cmd_time
            += std::chrono::duration_cast<std::chrono::microseconds>(deltaTime);
        ++mStatusMsg->cmd_count;
    }
    void cmd_next()
    {
        cmd_end();
        cmd_begin();
    }

    i2c_device mDevice;

    std::uint8_t mSpeedCtrl1;
    std::uint8_t mSpeedCtrl2;

    thread_bus_clock::time_point mCmdBeginTime;
    std::unique_ptr<driver_status_message> mStatusMsg
        = std::make_unique<driver_status_message>();
};


}

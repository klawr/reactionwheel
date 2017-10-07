#include "precompiled.hpp"
#include "drv10975.hpp"

#include <cassert>
#include <iostream>


namespace reactionwheel
{

namespace
{

/*
std::byte encode_drv_floating_point(unsigned int value)
{
    int msb = get_msb_pos(value);
    auto shift = std::max(0, msb - 3);

    if (shift > 0b111)
    {
        return std::byte{0b0111'1111};
    }
    return std::byte{
        ((value & (0b1111 << shift)) >> shift)
        | ((shift & 0b111) << 4)
    };
}

std::byte encode_motor_resistance(double rOhm)
{
    auto dscrt = static_cast<unsigned int>(rOhm / 0.00735);
    return dscrt
        ? encode_drv_floating_point(dscrt)
        : std::byte{0};
}

std::byte encode_velocity_constant(double kate)
{
    auto dscrt = static_cast<unsigned int>(kate * 1442);
    return dscrt
        ? encode_drv_floating_point(dscrt)
        : std::byte{0};
}

std::byte encode_delay_time(std::chrono::microseconds delay)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    auto nsdscrt = nanoseconds { delay / 2500ns };
    auto uidscrt = static_cast<unsigned int>(nsdscrt.count());
    return uidscrt
        ? encode_drv_floating_point(uidscrt)
        : std::byte{0};
}
*/


}

/*
drv10975::motor_resistance_t::motor_resistance_t(double rOhm)
    : mValue(encode_motor_resistance(rOhm))
{
}
*/

drv10975::drv10975(std::string_view device)
    : mDevice(device, i2c_device_id{ 0b101'0010 })
{
    mDevice.smbus_write_byte(i2c_register{ 0x03 }, std::byte{0b1100'0000});
    speed(0);

    init_dir_pin();
    set_dir_pin(true);

    
    motor_param1(true, std::byte{0b011'1010});
    motor_param2(false, std::byte{0x3F});//1C
    motor_param3(true, std::byte{0x00});

    mDevice.smbus_write_byte(sys_opt1_rid, std::byte{0b00'00'1'1'11});
    mDevice.smbus_write_byte(sys_opt2_rid, std::byte{0b11'001'111});
    mDevice.smbus_write_byte(sys_opt3_rid, std::byte{0b11'111'101});
    mDevice.smbus_write_byte(sys_opt4_rid, std::byte{0b10010'100});//10010'111
    mDevice.smbus_write_byte(sys_opt5_rid, std::byte{0b00000001});
    mDevice.smbus_write_byte(sys_opt6_rid, std::byte{0b0000'111'0});
    mDevice.smbus_write_byte(sys_opt7_rid, std::byte{0b1'000'1001});
    mDevice.smbus_write_byte(sys_opt8_rid, std::byte{0b0000'0'1'11});
    mDevice.smbus_write_byte(sys_opt9_rid, std::byte{0b01'11'11'0'0});

}
drv10975::~drv10975()
{
    speed(0);
}

void drv10975::speed(short speed)
{
    set_dir_pin(speed >= 0);
    if (speed < 0)
    {
        speed = -speed;
    }

    assert(0 <= speed && speed <= 511);
    mSpeedCtrl1 = static_cast<std::uint8_t>(speed);
    mSpeedCtrl2 = 0b1000'0000 | ((static_cast<std::uint16_t>(speed) >> 8) & 0b1);

    // MSB must be written first according to device docs
    cmd_begin();
    mDevice.smbus_write_byte(i2c_register{ 0x01 }, std::byte{mSpeedCtrl2});
    cmd_next();
    mDevice.smbus_write_byte(i2c_register{ 0x00 }, std::byte{mSpeedCtrl1});
    cmd_end();
}

std::unique_ptr<driver_status_message> drv10975::read_driver_status()
{
    cmd_begin();
    mStatusMsg->driver_status =
        static_cast<driver_status_code>(
            mDevice.smbus_read_byte(i2c_register{ 0x10 })
        );

    cmd_next();
    mStatusMsg->motor_speed =
        static_cast<std::uint16_t>(
            mDevice.smbus_read_byte(i2c_register{0x11})
        ) << 8;
    cmd_next();
    mStatusMsg->motor_speed |=
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x12})
        );

    cmd_next();
    mStatusMsg->motor_period =
        static_cast<std::uint16_t>(
            mDevice.smbus_read_byte(i2c_register{0x13})
        ) << 8;
    cmd_next();
    mStatusMsg->motor_period |=
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x14})
        );

    cmd_next();
    mStatusMsg->motor_kate =
        static_cast<std::uint16_t>(
            mDevice.smbus_read_byte(i2c_register{0x15})
        ) << 8;
    cmd_next();
    mStatusMsg->motor_kate |=
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x16})
        );

    cmd_next();
    mStatusMsg->supply_voltage =
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x1A})
        );

    cmd_next();
    mStatusMsg->speed_cmd =
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x1B})
        );

    cmd_next();
    mStatusMsg->speed_cmd_buffer =
        static_cast<std::uint8_t>(
            mDevice.smbus_read_byte(i2c_register{0x1C})
        );

    cmd_next();
    mStatusMsg->motor_status =
        static_cast<motor_fault_code>(
            mDevice.smbus_read_byte(i2c_register{0x1E})
        );

    cmd_end();

    auto result = std::move(mStatusMsg);
    mStatusMsg = std::make_unique<driver_status_message>();
    return std::move(result);
}

constexpr std::byte mp_mask { 0b0111'1111 };

void drv10975::motor_param1(bool doubleFrequency, std::byte motor_resistance)
{
    auto reg = std::byte{doubleFrequency} << 7 | (mp_mask & motor_resistance);
    cmd_begin();
    mDevice.smbus_write_byte(i2c_register{ 0x20 }, reg);
    cmd_end();
}
void drv10975::motor_param2(bool cycleAdjustment, std::byte motor_velocity_constant)
{
    auto reg = std::byte{cycleAdjustment} << 7 | (mp_mask & motor_velocity_constant);
    cmd_begin();
    mDevice.smbus_write_byte(i2c_register{ 0x21 }, reg);
    cmd_end();
}
void drv10975::motor_param3(bool ctrlAdvanceMode, std::byte delay)
{
    auto reg = std::byte{ctrlAdvanceMode} << 7 | (mp_mask & delay);
    cmd_begin();
    mDevice.smbus_write_byte(i2c_register{ 0x22 }, reg);
    cmd_end();
}

void drv10975::sys_opt2(std::byte raw)
{
    cmd_begin();
    mDevice.smbus_write_byte(sys_opt2_rid, raw);
    cmd_end();
}


}

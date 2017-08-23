
#include "mpu6050.h"

#include <cerrno>
#include <cstdlib>
#include <cstdint>
#include <iostream>

using namespace reactionwheel;

namespace
{

constexpr i2c_register reg_settings { 0x6B };
constexpr i2c_register reg_accel_x_h { 0x3B };
constexpr i2c_register reg_accel_x_l { 0x3C };
constexpr i2c_register reg_accel_y_h { 0x3D };
constexpr i2c_register reg_accel_y_l { 0x3E };
constexpr i2c_register reg_accel_z_h { 0x3F };
constexpr i2c_register reg_accel_z_l { 0x40 };
constexpr i2c_register reg_temp_h { 0x41 };
constexpr i2c_register reg_temp_l { 0x42 };
constexpr i2c_register reg_gyro_x_h { 0x43 };
constexpr i2c_register reg_gyro_x_l { 0x44 };
constexpr i2c_register reg_gyro_y_h { 0x45 };
constexpr i2c_register reg_gyro_y_l { 0x46 };
constexpr i2c_register reg_gyro_z_h { 0x47 };
constexpr i2c_register reg_gyro_z_l { 0x48 };

constexpr i2c_device_id to_dev_id(mpu6050::id which)
{
	switch (which)
	{
	case mpu6050::id::m0: return i2c_device_id { 0b1101000 };
	case mpu6050::id::m1: return i2c_device_id { 0b1101001 };
	default: throw "illegal mpu6050 id";
	}
}

}

mpu6050::mpu6050(std::string_view devicePath, id which)
	: mDevice(devicePath, to_dev_id(which))
{
	mDevice.smbus_write_byte(reg_settings, std::byte { 0x00 });
}

void mpu6050::update()
{
	mAcceleration.x = read16(reg_accel_x_l, reg_accel_x_h);
	mAcceleration.y = read16(reg_accel_y_l, reg_accel_y_h);
	mAcceleration.z = read16(reg_accel_z_l, reg_accel_z_h);
	mTemp = read16(reg_temp_l, reg_temp_h);
	mGyro.x = read16(reg_gyro_x_l, reg_gyro_x_h);
	mGyro.y = read16(reg_gyro_y_l, reg_gyro_y_h);
	mGyro.z = read16(reg_gyro_z_l, reg_gyro_z_h);
}

std::uint16_t mpu6050::read16u(reactionwheel::i2c_register low_reg, reactionwheel::i2c_register high_reg)
{
	return static_cast<uint16_t>(read8u(high_reg)) << 8 | read8u(low_reg);
}

int mpu6050::read16(reactionwheel::i2c_register low_reg, reactionwheel::i2c_register high_reg)
{
	return static_cast<int16_t>(read16u(low_reg, high_reg));
}

std::uint8_t mpu6050::read8u(reactionwheel::i2c_register id)
{
	return static_cast<std::uint8_t>(mDevice.smbus_read_byte(id));
}

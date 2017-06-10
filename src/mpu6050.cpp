
#include "mpu6050.h"

#include <cerrno>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include <wiringPiI2C.h>

namespace 
{
	constexpr register_id reg_settings { 0x6B };
	constexpr register_id reg_accel_x_h { 0x3B };
	constexpr register_id reg_accel_x_l { 0x3C };
	constexpr register_id reg_accel_y_h { 0x3D };
	constexpr register_id reg_accel_y_l { 0x3E };
	constexpr register_id reg_accel_z_h { 0x3F };
	constexpr register_id reg_accel_z_l { 0x40 };
	constexpr register_id reg_temp_h { 0x41 };
	constexpr register_id reg_temp_l { 0x42 };
	constexpr register_id reg_gyro_x_h { 0x43 };
	constexpr register_id reg_gyro_x_l { 0x44 };
	constexpr register_id reg_gyro_y_h { 0x45 };
	constexpr register_id reg_gyro_y_l { 0x46 };
	constexpr register_id reg_gyro_z_h { 0x47 };
	constexpr register_id reg_gyro_z_l { 0x48 };
}

mpu6050::mpu6050(device_id id)
	: mDevice(wiringPiI2CSetup(id.value()))
{
	if (mDevice == -1)
	{
		std::cout << "failed to initialize i2c device " << id.value() << ", errno: " << errno << std::endl;
		std::exit(-1);
	}
	write(reg_settings, 0x00);
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

std::uint16_t mpu6050::read16u(register_id low_reg, register_id high_reg)
{
	return static_cast<uint16_t>(read_raw(high_reg)) << 8 | read_raw(low_reg);
}

int mpu6050::read16(register_id low_reg, register_id high_reg)
{
	return static_cast<int16_t>(read16u(low_reg, high_reg));
}

std::uint8_t mpu6050::read_raw(register_id id)
{
	auto val = wiringPiI2CReadReg8(mDevice, id.value());
	if (val < 0)
	{
		std::cout << "read reg failed for " << id.value() << ", errno: " << errno << std::endl;
	}
	return static_cast<std::uint8_t>(val);
}

void mpu6050::write(register_id id, std::uint8_t data)
{
	if (wiringPiI2CWriteReg8(mDevice, id.value(), data) < 0)
	{
		std::cout << "write reg " << id.value() << " failed, data: " << data << ", errno: " << errno << std::endl;
	}
}

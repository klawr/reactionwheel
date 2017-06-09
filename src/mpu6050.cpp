
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

std::uint8_t mpu6050::read(register_id id)
{
	auto val = wiringPiI2CReadReg8(mDevice, id.value());
	if (val < 0)
	{
		std::cout << "read reg failed for " << id.value() << ", errno: " << errno << std::endl;
	}
	return val;
}

void mpu6050::write(register_id id, std::uint8_t data)
{
	if (wiringPiI2CWriteReg8(mDevice, id.value(), data) < 0)
	{
		std::cout << "write reg " << id.value() << " failed, data: " << data << ", errno: " << errno << std::endl;
	}
}

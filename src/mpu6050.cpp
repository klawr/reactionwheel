
#include "mpu6050.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>

#include <wiringPiI2C.h>

mpu6050::mpu6050(device_id id)
	: mDevice(wiringPiI2CSetup(id.value()))
{
	if (mDevice == -1)
	{
		std::cout << "failed to initialize i2c device " << id.value() << ", errno: " << errno << std::endl;
		std::exit(-1);
	}
}

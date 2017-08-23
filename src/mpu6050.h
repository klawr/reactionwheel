#pragma once

#include <cstdint>
#include <string_view>

#include "vector3.h"
#include "i2c.hpp"

class mpu6050
{
public:
	enum class id
	{
		m0,
		m1,
	};

	mpu6050(std::string_view devicePath, id which);

	void update();

	vector3i acceleration() const
	{
		return mAcceleration;
	}
	vector3i gyro() const
	{
		return mGyro;
	}

private:
	std::uint8_t read8u(reactionwheel::i2c_register id);

	int read16(reactionwheel::i2c_register low_reg, reactionwheel::i2c_register high_reg);
	std::uint16_t read16u(reactionwheel::i2c_register low_reg, reactionwheel::i2c_register high_reg);

	reactionwheel::i2c_device mDevice;
	vector3i mAcceleration;
	vector3i mGyro;
	int mTemp;
};


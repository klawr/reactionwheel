#pragma once

#include <cstdint>

#include "vector3.h"

class device_id
{
public:
	explicit constexpr device_id(int value)
		: mValue(value)
	{
	}

	constexpr int value() const
	{
		return mValue;
	}

private:
	const int mValue;
};

class register_id
{
public:
	explicit constexpr register_id(int value)
		: mValue(value)
	{
	}

	constexpr int value() const
	{
		return mValue;
	}

private:
	const int mValue;
};

class mpu6050
{
public:
	mpu6050(device_id i2c_device);
	
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
	std::uint8_t read_raw(register_id id);
	void write(register_id id, std::uint8_t value);

	int read16(register_id low_reg, register_id high_reg);
	std::uint16_t read16u(register_id low_reg, register_id high_reg);

	int mDevice;
	vector3i mAcceleration;
	vector3i mGyro;
	int mTemp;
};


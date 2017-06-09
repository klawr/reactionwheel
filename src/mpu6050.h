#pragma once

#include <cstdint>

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

private:
	std::uint8_t read(register_id id);
	void write(register_id id, std::uint8_t value);

	int mDevice;
};


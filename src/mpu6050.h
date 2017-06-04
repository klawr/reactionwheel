#pragma once

class device_id
{
public:
	explicit device_id(int value)
		: mValue(value)
	{
	}

	int value() const
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
	int mDevice;
};


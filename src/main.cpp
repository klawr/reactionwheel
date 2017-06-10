
#include <cstdio>

#include <chrono>
#include <thread>

#include <iostream>

#include "mpu6050.h"
#include "stepper.h"

#include <wiringPi.h>

int main()
{
	wiringPiSetupGpio();
	mpu6050 sensor { device_id { 0b1101000 } };
	
	StepperDriver driver; 

	for (int i = 0; i < 2000; ++i)
	{	
		driver.FullStep();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		sensor.update();
		auto acc = sensor.acceleration();
		printf("(%d, %d, %d)\n", acc.x, acc.y, acc.z);
		std::cout << sensor.gyro().z << std::endl;
	}
	driver.Disable();
}


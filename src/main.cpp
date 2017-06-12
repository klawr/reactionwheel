
#include <cstdio>

#include <chrono>
#include <thread>

#include <iostream>

#include "mpu6050.h"
#include "stepper.h"

#include <wiringPi.h>

int main()
{
	using namespace std::chrono;
	using namespace std::literals::chrono_literals;

	wiringPiSetupGpio();
	mpu6050 sensor { device_id { 0b1101000 } };
	
	StepperDriver driver;

	double deg = 0;
	double gyro_z = 0;
	
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i)
	{	
		driver.FullStep();

		sensor.update();
		//auto acc = sensor.acceleration();
		//printf("(%d, %d, %d)\n", acc.x, acc.y, acc.z);
		
		double gyro_z_old = gyro_z;
		double gyro_z = sensor.gyro().z * 250. /32768 - 0.512;
		std::this_thread::sleep_for(2939us - (high_resolution_clock::now() - start_time));
		auto elapsed_time = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
		start_time = high_resolution_clock::now();
		deg += elapsed_time.count() * gyro_z / 1'000'000;
		double accel = (gyro_z - gyro_z_old)*elapsed_time.count()/1'000'000;

		std::cout.precision(2);
		std::cout << accel << std::fixed << std::showpos << "\tdeg/s2 | " << gyro_z << std::fixed << std::showpos << "\tdeg/s | " << deg << std::fixed << std::showpos << "\tdeg | "
		       	  << elapsed_time.count() << " microseconds" << std::endl;
	}
	driver.Disable();
}


#include <cstdio>

#include <chrono>
#include <thread>

#include <iostream>

#include "mpu6050.h"
#include "stepper.h"
#include "vector3.h"
#include "curses.hpp"

#include <wiringPi.h>
#include <cmath>

int main()
{
	using namespace curses;
	curses_session csession;

	auto &wnd = csession.session_window();
	wnd.write("Hello World!");
	wnd.refresh();
	wnd.read_key();

	return 0;
}

int old_main()
{
	using namespace std::chrono;
	using namespace std::literals::chrono_literals;

	wiringPiSetupGpio();
	mpu6050 sensor { device_id { 0b1101000 } };
	
	StepperDriver driver;

	auto start_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = duration_cast<microseconds>(high_resolution_clock::now() - start_time);

	//declare all the vectors
	vector3d gyro; 
	vector3d racc,
	 	 rspd,
		 rdst,
		 tacc,
		 tspd,
		 tdst;
	//constants:
	constexpr vector3d flaw { 6.742826079, 1.868674477, 0.2783904902 };
	constexpr double g = 9.81,

	       time = 3'060.0 / 1'000'000,
	       bid = 250. / 32'768, // convert bits in rotational speed in degree / second
	       bia = g * 2 / 32'768; // convert bits in translational acceleration in meters / seconds_squared 
	std::cout << time << std:: endl; 
	for (int i = 0; i < 10000; ++i)
	{	
		//driver.FullStep();

		//update the sensor values and safe them in a vector
		sensor.update();
		auto acc = vector3d(sensor.acceleration());
		auto gyro_old = gyro; //safe last values for differentiation
		gyro = vector3d(sensor.gyro());
		//printf("(%d, %d, %d)\n", acc.x, acc.y, acc.z);
		//printf("(%d, %d, %d)\n", gyro.x, gyro.y, gyro.z);
		
		//make the values more usable by calculating results
		//translational acceleration
		tacc = acc * bia;
		//translational speed
		tspd += time * tacc;
		//translational distance
		tdst += time * tspd;
		//rotational acceleration
//	       	racc = (((gyro - gyro_old) * bid) - flaw) / time; // does not work. maybe because operators are just for 2?
		//rotational speed
		rspd = (gyro * bid) - 0.27839; // does not work. maybe because operators are just for 2?
		//rotational distance
		rdst += rspd * time;
		//radius


		//wait, so the loop is "exactly" 3000us long
		std::this_thread::sleep_for(3000us - (high_resolution_clock::now() - start_time));
		auto elapsed_time = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
		start_time = high_resolution_clock::now();
		
		//
		//output
		//
		//std::cout.precision(3);
		//std::cout << static_cast<int>(atan((acc.y*bia)/(acc.x*bia))*-180.0/3.1415926) << "\t acc.y \t"
		//	  << acc.y*bia << std::fixed << "\t acc.x \t" << acc.x*bia << std::fixed 
		//	  << "\t elapsed time \t" << elapsed_time.count() << std::endl;
		for (int j = 0; j < 20; j++)
		{
			std::cout << std::endl;
		}
		std::cout << "translational acceleration in meter / second_squared; x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", tacc.x, tacc.y, tacc.z);
		std::cout << "translational speed in meter / second;		    x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", tspd.x, tspd.y, tspd.z); 
		std::cout << "translational distance in meter;			    x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", tdst.x, tdst.y, tdst.z);
		std::cout << "rotational acceleration in degree / second_squared;   x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", racc.x, racc.y, racc.z);
		std::cout << "rotational speed in degree / second;		    x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", rspd.x, rspd.y, rspd.z);
		std::cout << "rotational distance in degree; 			    x , y , z" << std::endl;
		printf("(%f \t | \t %f \t | \t %f \t)\n", rdst.x, rdst.y, rdst.z);
		std::cout << i << std::endl; 
	
	//	std::cout.precision(2); 
	//	std:: cout << /* ... */ << std::endl;
	}
	driver.Disable();
	return 0;
}

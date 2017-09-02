
#include <cstdio>

#include <chrono>
#include <thread>
#include <functional>
#include <string_view>

#include <iostream>

#include <cursespp/cursespp.hpp>

#include "mpu6050.h"
#include "drv10975.hpp"
#include "vector3.h"
#include "frontend.h"
#include "thread_bus.h"

#include <cmath>

using namespace reactionwheel;
using namespace std::string_view_literals;

int sensor_main(message_port frontendPort, message_port motorPort);
int motor_main(message_port frontendPort, message_port motorPort);

int main()
{
	using namespace curspp;
	curses_session csession;
	curses_session::raw(true);
	curses_session::echo(false);
	curses_session::set(cursor_state::invisible);

	auto wnd = window::standard_screen();
	wnd.refresh();
	wnd.keypad(true);
	frontend ui { wnd };

	auto [sensorPort, xxxTmpSensorPort] = message_port::create_pair();
	auto [motorPort, xxxTmpMotorPort] = message_port::create_pair();
	auto xxxIComSensorMotor = message_port::create_pair();

	std::thread sensor_thread(sensor_main, std::move(xxxTmpSensorPort), std::move(std::get<0>(xxxIComSensorMotor)));
	std::thread motor_thread(motor_main, std::move(xxxTmpMotorPort), std::move(std::get<1>(xxxIComSensorMotor)));

	while (ui.live())
	{
		auto smsg = sensorPort.try_pop();
		auto mmsg = motorPort.try_pop();
		if (smsg || mmsg)
		{
			if (smsg)
			{
				if (auto sdatamsg = dynamic_cast<sensor_data_message*>(smsg.get()))
				{
					ui.update_accelleration(sdatamsg->raw_accelleration);
					ui.update_gyro(sdatamsg->raw_gyro);
				}
			}
			if (mmsg)
			{
				if (auto drv_stats = dynamic_cast<driver_status_message*>(mmsg.get()))
				{
					ui.swallow(*drv_stats);
				}
			}
		}
		else
		{
			std::this_thread::yield();
		}
	}

	motor_thread.join();
	sensor_thread.join();
	return 0;
}

int sensor_main(message_port frontendPort, message_port motorPort)
{
	try
	{
		mpu6050 sensor { "/dev/i2c-1"sv, mpu6050::id::m0 };

		while (!stopped())
		{
			sensor.update();
			auto msg = std::make_unique<sensor_data_message>();
			msg->raw_accelleration = sensor.acceleration();
			msg->raw_gyro = sensor.gyro();
			frontendPort.post(std::move(msg));

			//TODO: derive motor action
		}
    }
    catch (const std::exception &e)
    {
        std::cerr << "sensor_main failed: " << e.what() << std::endl;
        stop();
    }
	catch (...)
	{
		std::cerr << "sensor_main failed with an unknown exception." << std::endl;
	}
	return 0;
}

int motor_main(message_port frontendPort, message_port motorPort)
{
	try
	{
		drv10975 motor { "/dev/i2c-2"sv };
		motor.speed(400);

		while (!stopped())
		{
			motor.speed(400);
			
			frontendPort.post(motor.read_driver_status());
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "motor_main failed: " << e.what() << std::endl;
		stop();
	}
	catch (...)
	{
		std::cerr << "motor_main failed with an unknown exception." << std::endl;
	}
	return 0;
}

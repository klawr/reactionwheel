
#include <cstdio>

#include <chrono>
#include <thread>
#include <functional>

#include <iostream>

#include "mpu6050.h"
#include "stepper.h"
#include "vector3.h"
#include "curses.hpp"
#include "frontend.h"
#include "thread_bus.h"

#include <wiringPi.h>
#include <cmath>

using namespace reactionwheel;

int hw_main(thread_bus &bus);

int main()
{
	using namespace curses;
	curses_session csession;

	auto &wnd = csession.session_window();
	wnd.refresh();
	frontend ui { wnd };

	thread_bus bus;
	std::thread hw_thread(hw_main, std::ref(bus));

	data_point_message data_msg;
	while (!bus.stop_flag())
	{
		if (const auto [flag, value] = wnd.try_read_key();
				flag && value == 'q')
		{
			bus.stop();
		}

		if (bus.pop_data(data_msg))
		{
			ui.update_accelleration(data_msg.raw_accelleration);
			ui.update_gyro(data_msg.raw_gyro);
		}
		else
		{
			std::this_thread::yield();
		}
		ui.update_queue_load(bus.queue_load());
	}

	hw_thread.join();
	return 0;
}

int hw_main(thread_bus &bus)
{
	wiringPiSetupGpio();

	mpu6050 sensor { device_id { 0b1101000 } };
	
	data_point_message msg;
	while (!bus.stop_flag())
	{
		sensor.update();
		msg.raw_accelleration = sensor.acceleration();
		msg.raw_gyro = sensor.gyro();
		bus.push_data(msg);

		//TODO: derive motor action
	}
	return 0;
}

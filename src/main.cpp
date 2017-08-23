
#include <cstdio>

#include <chrono>
#include <thread>
#include <functional>
#include <string_view>

#include <iostream>

#include <cursespp/cursespp.hpp>

#include "mpu6050.h"
#include "vector3.h"
#include "frontend.h"
#include "thread_bus.h"

#include <cmath>

using namespace reactionwheel;
using namespace std::string_view_literals;

int hw_main(thread_bus &bus);

int main()
{
	using namespace curspp;
	curses_session csession;

	auto wnd = window::standard_screen();
	wnd.refresh();
	frontend ui { wnd };

	thread_bus bus;
	std::thread hw_thread(hw_main, std::ref(bus));

	data_point_message data_msg;
	while (!bus.stop_flag())
	{
		if (const auto [flag, value] = wnd.try_read_key();
				flag && std::holds_alternative<char32_t>(value)
				&& std::get<char32_t>(value) == U'q')
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
	try
	{
		mpu6050 sensor { "/dev/i2c-1"sv, mpu6050::id::m0 };

		data_point_message msg;
		while (!bus.stop_flag())
		{
			sensor.update();
			msg.raw_accelleration = sensor.acceleration();
			msg.raw_gyro = sensor.gyro();
			bus.push_data(msg);

			//TODO: derive motor action
		}
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what();
        bus.stop();
    }

	return 0;
}

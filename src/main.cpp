
#include <cstdio>

#include <chrono>
#include <thread>
#include <sstream>
#include <functional>

#include <iostream>

#include "mpu6050.h"
#include "stepper.h"
#include "vector3.h"
#include "curses.hpp"
#include "frontend.h"
#include "thread_bus.h"

#include "crow_all.h"

#include <wiringPi.h>
#include <cmath>

using namespace reactionwheel;

int hw_main(thread_bus &bus);
void web_main(thread_bus &bus);

int main()
{
	using namespace curses;
	curses_session csession;

	auto &wnd = csession.session_window();
	wnd.refresh();
	frontend ui { wnd };

	thread_bus bus;
	std::thread hw_thread(hw_main, std::ref(bus));
	std::thread web_thread(web_main, std::ref(bus));

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
	web_thread.join();
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

void web_main(thread_bus &bus)
{
    crow::logger::setLogLevel(crow::LogLevel::ERROR);
	crow::SimpleApp webService;

	// pls take a moment to note and embrace the evil that could happen here
	bus.register_stop_hook([&webService]() { webService.stop(); });

	CROW_ROUTE(webService, "/sensor")([&bus](){
		const auto data = bus.last_read_message();
		const auto &accel = data.raw_accelleration;
		const auto &gyro = data.raw_gyro;
		std::stringstream fmt;
		fmt << "{\"acceleration\": ["
				<< accel.x << "," << accel.y << "," << accel.z
			<< "],\"gyro\": ["
				<< gyro.x << "," << gyro.y << "," << gyro.z
			<< "]}";
		crow::response res{fmt.str()};
		res.add_header("Access-Control-Allow-Origin", "*");
		return res;
	});

	webService.port(8080).run();
}

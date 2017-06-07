#include "stepper.h"

#include <chrono>
#include <thread>
#include <wiringPi.h>

namespace
{
	constexpr std::chrono::milliseconds step_time { 1 };
	
	void XFullStep(int pin)
	{
		digitalWrite(pin, 1);
		std::this_thread::sleep_for(step_time);
		digitalWrite(pin, 0);
	}	

	void XHalfStep(int pin1, int pin2)
	{
		digitalWrite(pin1, 1);
		digitalWrite(pin2, 1);
		std::this_thread::sleep_for(step_time);
		digitalWrite(pin1, 0);
		digitalWrite(pin2, 0);
	}
}


StepperDriver::StepperDriver()
	: StepperDriver(238, 236, 233, 231)
{
}

StepperDriver::StepperDriver(int a, int b, int c, int d)
	: mPosition(0)
{
	mPin[0] = a;
	mPin[1] = b;
	mPin[2] = c;
	mPin[3] = d;

	for (int i = 0; i < 4; ++i)
	{
		pinMode(mPin[i], 1);
		digitalWrite(mPin[i], 0);
	}
}

void StepperDriver::AdvancePositionBy(int num)
{
	int posCandidate = mPosition + num;
	int sign = posCandidate < 0;
	mPosition = (sign * -posCandidate + !sign * posCandidate) % 8;
}

void StepperDriver::HalfStep(Direction dir)
{
	AdvancePositionBy(static_cast<int>(dir));

	int pinIndex = mPosition / 2;
	if (mPosition % 2)
	{

		XHalfStep(mPin[pinIndex], mPin[(pinIndex+1)%4]);
	}
	else
	{
		XFullStep(mPin[pinIndex]);
	}

}

void StepperDriver::FullStep(Direction dir)
{
	int advance = static_cast<int>(dir);
	AdvancePositionBy(advance * (!(mPosition % 2) + 1));
	
	XFullStep(mPin[mPosition/2]);
}


#include "stepper.h"

#include <chrono>
#include <thread>
#include <wiringPi.h>

StepperDriver::StepperDriver()
	: StepperDriver(238, 236, 233, 231)
{
}

StepperDriver::StepperDriver(int a, int b, int c, int d)
	: mPin{a, b, c, d}
	, mPosition(0)
{
	for (int i = 0; i < 4; ++i)
	{
		pinMode(mPin[i], 1);
		digitalWrite(mPin[i], 0)
	}
	UpdatePositionSignal(true);
}

StepperDriver::~StepperDriver()
{
	Disable();
}

void StepperDriver::AdvancePositionBy(int num)
{
	UpdatePositionSignal(false);

	int posCandidate = mPosition + num;
	int sign = posCandidate < 0;
	mPosition = (sign * -posCandidate + !sign * posCandidate) % 8;

	UpdatePositionSignal(true);
}

void StepperDriver::UpdatePositionSignal(bool value)
{
	int pinIdx = mPosition / 2;
	digitalWrite(mPin[pinIdx], !!value);
	if (mPosition % 2)
	{
		digitalWrite(mPin[(pinIdx+1)%4], !!value);
	}
}

void StepperDriver::Disable()
{
	UpdatePositionSignal(false);
}

void StepperDriver::HalfStep(Direction dir)
{
	AdvancePositionBy(static_cast<int>(dir));
}

void StepperDriver::FullStep(Direction dir)
{
	auto advance = static_cast<int>(dir);
	AdvancePositionBy(advance * (!(mPosition % 2) + 1));
}

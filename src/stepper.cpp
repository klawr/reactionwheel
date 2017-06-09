#include "stepper.h"

#include <chrono>
#include <thread>
#include <wiringPi.h>

StepperDriver::StepperDriver() // StepperDriver() bekommt diese 4 Pins als default
	: StepperDriver(238, 236, 233, 231)
{
}

StepperDriver::StepperDriver(int a, int b, int c, int d) // Diese 4 default Pins werden hier als Array gespeichert.
	: mPin{a, b, c, d}
	, mPosition(0)
{
	for (int i = 0; i < 4; ++i)
	{
		pinMode(mPin[i], 1); // Die 4 Pins werden als aktiv gesetzt
		digitalWrite(mPin[i], 0); // Die 4 Pins werden auf 0/low gesetzt
	}
	UpdatePositionSignal(true);
}

StepperDriver::~StepperDriver()
{
	Disable();
}

void StepperDriver::AdvancePositionBy(int num) // bestimmt die naechste Position des Motots
{
	UpdatePositionSignal(false);

	int posCandidate = mPosition + num; // num ist entweder 1 oder -1 (Forward oder Backward)
	int sign = posCandidate < 0; //wenn sign negativ ist:
	mPosition = (sign * -posCandidate + !sign * posCandidate) % 8; //wird mPosition positiv, sonst bleibt er positiv. das wird % 8 gerechnet, sodass mPosition zwischen 0 und 7 ist
	
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

void StepperDriver::HalfStep(Direction dir) //definition der Halfstep Funktion. uebergeben wird die richtung, welche standardmaessig forwards ist
{
	AdvancePositionBy(static_cast<int>(dir)); //setzt die position des motors einen schritt weiter
}

void StepperDriver::FullStep(Direction dir) // das gleiche nochmal fuer fullsteps
{
	auto advance = static_cast<int>(dir);
	AdvancePositionBy(advance * (!(mPosition % 2) + 1)); // falls die momentane position ein halfstep ist, wird auf den naechsten fullstep gesetzt
}

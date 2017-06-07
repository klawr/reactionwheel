#pragma once

enum class Direction
{
	Forward = 1,
	Backward = -1,
};

class StepperDriver
{
public:
	StepperDriver();
	StepperDriver(int firstPinNo, int secondPinNo, int thirdPinNo, int fourthPinNo);

	void HalfStep(Direction dir = Direction::Forward);
	void FullStep(Direction dir = Direction::Forward);
	
private:
	void AdvancePositionBy(int num);

	int mPin[4];
	int mPosition;
};


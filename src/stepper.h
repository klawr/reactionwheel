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
	~StepperDriver();

	void HalfStep(Direction dir = Direction::Forward);
	void FullStep(Direction dir = Direction::Forward);
	
	void Disable();

private:
	void AdvancePositionBy(int num);
	void UpdatePositionSignal(bool value);

	int mPin[4];
	int mPosition;
};


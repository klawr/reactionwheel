
#include "mpu6050.h"
#include "stepper.h"

#include <wiringPi.h>

int main()
{
	wiringPiSetupGpio();
	//mpu6050 sensor { device_id { 0b1101000 } };
	
	StepperDriver driver;

	for (int i = 0; i < 2000; ++i)
	{
		driver.HalfStep();
	}
}


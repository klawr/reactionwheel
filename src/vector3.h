#pragma once

#include <cmath>

struct vector3
{
	int x;
	int y;
	int z;

	vector3()
		: vector3(0, 0, 0)
	{
	}	

	vector3(int x, int y, int z)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	float r() const
	{
		return sqrt(x*x + y*y + z*z);
	}
};


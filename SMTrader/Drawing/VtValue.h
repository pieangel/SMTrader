#pragma once
struct VtValue
{
	double x;
	double y;
	VtValue()
	{
		x = 0;
		y = 0;
	}
	VtValue(double ax, double ay)
	{
		x = ax;
		y = ay;
	}
};
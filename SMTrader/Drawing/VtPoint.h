#pragma once
struct VtPoint
{
	int x;
	int y;
	VtPoint() 
	{
		x = 0;
		y = 0;
	}
	VtPoint(int ax, int ay)
	{
		x = ax;
		y = ay;
	}

	void SetXY(int ax, int ay)
	{
		x = ax;
		y = ay;
	}
};


struct VtColor
{
	int A;
	int R;
	int G;
	int B;
};

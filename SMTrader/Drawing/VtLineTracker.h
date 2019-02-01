#pragma once
#include "VtBaseTracker.h"
class VtLineTracker :
	public VtBaseTracker
{
public:
	VtLineTracker(VtElement* e)
		: VtBaseTracker(e)
	{
		Setup();
	}

	VtLineTracker(XYChart* chart, VtElement* e)
		: VtBaseTracker(chart,e)
	{
		Setup(chart);
	}
	VtLineTracker();
	virtual ~VtLineTracker();
	void Setup();
	void Setup(XYChart* chart);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void MovePoint(int x, int y);
	virtual void MovePoint(XYChart* chart, int x, int y);
};


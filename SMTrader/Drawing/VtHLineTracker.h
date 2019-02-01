#pragma once
#include "VtBaseTracker.h"
class XYChart;
class VtHLineTracker :
	public VtBaseTracker
{
public:
	VtHLineTracker(VtElement* e)
		: VtBaseTracker(e)
	{
		Setup();
	}

	VtHLineTracker(XYChart* chart, VtElement* e)
		: VtBaseTracker(chart, e)
	{
		Setup();
	}

	VtHLineTracker();
	virtual ~VtHLineTracker();
	void Setup();
	void Setup(XYChart* chart);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
};


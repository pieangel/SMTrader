#pragma once
#include "VtBaseTracker.h"
class VtVLineTracker :
	public VtBaseTracker
{
public:
	VtVLineTracker(VtElement* e)
		: VtBaseTracker(e)
	{
		Setup();
	}

	VtVLineTracker(XYChart* chart, VtElement* e)
		: VtBaseTracker(chart, e)
	{
		Setup(chart);
	}

	VtVLineTracker();
	virtual ~VtVLineTracker();
	void Setup();
	void Setup(XYChart* chart);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
};


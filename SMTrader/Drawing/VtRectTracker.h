#pragma once
#include "VtBaseTracker.h"
#include "VtElement.h"
class VtRectTracker :
	public VtBaseTracker
{
public:
	VtRectTracker(VtElement* e);
	VtRectTracker(XYChart*chart, VtElement* e);
	VtRectTracker();
	virtual ~VtRectTracker();

	void Setup();
	void Setup(XYChart* chart);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
};


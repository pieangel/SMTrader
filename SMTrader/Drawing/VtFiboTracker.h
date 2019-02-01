#pragma once
#include "VtBaseTracker.h"
#include "VtElement.h"
class VtFiboTracker :
	public VtBaseTracker
{
public:
	VtFiboTracker(VtElement* e);
	VtFiboTracker(XYChart*chart, VtElement* e);
	VtFiboTracker();
	virtual ~VtFiboTracker();

	void Setup();
	void Setup(XYChart* chart);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void Redim(int x, int y, RedimStatus redim);
	virtual void MovePoint(int x, int y);
	virtual void MovePoint(XYChart* chart, int x, int y);
};


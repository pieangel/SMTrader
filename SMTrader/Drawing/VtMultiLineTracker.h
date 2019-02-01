#pragma once
#include "VtBaseTracker.h"
#include "VtMultiLine.h"
class VtMultiLineTracker :
	public VtBaseTracker
{
public:
	VtMultiLineTracker(VtElement* e)
		: VtBaseTracker(e)
	{
		Setup((VtMultiLine*)e);
	}

	VtMultiLineTracker(XYChart* chart, VtElement* e)
		: VtBaseTracker(chart, e)
	{
		Setup(chart, (VtMultiLine*)e);
	}
	virtual RedimStatus IsOver(int x, int y);
	VtMultiLineTracker();
	virtual ~VtMultiLineTracker();
	void Setup(VtMultiLine* el);
	void Setup(XYChart* chart, VtMultiLine* el);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void MovePoint(int x, int y);
	virtual void Move(int x, int y);
private:
	VtMultiLine* _Parent = nullptr;
};


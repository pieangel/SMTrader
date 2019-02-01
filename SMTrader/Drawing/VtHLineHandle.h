#pragma once
#include "VtHandle.h"
class XYChart;
class VtHLineHandle :
	public VtHandle
{
public:
	VtHLineHandle(VtElement* e, RedimStatus o)
		: VtHandle(e, o)
	{
		RePosition(e);
	}

	VtHLineHandle(XYChart* chart, VtElement* e, RedimStatus o)
		: VtHandle(chart, e, o)
	{
		RePosition(e);
	}

	VtHLineHandle();
	virtual ~VtHLineHandle();
	virtual void RePosition(VtElement* e);
	virtual void RePosition(XYChart* chart, VtElement* e);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual RedimStatus IsOver(int x, int y);
};


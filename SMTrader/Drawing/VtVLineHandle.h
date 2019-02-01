#pragma once
#include "VtHandle.h"
class VtVLineHandle :
	public VtHandle
{
public:
	VtVLineHandle(VtElement* e, RedimStatus o)
		: VtHandle(e, o)
	{
		RePosition(e);
	}

	VtVLineHandle(XYChart* chart, VtElement* e, RedimStatus o)
		: VtHandle(chart, e, o)
	{
		RePosition(chart, e);
	}

	VtVLineHandle();
	virtual ~VtVLineHandle();
	virtual void RePosition(VtElement* e);
	virtual void RePosition(XYChart* chart, VtElement* e);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual RedimStatus IsOver(int x, int y);
};


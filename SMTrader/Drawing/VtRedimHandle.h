#pragma once
#include "VtHandle.h"
class VtRedimHandle :
	public VtHandle
{
public:
	VtRedimHandle(VtElement* e, RedimStatus o)
		: VtHandle(e, o)
	{
		RePosition(e);
	}

	VtRedimHandle(XYChart* chart, VtElement* e, RedimStatus o)
		: VtHandle(chart, e, o)
	{
		RePosition(chart, e);
	}

	VtRedimHandle();
	virtual ~VtRedimHandle();
	virtual void RePosition(VtElement* e);
	virtual void RePosition(XYChart* chart, VtElement* e);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
};


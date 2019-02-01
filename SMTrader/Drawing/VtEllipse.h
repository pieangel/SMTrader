#pragma once
#include "VtElement.h"
class XYChart;
class VtEllipse :
	public VtElement
{
public:
	VtEllipse(VtEllipse& e)
		: VtElement(e)
	{
		;
	}
	VtEllipse(int x, int y, int x1, int y1);
	VtEllipse(XYChart* chart, int x, int y, int x1, int y1);
	VtEllipse();
	virtual ~VtEllipse();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual VtElement* Copy();
	static void DrawEllipse(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int clrFill);
};


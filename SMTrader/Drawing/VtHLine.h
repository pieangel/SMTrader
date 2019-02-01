#pragma once
#include "VtElement.h"
class XYChart;
class VtHLine :
	public VtElement
{
public:
	VtHLine(VtHLine& src)
		: VtElement(src)
	{

	}
	VtHLine(int y);
	VtHLine(XYChart* chart, int y);
	VtHLine();
	virtual ~VtHLine();
	virtual VtElement* Copy();
	virtual bool Contains(int x, int y);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	static void DrawHLine(VtPoint point, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
	void DrawOrderLine(VtPoint point, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
};


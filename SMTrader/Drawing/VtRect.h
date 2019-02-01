#pragma once
#include "VtElement.h"
class VtRect :
	public VtElement
{
public:
	VtRect(VtRect& src)
		: VtElement(src)
	{
	}
	VtRect(int x, int y, int x1, int y1);
	VtRect(XYChart* chart, int x, int y, int x1, int y1);
	VtRect();
	virtual ~VtRect();
	virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	static void DrawRect(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int clrFill);
	static void DrawRect(VtPoint start, VtPoint end, VtValue value1, VtValue value2, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int clrFill);
};


#pragma once
#include "VtElement.h"
class VtVLine :
	public VtElement
{
public:
	VtVLine(VtVLine& src)
		: VtElement(src)
	{

	}
	VtVLine(int x);
	VtVLine(XYChart* chart, int x);
	VtVLine();
	virtual ~VtVLine();
	virtual bool Contains(int x, int y);
	virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	static void DrawVLine(VtPoint point, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
};


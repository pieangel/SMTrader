#pragma once
#include "VtElement.h"
class VtLine :
	public VtElement
{
public:
	VtLine(VtLine& src)
		: VtElement(src)
	{

	}
	VtLine(int x, int y, int x1, int y1);
	VtLine(XYChart* chart, int x, int y, int x1, int y1);
	VtLine();
	virtual ~VtLine();
	virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void MovePoint(int dx, int dy, VtPoint* linkedPoint);
	virtual void MovePoint(XYChart* chart, int dx, int dy, VtPoint* linkedPoint);
	static void DrawLine(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
};


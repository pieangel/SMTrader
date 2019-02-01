#pragma once
#include "VtElement.h"
#include <list>
class XYChart;
const int PointCount = 6;
class VtFibo :
	public VtElement
{
public:
	VtFibo(VtFibo& src)
		: VtElement(src)
	{
		for (int i = 0; i < PointCount; i++)
		{
			PointList[i] = src.PointList[i];
		}
	}
	VtFibo(int x, int y, int x1, int y1);
	VtFibo(XYChart* chart, int x, int y, int x1, int y1);
	VtFibo();
	virtual ~VtFibo();
	//virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void Redim(int x, int y, RedimStatus redim);
	// Point list for the ratio
	VtPoint PointList[PointCount];

	virtual void CopyFrom(VtElement* ele);
	virtual VtElement* Copy();
private:
	void MakePoints();
public:
	static void MakePoints(VtPoint start, VtPoint end, VtPoint* points);
	static void DrawFibo(VtPoint start, VtPoint end, VtPoint* points, XYChart* chart, int dx, int dy, DrawArea* d, int clrPen, int penWidth);
};


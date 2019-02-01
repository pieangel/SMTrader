#pragma once
#include "VtElement.h"
#include <vector>
class VtMultiLine :
	public VtElement
{
public:
	VtMultiLine(VtMultiLine& src)
		: VtElement(src)
	{
		for (auto i = src.PointList.begin(); i != src.PointList.end(); ++i)
		{
			PointList.push_back(*i);
		}

		for (auto i = src.ValueList.begin(); i != src.ValueList.end(); ++i)
		{
			ValueList.push_back(*i);
		}
	}
	VtMultiLine(int x, int y, int x1, int y1, std::vector<VtPoint> pointList);
	VtMultiLine(XYChart* chart, int x, int y, int x1, int y1, std::vector<VtPoint> pointList);
	VtMultiLine();
	virtual ~VtMultiLine();

	virtual void MovePoint(int dx, int dy, VtPoint* linkedPoint);
	virtual bool Contains(int x, int y);
	virtual void CopyFrom(VtElement* ele);
	virtual VtElement* Copy();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	static void DrawMultiLine(std::vector<VtPoint> pointList, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
	static void DrawMultiLine(std::vector<VtPoint> pointList, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth);
	static bool Contains(int x, int y, std::vector<VtPoint> points);

	virtual void PixelToValue(int startID, int zoomStartIndex, XYChart* chart);
	virtual void ValueToPixel(int startID, int zoomStartIndex, XYChart* chart);

	std::vector<VtPoint> PointList;
	std::vector<VtValue> ValueList;
};


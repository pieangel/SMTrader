#pragma once
#include "VtPoint.h"
#include <vector>
#include "VtUniqueID.h"
#include "VtDrawingConstants.h"
#include "chartdir.h"
#include "VtValue.h"

class VtSymbolMaster;
struct VtOrder;
class VtElement : VtUniqueID
{
public:
	VtElement(VtElement& src)
	{
		ByValue = true;
		Value1 = src.Value1;
		Value2 = src.Value2;
		Location0 = src.Location0;
		Location1 = src.Location1;
		Start0 = src.Start0;
		Start1 = src.Start1;

		_EleType = src._EleType;
		_IsFlled = src._IsFlled;
		_IsLine = src._IsLine;
		_IsSelected = src._IsSelected;

		_PenColor = src._PenColor;
		_FillColor = src._FillColor;
		_PenWidth = src._PenWidth;

		_Chart = nullptr;
		_UndoElement = nullptr;
	}
	VtElement();
	virtual ~VtElement();
	ElementType EleType() const { return _EleType; }
	void EleType(ElementType val) { _EleType = val; }
	bool IsSelected() const { return _IsSelected; }
	void IsSelected(bool val) { _IsSelected = val; }
	bool IsLine() const { return _IsLine; }
	void IsLine(bool val) { _IsLine = val; }
	VtColor PenColor() const { return _PenColor; }
	void PenColor(VtColor val) { _PenColor = val; }
	int PenWidth() const { return _PenWidth; }
	void PenWidth(int val) { _PenWidth = val; }
	VtColor FillColor() const { return _FillColor; }
	void FillColor(VtColor val) { _FillColor = val; }

	//std::vector<VtValue> ValueList;
	bool IsFlled() const { return _IsFlled; }
	void IsFlled(bool val) { _IsFlled = val; }
	XYChart* Chart() const { return _Chart; }
	void Chart(XYChart* val) { _Chart = val; }
	VtElement* UndoElement() const { return _UndoElement; }
	void UndoElement(VtElement* val) { _UndoElement = val; }
	VtSymbolMaster* SymbolMaster() const { return _SymbolMaster; }
	void SymbolMaster(VtSymbolMaster* val) { _SymbolMaster = val; }
	VtOrder* Order() const { return _Order; }
	void Order(VtOrder* val) { _Order = val; }
public:
	static int ColorToInt(VtColor srcColor)
	{
		int color = 0;
		color |= srcColor.A << 24;
		color |= srcColor.R << 16;
		color |= srcColor.G << 8;
		color |= srcColor.B;
		return color;
	}

	static double FindDistanceToSegment(int x, int y, VtPoint p1, VtPoint p2, VtPoint& closest, double& within)
	{
		VtPoint pt(x, y);
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		if ((dx == 0) && (dy == 0))
		{
			// It's a point not a line segment.
			closest = p1;
			dx = pt.x - p1.x;
			dy = pt.y - p1.y;
			return sqrt(dx * dx + dy * dy);
		}
		// Calculate the t that minimizes the distance.
		double t = ((pt.x - p1.x) * dx + (pt.y - p1.y) * dy) /
			(dx * dx + dy * dy);
		// See if this represents one of the segment's
		// end points or a point in the middle.
		if (t < 0)
		{
			closest.x = p1.x;
			closest.y = p1.y;
			dx = pt.x - p1.x;
			dy = pt.y - p1.y;
		}
		else if (t > 1)
		{
			closest.x = p2.x;
			closest.y = p2.y;
			dx = pt.x - p2.x;
			dy = pt.y - p2.y;
		}
		else
		{
			closest.x = (int)(p1.x + t * dx);
			closest.y = (int)(p1.y + t * dy);
			dx = pt.x - closest.x;
			dy = pt.y - closest.y;
		}

		within = t;
		return sqrt(dx * dx + dy * dy);
	}

	static double FindDistanceToSegment(VtPoint pt, VtPoint p1, VtPoint p2, VtPoint& closest, double& within)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		if ((dx == 0) && (dy == 0))
		{
			// It's a point not a line segment.
			closest = p1;
			dx = pt.x - p1.x;
			dy = pt.y - p1.y;
			return sqrt(dx * dx + dy * dy);
		}
		// Calculate the t that minimizes the distance.
		double t = ((pt.x - p1.x) * dx + (pt.y - p1.y) * dy) /
			(dx * dx + dy * dy);
		// See if this represents one of the segment's
		// end points or a point in the middle.
		if (t < 0)
		{
			closest.x = p1.x;
			closest.y = p1.y;
			dx = pt.x - p1.x;
			dy = pt.y - p1.y;
		}
		else if (t > 1)
		{
			closest.x = p2.x;
			closest.y = p2.y;
			dx = pt.x - p2.x;
			dy = pt.y - p2.y;
		}
		else
		{
			closest.x = (int)(p1.x + t * dx);
			closest.y = (int)(p1.y + t * dy);
			dx = pt.x - closest.x;
			dy = pt.y - closest.y;
		}

		within = t;
		return sqrt(dx * dx + dy * dy);
	}

	static void CopyStdProp(VtElement& from, VtElement& to)
	{
		to.ByValue = true;
		to.Value1 = from.Value1;
		to.Value2 = from.Value2;
		to.Location0 = from.Location0;
		to.Location1 = from.Location1;
		to.Start0 = from.Start0;
		to.Start1 = from.Start1;

		to._EleType = from._EleType;
		to._IsFlled = from._IsFlled;
		to._IsLine = from._IsLine;
		to._IsSelected = from._IsSelected;

		to._PenColor = from._PenColor;
		to._FillColor = from._FillColor;
		to._PenWidth = from._PenWidth;
	}


	static double Distance(VtPoint pt0, int x, int y)
	{
		double dx = pt0.x - x;
		double dy = pt0.y - y;
		double down = pow(dx, 2.0) + pow(dy, 2.0);
		return sqrt(down);
	}

	static double Distance(VtPoint pt0, VtPoint pt1, int x, int y)
	{
		double dist = 0.0;
		VtPoint closest;
		double within;
		FindDistanceToSegment(x, y, pt0, pt1, closest, within);

		double up = (pt0.y - pt1.y) * x + (pt1.x - pt0.x) * y + (pt0.x * pt1.y - pt1.x * pt0.y);
		double down = pow((double)(pt1.x - pt0.x), 2.0) + pow((double)(pt1.y - pt0.y), 2.0);
		down = sqrt(down);
		dist = abs(up) / down;

		return dist;
	}

	static void GeneralLineEquation(VtPoint pt1, VtPoint pt2, int& a, int& b, int& c)
	{
		a = pt1.y - pt2.y;
		b = pt2.x - pt1.x;
		c = pt1.x * pt2.y - pt2.x * pt1.y;
	}


	virtual bool Contains(int x, int y);
	virtual void MovePoint(int x, int y, VtPoint* linkedPoint) {};
	virtual void Move(int x, int y);
	virtual void Redim(int x, int y, RedimStatus redim);
	virtual void EndMoveRedim();

	virtual void MovePoint(XYChart* chart,  int x, int y, VtPoint* linkedPoint) {};
	virtual void Move(XYChart* chart, int x, int y);
	virtual void Redim(XYChart* chart, int x, int y, RedimStatus redim);
	virtual void EndMoveRedim(XYChart* chart );

	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy) 
	{ 
		//ValueToPixel();
	};
	virtual void Select() { _IsSelected = true; };
	virtual void Select(int sx, int sy, int ex, int ey) { };
	virtual void DeSelect() { _IsSelected = false; };
	virtual void CopyFrom(VtElement* ele);
	virtual VtElement* Copy();

	int GetX0() { return Location0.x; }
	int GetY0() { return Location0.y; }
	int GetX1() { return Location1.x; }
	int GetY1() { return Location1.y; }
public:
	// For By value
	bool     ByValue = true;
	VtValue  Value1;
	VtValue  Value2;
	VtPoint Start0, Start1;
	VtPoint Location0, Location1;
	//virtual void PixelToValue(XYChart* chart);
	//virtual void ValueToPixel(XYChart* chart);
	virtual void PixelToValue(int startID, int zoomStartIndex, XYChart* chart);
	virtual void ValueToPixel(int startID, int zoomStartIndex, XYChart* chart);
protected:
	ElementType _EleType = ElementType::None;
	bool _IsSelected = false;
	bool _IsLine = false;
	bool _IsFlled = false;

	VtColor _PenColor;
	int _PenWidth = 1;
	VtColor _FillColor;

	XYChart* _Chart = nullptr;
private:
	VtElement* _UndoElement = nullptr;
protected:
	VtSymbolMaster* _SymbolMaster = nullptr;
	VtOrder* _Order = nullptr;
};


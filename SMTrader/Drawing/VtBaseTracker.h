#pragma once
#include "VtElement.h"
#include <list>
#include "VtHandle.h"
class XYChart;
class VtBaseTracker :
	public VtElement
{
public:
	VtBaseTracker();
	VtBaseTracker(VtElement* el)
	{
		Location0 = el->Location0;
		Location1 = el->Location1;
		_IsSelected = false;
		_IsLine = el->IsLine();
		_Parent = el;
		EndMoveRedim();
	}

	VtBaseTracker(XYChart* chart, VtElement* el)
	{
		Location0 = el->Location0;
		Location1 = el->Location1;
		_IsSelected = false;
		_IsLine = el->IsLine();
		_Parent = el;
		EndMoveRedim(chart);
	}
	virtual ~VtBaseTracker();

protected:
	std::list<VtHandle*> _Handles;

public:
	VtElement* _Parent = nullptr;
	virtual void MovePoint(int x, int y);
	virtual void EndMoveRedim();
	virtual void Move(int x, int y);
	virtual RedimStatus IsOver(int x, int y);
	virtual void Redim(int x, int y, RedimStatus red);

	virtual void MovePoint(XYChart* chart, int x, int y);
	virtual void EndMoveRedim(XYChart* chart);
	virtual void Move(XYChart* chart, int x, int y);
	virtual void Redim(XYChart* chart, int x, int y, RedimStatus red);

	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);

	virtual void PixelToValue(XYChart* chart);
	virtual void ValueToPixel(XYChart* chart);

	virtual void PixelToValue(int startID, int zoomStartIndex, XYChart* chart);
	virtual void ValueToPixel(int startID, int zoomStartIndex, XYChart* chart);
};


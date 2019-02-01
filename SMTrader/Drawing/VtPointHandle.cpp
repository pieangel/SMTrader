#include "stdafx.h"
#include "VtPointHandle.h"


VtPointHandle::VtPointHandle()
	: VtHandle()
{
}


VtPointHandle::VtPointHandle(XYChart* chart, VtElement* e, RedimStatus o, VtPoint* pt)
{
	_Parent = e;
	_LinkedPoint = pt;
	_Operation = o;
	Location0.x = pt->x;
	Location0.y = pt->y;
	//PixelToValue(chart);
}

VtPointHandle::~VtPointHandle()
{
}

VtPointHandle::VtPointHandle(VtElement * e, RedimStatus o, VtPoint * pt)
{
	_Parent = e;
	_LinkedPoint = pt;
	_Operation = o;
	Location0.x = pt->x;
	Location0.y = pt->y;
}

void VtPointHandle::RePosition(VtElement * e)
{
	Location0.x = _LinkedPoint->x;
	Location0.y = _LinkedPoint->y;

	Location1.x = Location0.x + HandleRadius;
	Location1.y = Location0.y + HandleRadius;
}

void VtPointHandle::RePosition(XYChart* chart, VtElement* e)
{
	Location0.x = _LinkedPoint->x;
	Location0.y = _LinkedPoint->y;

	Location1.x = Location0.x + HandleRadius;
	Location1.y = Location0.y + HandleRadius;

	//PixelToValue(chart);
}

void VtPointHandle::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtHandle::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtPointHandle::Move(int x, int y)
{
	VtHandle::Move(x, y);
}

void VtPointHandle::Move(XYChart* chart, int x, int y)
{
	VtHandle::Move(chart, x, y);
}

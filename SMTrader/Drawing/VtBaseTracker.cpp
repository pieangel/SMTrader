#include "stdafx.h"
#include "VtBaseTracker.h"
#include <algorithm>

VtBaseTracker::VtBaseTracker()
{
}


VtBaseTracker::~VtBaseTracker()
{
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		delete (*i);
}

void VtBaseTracker::MovePoint(int x, int y)
{
}

void VtBaseTracker::MovePoint(XYChart* chart, int x, int y)
{

}

void VtBaseTracker::EndMoveRedim()
{
	VtElement::EndMoveRedim();
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->EndMoveRedim();
}

void VtBaseTracker::EndMoveRedim(XYChart* chart)
{
	VtElement::EndMoveRedim(chart);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->EndMoveRedim(chart);
}

void VtBaseTracker::Move(int x, int y)
{
	VtElement::Move(x, y);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->RePosition(this);
}

void VtBaseTracker::Move(XYChart* chart, int x, int y)
{
	VtElement::Move(chart, x, y);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->RePosition(chart, this);
}

RedimStatus VtBaseTracker::IsOver(int x, int y)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	{
		RedimStatus ret = RedimStatus::None;
		ret = (*i)->IsOver(x, y);
		if (ret != RedimStatus::None)
			return ret;
	}
	return Contains(x, y) ? RedimStatus::MoveSelected : RedimStatus::Idle;
}

void VtBaseTracker::Redim(int x, int y, RedimStatus red)
{
	VtElement::Redim(x, y, red);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->RePosition(this);
	//for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	//	(*i)->Redim(x, y, red);
}

void VtBaseTracker::Redim(XYChart* chart, int x, int y, RedimStatus red)
{
	VtElement::Redim(chart, x, y, red);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->RePosition(chart, this);
}

void VtBaseTracker::Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	{
		if ((*i)->IsVisible)
		{
			(*i)->Draw(startID, zoomStartIndex, d, chart, dx, dy);
		}
	}
}

void VtBaseTracker::PixelToValue(XYChart* chart)
{
	//VtElement::PixelToValue(chart);
	//for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	//	(*i)->PixelToValue(chart);
}

void VtBaseTracker::PixelToValue(int startID, int zoomStartIndex, XYChart* chart)
{
	VtElement::PixelToValue(startID, zoomStartIndex, chart);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->PixelToValue(startID, zoomStartIndex, chart);
}

void VtBaseTracker::ValueToPixel(XYChart* chart)
{
	//TRACE("ValueToPixel\n");
	//VtElement::ValueToPixel(chart);
	//for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	//	(*i)->ValueToPixel(chart);
}

void VtBaseTracker::ValueToPixel(int startID, int zoomStartIndex, XYChart* chart)
{
	VtElement::ValueToPixel(startID, zoomStartIndex, chart);
	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->ValueToPixel(startID, zoomStartIndex, chart);
}

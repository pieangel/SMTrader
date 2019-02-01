#include "stdafx.h"
#include "VtMultiLineTracker.h"
#include "VtPointHandle.h"

VtMultiLineTracker::VtMultiLineTracker()
{
}


RedimStatus VtMultiLineTracker::IsOver(int x, int y)
{
	if (_Parent == nullptr)
		return RedimStatus::Idle;

	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
	{
		RedimStatus ret = RedimStatus::None;
		ret = (*i)->IsOver(x, y);
		if (ret != RedimStatus::None)
			return ret;
	}

	return _Parent->Contains(x, y) ? RedimStatus::MoveSelected : RedimStatus::Idle;

	//return VtMultiLine::Contains(x, y, _Parent->PointList) ? RedimStatus::MoveSelected : RedimStatus::Idle;
}

VtMultiLineTracker::~VtMultiLineTracker()
{
}

void VtMultiLineTracker::Setup(VtMultiLine* el)
{
	_Parent = el;
	for (size_t i = 0; i < el->PointList.size(); i++)
	{
		_Handles.push_back(new VtPointHandle(el, RedimStatus::MovePoint, &el->PointList[i]));
	}
}

void VtMultiLineTracker::Setup(XYChart* chart, VtMultiLine* el)
{
	_Parent = el;
	for (size_t i = 0; i < el->PointList.size(); i++)
	{
		_Handles.push_back(new VtPointHandle(chart, el, RedimStatus::MovePoint, &el->PointList[i]));
	}
}

void VtMultiLineTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtMultiLineTracker::MovePoint(int x, int y)
{
	if (_Parent == nullptr)
		return;

	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		if ((*i)->IsSelected())
		{
			(*i)->Move(x, y);
			((VtPointHandle*)(*i))->LinkedPoint()->SetXY((*i)->GetX0(), (*i)->GetY0());
		}
	}
}

void VtMultiLineTracker::Move(int x, int y)
{
	VtBaseTracker::Move(x, y);
	if (_Parent == nullptr)
		return;

	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		(*i)->Move(x, y);
		((VtPointHandle*)(*i))->LinkedPoint()->SetXY((*i)->GetX0(), (*i)->GetY0());
	}
}

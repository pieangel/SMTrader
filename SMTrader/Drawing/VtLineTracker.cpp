
#include "stdafx.h"
#include "VtLineTracker.h"
#include "VtPointHandle.h"

VtLineTracker::VtLineTracker()
{
}


VtLineTracker::~VtLineTracker()
{
}

void VtLineTracker::Setup()
{
	//NW
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtLineTracker::Setup(XYChart* chart)
{
	//NW
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtLineTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtLineTracker::MovePoint(int x, int y)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		if ((*i)->IsSelected())
		{
			(*i)->Move(x, y);
			((VtPointHandle*)(*i))->LinkedPoint()->SetXY((*i)->GetX0(), (*i)->GetY0());
		}
	}
}

void VtLineTracker::MovePoint(XYChart* chart, int x, int y)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		if ((*i)->IsSelected())
		{
			(*i)->Move(chart, x, y);
			if (_Parent != nullptr)
			{
				_Parent->MovePoint(chart, x, y, ((VtPointHandle*)(*i))->LinkedPoint());
			}
		}
	}
}


#include "stdafx.h"
#include "VtFiboTracker.h"
#include "VtRedimHandle.h"
#include "VtShapesCollection.h"
#include "VtPointHandle.h"

VtFiboTracker::VtFiboTracker(VtElement* e)
	: VtBaseTracker(e)
{
	Setup();
}

VtFiboTracker::VtFiboTracker()
	: VtBaseTracker()
{
}


VtFiboTracker::VtFiboTracker(XYChart*chart, VtElement* e)
	: VtBaseTracker(chart, e)
{
	Setup(chart);
}

VtFiboTracker::~VtFiboTracker()
{
}

void VtFiboTracker::Setup()
{
	//NW
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtFiboTracker::Setup(XYChart* chart)
{
	//NW
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtFiboTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtFiboTracker::Redim(int x, int y, RedimStatus redim)
{
	VtPoint* Loc0 = nullptr, *Loc1 = nullptr, *St0 = nullptr, *St1 = nullptr;

	if (Location0.y > Location1.y)
	{
		Loc0 = &Location1;
		Loc1 = &Location0;
		St0 = &Start1;
		St1 = &Start0;
	}
	else
	{
		Loc0 = &Location0;
		Loc1 = &Location1;
		St0 = &Start0;
		St1 = &Start1;
	}

	switch (redim)
	{
	case RedimStatus::NW:
		Loc0->x = St0->x + x;
		Loc0->y = St0->y + y;
		break;
	case RedimStatus::N:
		Loc0->y = St0->y + y;
		break;
	case RedimStatus::NE:
		Loc1->x = St1->x + x;
		Loc0->y = St0->y + y;
		break;
	case RedimStatus::E:
		Loc1->x = St1->x + x;
		break;
	case RedimStatus::SE:
		Loc1->x = St1->x + x;
		Loc1->y = St1->y + y;
		break;
	case RedimStatus::S:
		Loc1->y = St1->y + y;
		break;
	case RedimStatus::SW:
		Loc0->x = St0->x + x;
		Loc1->y = St1->y + y;
		break;
	case RedimStatus::W:
		Loc0->x = St0->x + x;
		break;
	case RedimStatus::MoveUpDown:
		Loc0->y = St0->y + y;
		break;
	case RedimStatus::MoveLeftRight:
		Loc0->x = St0->x + x;
		break;
	default:
		break;
	}

	for (auto i = _Handles.begin(); i != _Handles.end(); i++)
		(*i)->RePosition(this);

	_Parent->Location0.x = this->Location0.x;
	_Parent->Location0.y = this->Location0.y;
	_Parent->Location1.x = this->Location1.x;
	_Parent->Location1.y = this->Location1.y;
}


void VtFiboTracker::MovePoint(int x, int y)
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

void VtFiboTracker::MovePoint(XYChart* chart, int x, int y)
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

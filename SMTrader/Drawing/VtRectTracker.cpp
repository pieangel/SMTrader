#include "stdafx.h"
#include "VtRectTracker.h"
#include "VtRedimHandle.h"


VtRectTracker::VtRectTracker(VtElement* e)
	: VtBaseTracker(e)
{
	Setup();
}

VtRectTracker::VtRectTracker()
	: VtBaseTracker()
{
}


VtRectTracker::VtRectTracker(XYChart*chart, VtElement* e)
	: VtBaseTracker(chart, e)
{
	Setup(chart);
}

VtRectTracker::~VtRectTracker()
{
}

void VtRectTracker::Setup()
{
	//NW
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::NW));
	//SE
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::SE));
	//N
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::N));
	//NE
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::NE));
	//E
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::E));
	//S
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::S));
	//SW
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::SW));
	//W
	_Handles.push_back(new VtRedimHandle(this, RedimStatus::W));
}

void VtRectTracker::Setup(XYChart* chart)
{
	//NW
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::NW));
	//SE
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::SE));
	//N
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::N));
	//NE
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::NE));
	//E
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::E));
	//S
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::S));
	//SW
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::SW));
	//W
	_Handles.push_back(new VtRedimHandle(chart, this, RedimStatus::W));
}

void VtRectTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

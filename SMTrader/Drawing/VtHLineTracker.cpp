#include "stdafx.h"
#include "VtHLineTracker.h"
#include "VtHLineHandle.h"


VtHLineTracker::VtHLineTracker()
	: VtBaseTracker()
{
}


VtHLineTracker::~VtHLineTracker()
{
}

void VtHLineTracker::Setup()
{
	_Handles.push_back(new VtHLineHandle(this, RedimStatus::MoveUpDown));
}

void VtHLineTracker::Setup(XYChart* chart)
{
	_Handles.push_back(new VtHLineHandle(chart, this, RedimStatus::MoveUpDown));
}

void VtHLineTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

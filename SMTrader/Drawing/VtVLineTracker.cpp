#include "stdafx.h"
#include "VtVLineTracker.h"
#include "VtVLineHandle.h"


VtVLineTracker::VtVLineTracker()
{
}


VtVLineTracker::~VtVLineTracker()
{
}

void VtVLineTracker::Setup()
{
	_Handles.push_back(new VtVLineHandle(this, RedimStatus::MoveLeftRight));
}

void VtVLineTracker::Setup(XYChart* chart)
{
	_Handles.push_back(new VtVLineHandle(chart, this, RedimStatus::MoveLeftRight));
}

void VtVLineTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

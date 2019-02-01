#include "stdafx.h"
#include "VtVLineHandle.h"


VtVLineHandle::VtVLineHandle()
{
}


VtVLineHandle::~VtVLineHandle()
{
}

void VtVLineHandle::RePosition(VtElement * e)
{
	Location0.x = e->GetX0();
	Location0.y = 0;

	Location1.x = e->GetX0();
	Location1.y = 0;
}

void VtVLineHandle::RePosition(XYChart* chart, VtElement* e)
{
	Location0.x = e->GetX0();
	Location0.y = 0;

	Location1.x = e->GetX0();
	Location1.y = 0;

	//PixelToValue(chart);
}

void VtVLineHandle::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrFill = VtElement::ColorToInt(_FillColor);
	int clrSel = VtElement::ColorToInt(_SelPenColor);
	int clrBorder = VtElement::ColorToInt(_PenColor);
	//ValueToPixel(idStart, zoomStartIndex, chart);

	int penWidth = _PenWidth;
	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int halfY = (int)((plotAreaTopY + height) / 2.0);
	if (_IsSelected)
		d->circle(Location0.x + dx, halfY + dy, HandleRadius, HandleRadius, clrSel, clrFill);
	else
		d->circle(Location0.x + dx, halfY + dy, HandleRadius, HandleRadius, clrBorder, clrFill);
}

RedimStatus VtVLineHandle::IsOver(int x, int y)
{
	int dist = abs(x - Location0.x);
	if (dist <= HandleRadius)
		return _Operation;
	else
		return VtHandle::IsOver(x, y);
}

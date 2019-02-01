#include "stdafx.h"
#include "VtHLineHandle.h"
#include <math.h>


VtHLineHandle::VtHLineHandle()
{
}


VtHLineHandle::~VtHLineHandle()
{
}

void VtHLineHandle::RePosition(VtElement * e)
{
	Location0.x = 0;
	Location0.y = e->GetY0();

	Location1.x = 1000;
	Location1.y = e->GetX0();
}

void VtHLineHandle::RePosition(XYChart* chart, VtElement* e)
{
	Location0.x = 0;
	Location0.y = e->GetY0();

	Location1.x = 1000;
	Location1.y = e->GetX0();

	//PixelToValue(chart);
}

void VtHLineHandle::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrFill = VtElement::ColorToInt(_FillColor);
	int clrSel = VtElement::ColorToInt(_SelPenColor);
	int clrBorder = VtElement::ColorToInt(_PenColor);
	//ValueToPixel(idStart, zoomStartIndex, chart);

	int penWidth = _PenWidth;
	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int width = plotArea->getWidth();
	int halfX = (int)((plotAreaLeftX + width) / 2.0);
	if (_IsSelected)
		d->circle(halfX + dx, Location0.y + dy, HandleRadius, HandleRadius, clrSel, clrFill);
	else
		d->circle(halfX + dx, Location0.y + dy, HandleRadius, HandleRadius, clrBorder, clrFill);
}

RedimStatus VtHLineHandle::IsOver(int x, int y)
{
	int dist = abs(y - Location0.y);
	if (dist <= HandleRadius)
		return _Operation;
	else
		return VtHandle::IsOver(x, y);
}

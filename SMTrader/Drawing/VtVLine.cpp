#include "stdafx.h"
#include "VtVLine.h"
#include "../Util/formatstdstring.h"

VtVLine::VtVLine(int x)
{
	Location0.x = x;
	Location0.y = 0;
	Location1.x = x;
	Location1.y = 100;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::VLine;
}

VtVLine::VtVLine()
{
}


VtVLine::VtVLine(XYChart* chart, int x)
{
	Location0.x = x;
	Location0.y = 0;
	Location1.x = x;
	Location1.y = 100;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::VLine;
}

VtVLine::~VtVLine()
{
}

bool VtVLine::Contains(int x, int y)
{
	int dist = abs(x - Location0.x);
	if (dist <= SelectedRadius)
		return true;
	else
		return false;
}

VtElement * VtVLine::Copy()
{
	auto r = new VtVLine(*this);
	return r;
}

void VtVLine::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;
	//gfx.line((Location0.X + dx) * zoom, (Location0.Y + dy) * zoom, (Location1.X + dx) * zoom, (Location1.Y + dy) * zoom, clrPen, penWidth);
	//ValueToPixel(idStart, zoomStartIndex, chart);
	DrawVLine(Location0, d, chart, dx, dy, clrPen, penWidth);
}

void VtVLine::DrawVLine(VtPoint point, DrawArea * d, XYChart * chart, int dx, int dy, int clrPen, int penWidth)
{
	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaTopY = plotArea->getTopY() + chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int halfY = (int)(height / 2.0);
	d->line(point.x + dx, plotAreaTopY, point.x + dx, plotAreaTopY + height, clrPen, penWidth);

	std::string valueText = format(_T("%.2f"), chart->getXValue(point.x + dx));
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(point.x + dx + HandleRadius, halfY + HandleRadius, 0x000000);
	t->destroy();
}

#include "stdafx.h"
#include "VtEllipse.h"
#include "chartdir.h"
#include "../Util/formatstdstring.h"

VtEllipse::VtEllipse(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::Ellipse;
}

VtEllipse::VtEllipse()
{
}


VtEllipse::VtEllipse(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::Ellipse;
}

VtEllipse::~VtEllipse()
{
}

void VtEllipse::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int clrFill = VtElement::ColorToInt(_FillColor);
	//ValueToPixel(idStart, zoomStartIndex, chart);
	
	DrawEllipse(Location0, Location1, d, chart, dx, dy, clrPen, clrFill);
}

VtElement * VtEllipse::Copy()
{
	auto r = new VtEllipse(*this);
	return r;
}

void VtEllipse::DrawEllipse(VtPoint start, VtPoint end, DrawArea * d, XYChart * chart, int dx, int dy, int clrPen, int clrFill)
{
	d->circle(
		start.x + dx + (int)((end.x - start.x) / 2.0),
		start.y + dy + (int)((end.y - start.y) / 2.0),
		(int)((end.x - start.x) / 2.0),
		(int)((end.y - start.y) / 2.0), clrPen, clrFill);

	int halfX = (int)((end.x - start.x) / 2.0);
	int halfY = (int)((end.y - start.y) / 2.0);

	std::string font = _T("±¼¸²");
	std::string valueText = format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX + dx), chart->getYValue(start.y + dy));
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + halfX + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = format(_T("x = %.2f, y = %.2f"), chart->getXValue(halfX + dx), chart->getYValue(end.y + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + halfX + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = format(_T("x = %.2f, y = %.2f"), chart->getXValue(start.x + dx), chart->getYValue(halfY + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + dx + HandleRadius, start.y + halfY + dy + HandleRadius, 0x000000);
	t->destroy();
	valueText = format(_T("x = %.2f, y = %.2f"), chart->getXValue(end.x + dx), chart->getYValue(halfY + dy));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + dx + HandleRadius, start.y + halfY + dy + HandleRadius, 0x000000);
	t->destroy();
}



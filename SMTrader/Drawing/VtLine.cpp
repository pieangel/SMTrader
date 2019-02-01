#include "stdafx.h"
#include "VtLine.h"
#include "../Util/formatstdstring.h"

VtLine::VtLine(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::Line;
}

VtLine::VtLine()
{
}


VtLine::VtLine(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::Line;
}

VtLine::~VtLine()
{
}

VtElement * VtLine::Copy()
{
	auto r = new VtLine(*this);
	return r;
}

void VtLine::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	//VtElement::ValueToPixel();
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;

	CString msg;
	msg.Format(_T("LineCoor :: x = %d, y = %d\n"), Location0.x, Location0.y);
	//TRACE(msg);

	DrawLine(Location0, Location1, d, chart, dx, dy, clrPen, penWidth);
}

void VtLine::MovePoint(int dx, int dy, VtPoint* linkedPoint)
{
	if (linkedPoint != nullptr)
	{
		if (linkedPoint == &Location0)
		{
			Location0.x = Start0.x - dx;
			Location0.y = Start0.y - dy;
		}
		else
		{
			Location1.x = Start1.x - dx;
			Location1.y = Start1.y - dy;
		}
	}
}

void VtLine::MovePoint(XYChart* chart, int dx, int dy, VtPoint* linkedPoint)
{
	if (linkedPoint != nullptr)
	{
		if (linkedPoint == &Location0)
		{
			Location0.x = Start0.x - dx;
			Location0.y = Start0.y - dy;
		}
		else
		{
			Location1.x = Start1.x - dx;
			Location1.y = Start1.y - dy;
		}
	}
}


void VtLine::DrawLine(VtPoint start, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth)
{
	d->line(start.x + dx, start.y + dy, end.x + dx, end.y + dy, clrPen, penWidth);
	int extend = 100;
	d->line(start.x + dx - extend, start.y + dy, start.x + dx + extend, start.y + dy, clrPen, penWidth);
	d->line(end.x + dx - extend, end.y + dy, end.x + dx + extend, end.y + dy, clrPen, penWidth);

	double first = chart->getYValue(start.y + dy);
	double last = chart->getYValue(end.y + dy);
	double height = abs(last - first);

	
	std::string valueText = format(_T("%.2f"), first);
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = format(_T("%.2f"), last);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + dy + HandleRadius, 0x000000);
	t->destroy();

	valueText = format(_T("%.2f"), height);
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(start.x + HandleRadius, start.y + dy + HandleRadius*4, 0x0000ff);
	t->destroy();
}
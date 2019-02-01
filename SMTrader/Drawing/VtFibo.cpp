#include "stdafx.h"
#include "VtFibo.h"
#include <math.h>
#include <string>
#include "../Util/formatstdstring.h"
#include "VtShapesCollection.h"

VtFibo::VtFibo(int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;

	_IsSelected = true;
	MakePoints();
	EndMoveRedim();
	_EleType = EditOption::Fibo;
}

VtFibo::VtFibo()
{
}


VtFibo::VtFibo(XYChart* chart, int x, int y, int x1, int y1)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;

	_IsSelected = true;
	MakePoints();
	EndMoveRedim(chart);
	_EleType = EditOption::Fibo;
}

VtFibo::~VtFibo()
{
}

void VtFibo::CopyFrom(VtElement* ele)
{
	VtElement::CopyFrom(ele);
	for (int i = 0; i < PointCount; i++)
	{
		PointList[i] = ((VtFibo*)ele)->PointList[i];
	}
}

VtElement* VtFibo::Copy()
{
	VtFibo* r = new VtFibo(*this);
	for (int i = 0; i < PointCount; i++)
	{
		r->PointList[i] = PointList[i];
	}
	return r;
}

void VtFibo::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;
	MakePoints(Location0, Location1, PointList);
	DrawFibo(Location0, Location1, PointList, chart, dx, dy, d, clrPen, penWidth);
}

void VtFibo::Redim(int x, int y, RedimStatus redim)
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

	if (!_IsLine)
	{
		// manage redim limits
		if (Location1.x <= Location0.x)
			Location1.x = Location0.x + 10;
		if (Location1.y <= Location0.y)
			Location1.y = Location0.y + 10;
	}
}

void VtFibo::MakePoints()
{
	int deltaX = abs(Location1.x - Location0.x);
	int deltaY = abs(Location1.y - Location0.y);
	for (int i = 0; i < PointCount; i++)
	{
		int x = (int)(deltaX * i / (PointCount - 1));
		int y = (int)(deltaY * Ratio[i] * 10 / 1000);
		PointList[i].x = x;
		PointList[i].y = y;
	}
}

void VtFibo::MakePoints(VtPoint start, VtPoint end, VtPoint* points)
{
	int deltaX = abs(end.x - start.x);
	int deltaY = abs(end.y - start.y);
	int count = 6;
	for (int i = 0; i < count; i++)
	{
		int x = (int)(deltaX * i / (count - 1));
		int y = (int)(deltaY * Ratio[i] * 10 / 1000);
		points[i].x = x;
		points[i].y = y;
	}
}

void VtFibo::DrawFibo(VtPoint start, VtPoint end, VtPoint * points, XYChart * chart, int dx, int dy, DrawArea* d, int clrPen, int penWidth)
{
	VtFibo::MakePoints(start, end, points);
	int startY = start.y + dy;
	int yPos = 0;
	for (int i = 0; i < 6; i++)
	{
		if (start.y < end.y)
		{
			yPos = startY + dy + points[i].y;
		}
		else
		{
			yPos = startY + dy - points[i].y;
		}

		d->line(start.x + dx, yPos, end.x + dx, yPos, clrPen, penWidth);

		std::string valueText = format(_T("%.2f%% %.2f"), Ratio[i], chart->getYValue(yPos));
		std::string font = _T("±¼¸²");
		TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
		int startX = min(start.x + dx, end.x + dx);
		t->draw(startX + HandleRadius, yPos + HandleRadius, 0x000000);
		t->destroy();
	}

	// Show the height of the fibo.
	{
		std::string valueText = format(_T("height : %.2f"), abs(chart->getYValue(end.y) - chart->getYValue(start.y)));
		std::string font = _T("±¼¸²");
		TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
		int startX = max(start.x + dx, end.x + dx);
		int startY = min(start.y + dy, end.y + dy);
		t->draw(startX + HandleRadius, startY + HandleRadius, 0x000000);
		t->destroy();
	}

	d->line(start.x + dx, start.y + dy, end.x + dx, end.y + dy, clrPen, penWidth);
}

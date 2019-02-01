#include "stdafx.h"
#include "VtHLine.h"
#include "../Util/formatstdstring.h"
#include "Poco/NumberFormatter.h"
#include "../VtSymbolMaster.h"
#include "../VtOrder.h"
using Poco::NumberFormatter;

VtHLine::VtHLine(int y)
{
	Location0.x = 0;
	Location0.y = y;
	Location1.x = 100;
	Location1.y = y;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim();
	_EleType = EditOption::HLine;
}

VtHLine::VtHLine()
{
}


VtHLine::VtHLine(XYChart* chart, int y)
{
	Location0.x = 0;
	Location0.y = y;
	Location1.x = 100;
	Location1.y = y;
	_IsLine = true;
	_IsSelected = true;
	EndMoveRedim(chart);
	_EleType = EditOption::HLine;
}

VtHLine::~VtHLine()
{
}

VtElement * VtHLine::Copy()
{
	auto r = new VtHLine(*this);
	return r;
}

bool VtHLine::Contains(int x, int y)
{
	int dist = abs(y - Location0.y);
	if (dist <= SelectedRadius)
		return true;
	else
		return false;
}

void VtHLine::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;
	if (_SymbolMaster)
		DrawOrderLine(Location0, d, chart, dx, dy, clrPen, 2);
	else
		DrawHLine(Location0, d, chart, dx, dy, clrPen, penWidth);
}

void VtHLine::DrawHLine(VtPoint point, DrawArea * d, XYChart * chart, int dx, int dy, int clrPen, int penWidth)
{
	PlotArea* plotArea = chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
	int width = plotArea->getWidth();
	int halfX = (int)(width / 2.0);

	d->line(plotAreaLeftX, point.y + dy, plotAreaLeftX + width, point.y + dy, clrPen, penWidth);

	std::string valueText = format(_T("%.2f"), chart->getYValue(point.y + dy));
	std::string font = _T("±¼¸²");
	TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(halfX + HandleRadius, point.y + dy + HandleRadius, 0x000000);
	t->destroy();
}

void VtHLine::DrawOrderLine(VtPoint point, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth)
{
	if (_SymbolMaster)
	{
		PlotArea* plotArea = chart->getPlotArea();
		int plotAreaLeftX = plotArea->getLeftX() + chart->getAbsOffsetX();
		int width = plotArea->getWidth();
		int halfX = (int)(width / 2.0);

		if (_Order)
		{
			if (_Order->state == VtOrderState::Reserved)
			{
				if (_Order->orderPosition == VtPositionType::Buy)
				{
					_PenColor.A = 0;
					_PenColor.R = 255; 
					_PenColor.G = 105;
					_PenColor.B = 180;
				}
				else
				{
					_PenColor.A = 0;
					_PenColor.R = 0;
					_PenColor.G = 0;
					_PenColor.B = 180;
				}
			}
			else
			{
				if (_Order->orderPosition == VtPositionType::Buy)
				{
					_PenColor.A = 0;
					_PenColor.R = 238;
					_PenColor.G = 52;
					_PenColor.B = 54;
				}
				else
				{
					_PenColor.A = 0;
					_PenColor.R = 1;
					_PenColor.G = 135;
					_PenColor.B = 227;
				}
			}

			clrPen = VtElement::ColorToInt(_PenColor);
		}

		d->line(plotAreaLeftX, point.y + dy, plotAreaLeftX + width, point.y + dy, clrPen, penWidth);
		std::string valueText = NumberFormatter::format(chart->getYValue(point.y + dy), _SymbolMaster->decimal);
		std::string font = _T("±¼¸²");
		TTFText* t = d->text(valueText.c_str(), font.c_str(), 8);
		t->draw(halfX + HandleRadius, point.y + dy + HandleRadius, 0x000000);
		t->destroy();
	}
}

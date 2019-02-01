#include "stdafx.h"
#include "VtMultiLine.h"
#include "../Util/formatstdstring.h"

VtMultiLine::VtMultiLine(int x, int y, int x1, int y1, std::vector<VtPoint> pointList)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;

	for (auto i = pointList.begin(); i != pointList.end(); ++i)
	{
		PointList.push_back(*i);
	}

	EndMoveRedim();
	_EleType = EditOption::MultiLine;
}

VtMultiLine::VtMultiLine()
{
}


VtMultiLine::VtMultiLine(XYChart* chart, int x, int y, int x1, int y1, std::vector<VtPoint> pointList)
{
	Location0.x = x;
	Location0.y = y;
	Location1.x = x1;
	Location1.y = y1;
	_IsSelected = true;

	for (auto i = pointList.begin(); i != pointList.end(); ++i)
	{
		PointList.push_back(*i);
	}

	EndMoveRedim(chart);
	_EleType = EditOption::MultiLine;
}

VtMultiLine::~VtMultiLine()
{
}

void VtMultiLine::MovePoint(int dx, int dy, VtPoint* linkedPoint)
{
	for (size_t i = 0; i < PointList.size(); i++)
	{
		if (&PointList[i] == linkedPoint)
		{
			PointList[i].x = Start0.x - dx;
			PointList[i].y = Start0.y - dy;
			break;
		}
	}
}

bool VtMultiLine::Contains(int x, int y)
{
	/*
	double dist = 0.0;
	VtPoint closest;
	double within;
	
	for (size_t i = 0; i < PointList.size() - 1; i++)
	{
		VtPoint start = PointList[i];
		VtPoint end = PointList[i + 1];
		dist = VtElement::FindDistanceToSegment(x, y, start, end, closest, within);
		if (dist <= SelectedRadius)
			return true;
		else
			return false;
	}

	return false;
	*/
	return VtMultiLine::Contains(x, y, PointList);
}

bool VtMultiLine::Contains(int x, int y, std::vector<VtPoint> points)
{
	double dist = 0.0;
	VtPoint closest;
	double within;
	bool result = false;
	for (size_t i = 0; i < points.size() - 1; i++)
	{
		VtPoint start = points[i];
		VtPoint end = points[i + 1];
		dist = VtElement::FindDistanceToSegment(x, y, start, end, closest, within);
		if (dist <= SelectedRadius)
		{
			return true;
		}
	}
	return false;
}

void VtMultiLine::CopyFrom(VtElement* src)
{
	VtElement::CopyFrom(src);

	for (auto i = ((VtMultiLine*)src)->PointList.begin(); i != ((VtMultiLine*)src)->PointList.end(); ++i)
	{
		PointList.push_back(*i);
	}

	for (auto i = ((VtMultiLine*)src)->ValueList.begin(); i != ((VtMultiLine*)src)->ValueList.end(); ++i)
	{
		ValueList.push_back(*i);
	}
}

/*
void VtMultiLine::PixelToValue(XYChart* chart)
{
	ValueList.clear();
	for (size_t i = 0; i < PointList.size(); i++)
	{
		//d->line(PointList[i].x, PointList[i].y, PointList[i + 1].x, PointList[i + 1].y, clrPen, penWidth);
		VtValue val;
		val.x = chart->getXValue(PointList[i].x);
		val.y = chart->getYValue(PointList[i].y);
		ValueList.push_back(val);
	}
}
*/

void VtMultiLine::PixelToValue(int startID, int zoomStartIndex, XYChart* chart)
{
	ValueList.clear();
	for (size_t i = 0; i < PointList.size(); i++)
	{
		VtValue val;
		val.x = chart->getXValue(PointList[i].x) + startID + zoomStartIndex;
		val.y = chart->getYValue(PointList[i].y);

		ValueList.push_back(val);
	}
}

/*
void VtMultiLine::ValueToPixel(XYChart* chart)
{
	for (size_t i = 0; i < ValueList.size(); i++)
	{
		PointList[i].x = chart->getXCoor(ValueList[i].x);
		PointList[i].y = chart->getYCoor(ValueList[i].y);
	}
}
*/

void VtMultiLine::ValueToPixel(int startID, int zoomStartIndex, XYChart* chart)
{
	for (size_t i = 0; i < ValueList.size(); i++)
	{
		PointList[i].x = chart->getXCoor(ValueList[i].x - startID - zoomStartIndex);
		PointList[i].y = chart->getYCoor(ValueList[i].y);
	}
}

VtElement * VtMultiLine::Copy()
{
	auto r = new VtMultiLine(*this);
	return r;
}

void VtMultiLine::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrPen = VtElement::ColorToInt(_PenColor);
	int penWidth = _PenWidth;
	
	VtMultiLine::DrawMultiLine(PointList, d, chart, dx, dy, clrPen, penWidth);
}

void VtMultiLine::DrawMultiLine(std::vector<VtPoint> pointList, VtPoint end, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth)
{
	size_t size = pointList.size();
	if (size == 0)
		return;

	std::string valueText;
	std::string font = _T("±¼¸²");
	TTFText* t = nullptr;
	for (size_t i = 0; i < size - 1; i++)
	{
		d->line(pointList[i].x, pointList[i].y, pointList[i + 1].x, pointList[i + 1].y, clrPen, penWidth);
		valueText = format(_T("%.2f, %.2f"), chart->getXValue(pointList[i].x), chart->getYValue(pointList[i].y));
		t = d->text(valueText.c_str(), font.c_str(), 8);
		t->draw(pointList[i].x + HandleRadius, pointList[i].y + HandleRadius, 0x000000);
		t->destroy();
	}

	valueText = format(_T("%.2f, %.2f"), chart->getXValue(pointList[size - 1].x), chart->getYValue(pointList[size - 1].y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(pointList[size - 1].x + HandleRadius, pointList[size - 1].y + HandleRadius, 0x000000);
	t->destroy();

	d->line(pointList[size - 1].x, pointList[size - 1].y, end.x, end.y, clrPen, penWidth);
	valueText = format(_T("%.2f, %.2f"), chart->getXValue(end.x), chart->getYValue(end.y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(end.x + HandleRadius, end.y + HandleRadius, 0x000000);
	t->destroy();
}

void VtMultiLine::DrawMultiLine(std::vector<VtPoint> pointList, DrawArea* d, XYChart* chart, int dx, int dy, int clrPen, int penWidth)
{
	std::string valueText;
	std::string font = _T("±¼¸²");
	TTFText* t = nullptr;

	for (size_t i = 0; i < pointList.size() - 1; i++)
	{
		d->line(pointList[i].x, pointList[i].y, pointList[i + 1].x, pointList[i + 1].y, clrPen, penWidth);
		d->line(pointList[i].x, pointList[i].y, pointList[i + 1].x, pointList[i + 1].y, clrPen, penWidth);
		valueText = format(_T("%.2f, %.2f"), chart->getXValue(pointList[i].x), chart->getYValue(pointList[i].y));
		t = d->text(valueText.c_str(), font.c_str(), 8);
		t->draw(pointList[i].x + HandleRadius, pointList[i].y + HandleRadius, 0x000000);
		t->destroy();
	}

	valueText = format(_T("%.2f, %.2f"), chart->getXValue(pointList[pointList.size() - 1].x), chart->getYValue(pointList[pointList.size() - 1].y));
	t = d->text(valueText.c_str(), font.c_str(), 8);
	t->draw(pointList[pointList.size() - 1].x + HandleRadius, pointList[pointList.size() - 1].y + HandleRadius, 0x000000);
	t->destroy();
}

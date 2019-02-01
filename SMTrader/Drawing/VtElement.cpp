#include "stdafx.h"
#include "VtElement.h"
#include <math.h>
#include "VtDrawingConstants.h"
#include "VtShapesCollection.h"

VtElement::VtElement()
	: VtUniqueID()
{
	_FillColor.A = 0;
	_FillColor.R = 255;
	_FillColor.G = 255;
	_FillColor.B = 255;

	_PenColor.A = 0;
	_PenColor.R = 255;
	_PenColor.G = 255;
	_PenColor.B = 255;

	_SymbolMaster = nullptr;
	_Order = nullptr;
}


VtElement::~VtElement()
{
	if (_UndoElement != nullptr)
	{
		delete _UndoElement;
		_UndoElement = nullptr;
	}
}

#pragma region virtual Functions
bool VtElement::Contains(int x, int y)
{
	if (_IsLine)
	{
		double dist = 0.0;
		VtPoint closest;
		double within;
		dist = VtElement::FindDistanceToSegment(x, y, Location0, Location1, closest, within);
		//if (within < 0 || within > 1)
		//	return false;
		//dist = VtElement::Distance(Location0, Location1, x, y);
		if (dist <= SelectedRadius)
			return true;
		else
			return false;
	}
	else
	{
		bool inX = false;
		bool inY = false;
		int sx = Location0.x;
		int sy = Location0.y;
		int ex = Location1.x;
		int ey = Location1.y;
		VtShapesCollection::SanitizeRect(sx, sy, ex, ey);
		sx -= HandleRadius;
		ex += HandleRadius;
		sy -= HandleRadius;
		ey += HandleRadius;

		if (x >= sx && x <= ex)
			inX = true;

		if (y >= sy && y <= ey)
			inY = true;

		return (inX && inY);
	}
}

void VtElement::Move(int x, int y)
{
	Location0.x = Start0.x - x;
	Location0.y = Start0.y - y;
	Location1.x = Start1.x - x;
	Location1.y = Start1.y - y;

	//PixelToValue();
}

void VtElement::Move(XYChart* chart, int x, int y)
{
	Location0.x = Start0.x - x;
	Location0.y = Start0.y - y;
	Location1.x = Start1.x - x;
	Location1.y = Start1.y - y;

	//PixelToValue(chart);
}

void VtElement::Redim(int x, int y, RedimStatus redim)
{
	switch (redim)
	{
	case RedimStatus::NW:
		Location0.x = Start0.x + x;
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::N:
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::NE:
		Location1.x = Start1.x + x;
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::E:
		Location1.x = Start1.x + x;
		break;
	case RedimStatus::SE:
		Location1.x = Start1.x + x;
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::S:
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::SW:
		Location0.x = Start0.x + x;
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::W:
		Location0.x = Start0.x + x;
		break;
	case RedimStatus::MoveUpDown:
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::MoveLeftRight:
		Location0.x = Start0.x + x;
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

	//PixelToValue();
}

void VtElement::Redim(XYChart* chart, int x, int y, RedimStatus redim)
{
	switch (redim)
	{
	case RedimStatus::NW:
		Location0.x = Start0.x + x;
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::N:
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::NE:
		Location1.x = Start1.x + x;
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::E:
		Location1.x = Start1.x + x;
		break;
	case RedimStatus::SE:
		Location1.x = Start1.x + x;
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::S:
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::SW:
		Location0.x = Start0.x + x;
		Location1.y = Start1.y + y;
		break;
	case RedimStatus::W:
		Location0.x = Start0.x + x;
		break;
	case RedimStatus::MoveUpDown:
		Location0.y = Start0.y + y;
		break;
	case RedimStatus::MoveLeftRight:
		Location0.x = Start0.x + x;
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

	//PixelToValue(chart);
}

void VtElement::EndMoveRedim()
{
	Start0.x = Location0.x;
	Start0.y = Location0.y;
	Start1.x = Location1.x;
	Start1.y = Location1.y;

	//PixelToValue();
}

void VtElement::EndMoveRedim(XYChart* chart)
{
	Start0.x = Location0.x;
	Start0.y = Location0.y;
	Start1.x = Location1.x;
	Start1.y = Location1.y;

	//PixelToValue(chart);
}

void VtElement::CopyFrom(VtElement* src)
{
	ByValue = true;
	Value1 = src->Value1;
	Value2 = src->Value2;
	Location0 = src->Location0;
	Location1 = src->Location1;
	Start0 = src->Start0;
	Start1 = src->Start1;

	_EleType = src->_EleType;
	_IsFlled = src->_IsFlled;
	_IsLine = src->_IsLine;
	_IsSelected = src->_IsSelected;

	_PenColor = src->_PenColor;
	_FillColor = src->_FillColor;
	_PenWidth = src->_PenWidth;
}

VtElement* VtElement::Copy()
{
	VtElement* newEle = new VtElement(*this);
	return newEle;
}

/*
void VtElement::PixelToValue()
{
	if (_Chart != nullptr && ByValue)
	{
		Value1.x = _Chart->getXValue(Location0.x);
		Value1.y = _Chart->getYValue(Location0.y);
		Value2.x = _Chart->getXValue(Location1.x);
		Value2.y = _Chart->getYValue(Location1.y);
	}
}


void VtElement::PixelToValue(XYChart* chart)
{
	if (chart != nullptr && ByValue)
	{
		Value1.x = chart->getXValue(Location0.x);
		Value1.y = chart->getYValue(Location0.y);
		Value2.x = chart->getXValue(Location1.x);
		Value2.y = chart->getYValue(Location1.y);
	}
}
*/
void VtElement::PixelToValue(int startID, int zoomStartIndex, XYChart* chart)
{
	if (chart != nullptr && ByValue)
	{
		Value1.x = chart->getXValue(Location0.x) + startID + zoomStartIndex;
		Value1.y = chart->getYValue(Location0.y);
		Value2.x = chart->getXValue(Location1.x) + startID + zoomStartIndex;
		Value2.y = chart->getYValue(Location1.y);
	}
}

/*
void VtElement::ValueToPixel()
{
	if (_Chart != nullptr && ByValue)
	{
		Location0.x = _Chart->getXCoor(Value1.x);
		Location0.y = _Chart->getYCoor(Value1.y);
		Location1.x = _Chart->getXCoor(Value2.x);
		Location1.y = _Chart->getYCoor(Value2.y);
	}
}


void VtElement::ValueToPixel(XYChart* chart)
{
	if (chart != nullptr && ByValue)
	{
		Start0.x = Location0.x = chart->getXCoor(Value1.x);
		Start0.y = Location0.y = chart->getYCoor(Value1.y);
		Start1.x = Location1.x = chart->getXCoor(Value2.x);
		Start1.y = Location1.y = chart->getYCoor(Value2.y);
	}
}
*/
void VtElement::ValueToPixel(int startID, int zoomStartIndex, XYChart* chart)
{
	if (chart != nullptr && ByValue)
	{
		Start0.x = Location0.x = chart->getXCoor(Value1.x - startID - zoomStartIndex);
		Start0.y = Location0.y = chart->getYCoor(Value1.y);
		Start1.x = Location1.x = chart->getXCoor(Value2.x - startID - zoomStartIndex);
		Start1.y = Location1.y = chart->getYCoor(Value2.y);
	}
}

#pragma endregion virtual Functions


#include "stdafx.h"
#include "VtHandle.h"

VtHandle::VtHandle()
	:VtElement()
{
	_FillColor.A = 0;
	_FillColor.R = 255;
	_FillColor.G = 0;
	_FillColor.B = 0;

	_PenColor.A = 0;
	_PenColor.R = 255;
	_PenColor.G = 255;
	_PenColor.B = 0;

	_SelPenColor.A = 0;
	_SelPenColor.R = 0;
	_SelPenColor.G = 0;
	_SelPenColor.B = 255;
}


VtHandle::~VtHandle()
{
}

RedimStatus VtHandle::IsOver(int x, int y)
{
	double dist = 0.0;
	dist = VtElement::Distance(Location0, x, y);
	CString msg;
	msg.Format(_T("Distance = %.2f\n"), dist);
	//TRACE(msg);

	if (dist <= SelectedRadius)
	{
		_IsSelected = true;
		return _Operation;
	}
	else
	{
		_IsSelected = false;
		return RedimStatus::None;
	}
}

void VtHandle::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	int clrFill = VtElement::ColorToInt(_FillColor);
	int clrSel = VtElement::ColorToInt(_SelPenColor);
	int clrBorder = VtElement::ColorToInt(_PenColor);
	//ValueToPixel(idStart, zoomStartIndex, chart);
	if (_IsSelected)
		d->circle(Location0.x + dx, Location0.y + dy, HandleRadius, HandleRadius, clrSel, clrFill);
	else
		d->circle(Location0.x + dx, Location0.y + dy, HandleRadius, HandleRadius, clrBorder, clrFill);
}

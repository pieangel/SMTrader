#pragma once
#include "VtElement.h"
#include "VtPoint.h"
#include "VtDrawingConstants.h"
#include "chartdir.h"

class VtHandle :
	public VtElement
{
public:
	VtHandle(VtElement* e, RedimStatus o)
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

		_Operation = o;
		RePosition(e);
	}

	VtHandle(XYChart* chart, VtElement* e, RedimStatus o)
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

		_Operation = o;
		RePosition(chart, e);
	}

	VtHandle();
	virtual ~VtHandle();

protected:
	VtColor _SelPenColor;
	RedimStatus _Operation;
public:
	virtual void SyncPoint() {};
	virtual void RePosition(VtElement* e) {};
	virtual void RePosition(XYChart* chart, VtElement* e) {};
	bool IsVisible = true;
	virtual RedimStatus IsOver(int x, int y);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
};


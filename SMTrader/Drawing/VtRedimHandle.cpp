#include "stdafx.h"
#include "VtRedimHandle.h"
#include "VtShapesCollection.h"

VtRedimHandle::VtRedimHandle()
	: VtHandle()
{
}


VtRedimHandle::~VtRedimHandle()
{
}

void VtRedimHandle::RePosition(VtElement* e)
{
	/*
	switch (_Operation)
	{
	case RedimStatus::NW:
		Location0.x = e->GetX0();
		Location0.y = e->GetY0();
		break;
	case RedimStatus::N:
		Location0.x = e->GetX0() + ((e->GetX1() - e->GetX0()) / 2);
		Location0.y = e->GetY0();
		break;
	case RedimStatus::NE:
		Location0.x = e->GetX1();
		Location0.y = e->GetY0();
		break;
	case RedimStatus::E:
		Location0.x = e->GetX1();
		Location0.y = e->GetY0() + (e->GetY1() - e->GetY0()) / 2;
		break;
	case RedimStatus::SE:
		Location0.x = e->GetX1();
		Location0.y = e->GetY1();
		break;
	case RedimStatus::S:
		Location0.x = e->GetX0() + (e->GetX1() - e->GetX0()) / 2;
		Location0.y = e->GetY1();
		break;
	case RedimStatus::SW:
		Location0.x = e->GetX0();
		Location0.y = e->GetY1();
		break;
	case RedimStatus::W:
		Location0.x = e->GetX0();
		Location0.y = e->GetY0() + (e->GetY1() - e->GetY0()) / 2;
		break;
	}
	Location1.x = Location0.x + HandleRadius;
	Location1.y = Location0.y + HandleRadius;
	*/


	int sx, sy, ex, ey;
	sx = e->GetX0();
	sy = e->GetY0();
	ex = e->GetX1();
	ey = e->GetY1();

	VtShapesCollection::SanitizeRect(sx, sy, ex, ey);

	switch (_Operation)
	{
	case RedimStatus::NW:
		Location0.x = sx;
		Location0.y = sy;
		break;
	case RedimStatus::N:
		Location0.x = sx + (ex - sx) / 2;
		Location0.y = sy;
		break;
	case RedimStatus::NE:
		Location0.x = ex;
		Location0.y = sy;
		break;
	case RedimStatus::E:
		Location0.x = ex;
		Location0.y = sy + (ey - sy) / 2;
		break;
	case RedimStatus::SE:
		Location0.x = ex;
		Location0.y = ey;
		break;
	case RedimStatus::S:
		Location0.x = sx + (ex - sx) / 2;
		Location0.y = ey;
		break;
	case RedimStatus::SW:
		Location0.x = sx;
		Location0.y = ey;
		break;
	case RedimStatus::W:
		Location0.x = sx;
		Location0.y = sy + (ey - sy) / 2;
		break;
	}
	Location1.x = Location0.x + HandleRadius;
	Location1.y = Location0.y + HandleRadius;
}

void VtRedimHandle::RePosition(XYChart* chart, VtElement* e)
{
	int sx, sy, ex, ey;
	sx = e->GetX0();
	sy = e->GetY0();
	ex = e->GetX1();
	ey = e->GetY1();

	VtShapesCollection::SanitizeRect(sx, sy, ex, ey);

	switch (_Operation)
	{
	case RedimStatus::NW:
		Location0.x = sx;
		Location0.y = sy;
		break;
	case RedimStatus::N:
		Location0.x = sx + (ex - sx) / 2;
		Location0.y = sy;
		break;
	case RedimStatus::NE:
		Location0.x = ex;
		Location0.y = sy;
		break;
	case RedimStatus::E:
		Location0.x = ex;
		Location0.y = sy + (ey - sy) / 2;
		break;
	case RedimStatus::SE:
		Location0.x = ex;
		Location0.y = ey;
		break;
	case RedimStatus::S:
		Location0.x = sx + (ex - sx) / 2;
		Location0.y = ey;
		break;
	case RedimStatus::SW:
		Location0.x = sx;
		Location0.y = ey;
		break;
	case RedimStatus::W:
		Location0.x = sx;
		Location0.y = sy + (ey - sy) / 2;
		break;
	}
	Location1.x = Location0.x + HandleRadius;
	Location1.y = Location0.y + HandleRadius;
}

void VtRedimHandle::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtHandle::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

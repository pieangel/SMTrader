#include "stdafx.h"
#include "VtShapesCollection.h"
#include <algorithm>
#include "VtRectTracker.h"
#include "VtMultiLineTracker.h"
#include "VtHLineTracker.h"
#include "VtVLineTracker.h"
#include "VtLineTracker.h"
#include "VtMultiLine.h"
#include "VtRect.h"
#include "VtLine.h"
#include "VtVLine.h"
#include "VtVLineTracker.h"
#include "VtHLine.h"
#include "VtHLineTracker.h"
#include "VtEllipse.h"
#include "VtFibo.h"
#include <math.h>
#include "../Chart/ChartViewer.h"
#include "VtFiboTracker.h"
#include "../VtOrder.h"

VtShapesCollection::VtShapesCollection()
{
	InitUndoBuff();
}


VtShapesCollection::~VtShapesCollection()
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		delete *i;
		*i = nullptr;
	}

	if (Tracker != nullptr)
		delete Tracker;
	if (undoB != nullptr)
	{
		delete undoB;
	}
}

void VtShapesCollection::DeSelect()
{
	for (auto i = List.begin(); i != List.end(); i++)
		(*i)->IsSelected(false);
	SelectedElement = nullptr;
	if (Tracker != nullptr)
	{
		delete Tracker;
		Tracker = nullptr;
	}
}

VtElement * VtShapesCollection::GetCopyOfSelectedElement()
{
	if (SelectedElement == nullptr)
		return nullptr;
	auto el = SelectedElement->Copy();
	return el;
}

void VtShapesCollection::CopySelected(int dx, int dy)
{
	if (SelectedElement == nullptr)
		return;
	auto ele = GetCopyOfSelectedElement();
	ele->Move(dx, dy);
	DeSelect();
	List.push_back(ele);

	Tracker = new VtRectTracker(ele);
	SelectedElement = ele;
	SelectedElement->EndMoveRedim();
}

void VtShapesCollection::RemoveElement(VtElement * ele)
{
	if (ele != nullptr)
	{
		VtElement* temp = ele;
		List.remove(ele);

		delete temp;
	}
}

void VtShapesCollection::ClearElements()
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		delete (*i);
	}

	List.clear();
}

void VtShapesCollection::MovePoint(int dx, int dy)
{
	if (Tracker != nullptr)
		Tracker->MovePoint(dx, dy);
}

void VtShapesCollection::MovePoint(XYChart* chart, int dx, int dy)
{
	if (Tracker != nullptr)
		Tracker->MovePoint(dx, dy);
}

void VtShapesCollection::Move(int dx, int dy)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			(*i)->Move(dx, dy);
	}

	if (Tracker != nullptr)
		Tracker->Move(dx, dy);
}

void VtShapesCollection::Move(XYChart* chart, int dx, int dy)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			(*i)->Move(dx, dy);
	}
}

void VtShapesCollection::EndMove()
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
		{
			(*i)->EndMoveRedim();
			storeDo(BuffOp::Update, *i);
		}
	}
}

void VtShapesCollection::EndMove(XYChart* chart)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if (!(*i)->IsSelected())
			continue;
		(*i)->EndMoveRedim();
	}
}

void VtShapesCollection::Redim(int x, int y, RedimStatus redim)
{
	
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			(*i)->Redim(x, y, redim);
	}
	
	if (Tracker != nullptr)
		Tracker->Redim(x, y, redim);
}

std::list<VtElement*>* VtShapesCollection::GetSelectedList()
{
	auto tempList = new std::list<VtElement*>();
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			tempList->push_back(*i);
	}
	return tempList;
}

void VtShapesCollection::Click(int x, int y)
{
	SelectedElement = nullptr;
	if (Tracker != nullptr)
	{
		delete Tracker;
		Tracker = nullptr;
	}

	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			(*i)->DeSelect();
		if ((*i)->Contains(x, y))
			SelectedElement = *i;
	}

	if (SelectedElement == nullptr)
		return;
	SelectedElement->Select();

	if (SelectedElement->EleType() == ElementType::MultiLine)
		Tracker = new VtMultiLineTracker(SelectedElement);
	else if (SelectedElement->EleType() == ElementType::HLine)
		Tracker = new VtHLineTracker(SelectedElement);
	else if (SelectedElement->EleType() == ElementType::VLine)
		Tracker = new VtVLineTracker(SelectedElement);
	else if (SelectedElement->EleType() == ElementType::Line)
		Tracker = new VtLineTracker(SelectedElement);
	else if (SelectedElement->EleType() == ElementType::Fibo)
		Tracker = new VtFiboTracker(SelectedElement);
	else
		Tracker = new VtRectTracker(SelectedElement);
}

void VtShapesCollection::Click(XYChart* chart, int x, int y)
{
	SelectedElement = nullptr;
	if (Tracker != nullptr)
	{
		delete Tracker;
		Tracker = nullptr;
	}

	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected())
			(*i)->DeSelect();
		if ((*i)->Contains(x, y))
			SelectedElement = *i;
	}

	if (SelectedElement == nullptr)
		return;
	SelectedElement->Select();

	if (SelectedElement->EleType() == ElementType::MultiLine)
		Tracker = new VtMultiLineTracker(chart, SelectedElement);
	else if (SelectedElement->EleType() == ElementType::HLine)
		Tracker = new VtHLineTracker(chart, SelectedElement);
	else if (SelectedElement->EleType() == ElementType::VLine)
		Tracker = new VtVLineTracker(chart, SelectedElement);
	else if (SelectedElement->EleType() == ElementType::Line)
		Tracker = new VtLineTracker(chart, SelectedElement);
	else if (SelectedElement->EleType() == ElementType::Fibo)
		Tracker = new VtFiboTracker(SelectedElement);
	else
		Tracker = new VtRectTracker(chart, SelectedElement);
}

void VtShapesCollection::SanitizeRect(int & sx, int & sy, int & ex, int & ey)
{
	int startX = min(sx, ex);
	int startY = min(sy, ey);
	int endX = max(sx, ex);
	int endY = max(sy, ey);
	sx = startX;
	sy = startY;
	ex = endX;
	ey = endY;
}

void VtShapesCollection::MultiSelect(int startX, int startY, int endX, int endY)
{
	SanitizeRect(startX, startY, endX, endY);
	if (Tracker != nullptr)
	{
		delete Tracker;
		Tracker = nullptr;
	}

	SelectedElement = nullptr;

	for (auto i = List.begin(); i != List.end(); ++i)
	{
		VtElement* ele = *i;
		ele->DeSelect();
		int x = ele->GetX0();
		int x1 = ele->GetX1();
		int y = ele->GetY0();
		int y1 = ele->GetY1();
		if (x > x1)
			std::swap(x, x1);
		if (y > y1)
			std::swap(y, y1);
		bool inRect = (x <= endX && x1 >= startX && y <= endY && y1 >= startY);
		if (!inRect)
			continue;
		SelectedElement = ele;
		SelectedElement->Select();
	}

	if (SelectedElement != nullptr)
	{
		if (SelectedElement->EleType() == ElementType::MultiLine)
			Tracker = new VtMultiLineTracker(SelectedElement);
		else if (SelectedElement->EleType() == ElementType::HLine)
			Tracker = new VtHLineTracker(SelectedElement);
		else if (SelectedElement->EleType() == ElementType::VLine)
			Tracker = new VtVLineTracker(SelectedElement);
		else if (SelectedElement->EleType() == ElementType::Line)
			Tracker = new VtVLineTracker(SelectedElement);
		else if (SelectedElement->EleType() == ElementType::Fibo)
			Tracker = new VtFiboTracker(SelectedElement);
		else
			Tracker = new VtRectTracker(SelectedElement);
	}
}

void VtShapesCollection::AddMultiLine(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, std::vector<VtPoint> pointList)
{
	DeSelect();
	auto r = new VtMultiLine(x, y, x1, y1, pointList);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtMultiLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddMultiLine(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, std::vector<VtPoint> pointList)
{
	DeSelect();
	auto r = new VtMultiLine(x, y, x1, y1, pointList);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtMultiLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddRect(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled)
{
	if (x1 - HandleRadius <= x)
		x1 = x + HandleRadius;
	if (y1 - HandleRadius <= y)
		y1 = y + HandleRadius;

	DeSelect();
	
	auto r = new VtRect(x, y, x1, y1);

	CString msg;
	msg.Format(_T("Address = %x\n"), r);
	//TRACE(msg);

	if (r != nullptr)
	{
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
		r->IsFlled(filled);
	}

	List.push_back(r);
	
	Tracker = new VtRectTracker(r);
	msg.Format(_T("Address = %x\n"), Tracker);
	//TRACE(msg);
	SelectedElement = r;
	SelectedElement->Select();
	
}

void VtShapesCollection::AddRect(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled)
{
	if (x1 - HandleRadius <= x)
		x1 = x + HandleRadius;
	if (y1 - HandleRadius <= y)
		y1 = y + HandleRadius;

	DeSelect();

	auto r = new VtRect(chart, x, y, x1, y1);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
		r->IsFlled(filled);
	}

	List.push_back(r);
	Tracker = new VtRectTracker(chart,r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddLine(int x, int y, int x1, int y1, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtLine(x, y, x1, y1);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}
	List.push_back(r);

	Tracker = new VtLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddLine(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtLine(chart, x, y, x1, y1);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}
	List.push_back(r);

	Tracker = new VtLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddVLine(int x, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtVLine(x);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->PenWidth(penWidth);

		List.push_back(r);

		Tracker = new VtVLineTracker(r);

		SelectedElement = r;
		SelectedElement->Select();
	}
}

void VtShapesCollection::AddVLine(XYChart* chart, int x, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtVLine(x);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->PenWidth(penWidth);

		List.push_back(r);

		Tracker = new VtVLineTracker(r);

		SelectedElement = r;
		SelectedElement->Select();
	}
}

void VtShapesCollection::AddHLine(int y, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtHLine(y);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtHLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddHLine(XYChart* chart, int y, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtHLine(y);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtHLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

VtElement* VtShapesCollection::AddHLineByOrder(XYChart* chart, int y, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtHLine(y);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtHLineTracker(r);
	SelectedElement = r;
	SelectedElement->Select();

	return r;
}

VtElement* VtShapesCollection::FindObjectByOrder(VtOrder* order)
{
	if (!order)
		return nullptr;

	for (auto it = List.begin(); it != List.end(); ++it)
	{
		VtElement* ele = *it;
		if (ele->Order() == order)
		{
			return ele;
		}
	}

	return nullptr;
}

VtElement* VtShapesCollection::FindObjectByOrderNo(std::string orderNo)
{
	for (auto it = List.begin(); it != List.end(); ++it)
	{
		VtElement* ele = *it;
		if (ele->Order()->orderNo.compare(orderNo) == 0)
		{
			return ele;
		}
	}

	return nullptr;
}

void VtShapesCollection::AddEllipse(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled)
{
	if (x1 - HandleRadius <= x)
		x1 = x + HandleRadius;
	if (y1 - HandleRadius <= y)
		y1 = y + HandleRadius;

	DeSelect();
	auto r = new VtEllipse(x, y, x1, y1);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
		r->IsFlled(filled);
	}

	List.push_back(r);
	storeDo(BuffOp::Insert, r);

	Tracker = new VtRectTracker(r);

	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddEllipse(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled)
{
	if (x1 - HandleRadius <= x)
		x1 = x + HandleRadius;
	if (y1 - HandleRadius <= y)
		y1 = y + HandleRadius;

	DeSelect();
	auto r = new VtEllipse(x, y, x1, y1);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->FillColor(fillC);
		r->PenWidth(penWidth);
		r->IsFlled(filled);
	}

	List.push_back(r);

	Tracker = new VtRectTracker(r);

	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddFibo(int x, int y, int x1, int y1, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtFibo(x, y, x1, y1);
	if (r != nullptr)
	{
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtFiboTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::AddFibo(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, int penWidth)
{
	DeSelect();
	auto r = new VtFibo(x, y, x1, y1);
	if (r != nullptr)
	{
		r->Chart(chart);
		r->PenColor(penC);
		r->PenWidth(penWidth);
	}

	List.push_back(r);

	Tracker = new VtRectTracker(r);
	SelectedElement = r;
	SelectedElement->Select();
}

void VtShapesCollection::DrawUnselected(int idStart, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if (!(*i)->IsSelected())
		{
			(*i)->Draw(idStart, zoomStartIndex, d, chart, dx, dy);
		}

	}
}

void VtShapesCollection::DrawSelected(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	bool almostOneSelected = false;
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if (!(*i)->IsSelected())
			continue;
		(*i)->Draw(startID, zoomStartIndex, d, chart, dx, dy);
		almostOneSelected = true;
	}

	if (almostOneSelected && Tracker != nullptr)
		Tracker->Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtShapesCollection::PixelToValue(XYChart* chart)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		//(*i)->PixelToValue(chart);
	}

	if (Tracker != nullptr)
		Tracker->PixelToValue(chart);
}

void VtShapesCollection::PixelToValue(int startID, int zoomStartIndex, XYChart* chart)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		(*i)->PixelToValue(startID, zoomStartIndex, chart);
	}

	if (Tracker != nullptr)
		Tracker->PixelToValue(startID, zoomStartIndex, chart);
}

void VtShapesCollection::ValueToPixel(XYChart* chart)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		//(*i)->ValueToPixel(chart);
	}

	if (Tracker != nullptr)
		Tracker->ValueToPixel(chart);
}

void VtShapesCollection::ValueToPixel(int startID, int zoomStartIndex, XYChart* chart)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		(*i)->ValueToPixel(startID, zoomStartIndex, chart);
	}

	if (Tracker != nullptr)
		Tracker->ValueToPixel(startID, zoomStartIndex, chart);
}

void VtShapesCollection::SaveDimension(CChartViewer * a_pChartViewer, int dataSize, XYChart* chart)
{
	if (a_pChartViewer == nullptr || dataSize == 0 || chart == nullptr)
		return;
	xmin = chart->xAxis()->getMinValue();
	xmax = chart->xAxis()->getMaxValue();
	ymin = chart->yAxis()->getMinValue();
	ymax = chart->yAxis()->getMaxValue();

	startIndex = (int)(a_pChartViewer->getViewPortLeft() * dataSize);
}

bool VtShapesCollection::CheckChangeDimension(CChartViewer * a_pChartViewer, int dataSize, XYChart* chart)
{
	if (a_pChartViewer == nullptr || dataSize == 0 || chart == nullptr)
		return false;

	double nxmin = chart->xAxis()->getMinValue();
	double nxmax = chart->xAxis()->getMaxValue();
	double nymin = chart->yAxis()->getMinValue();
	double nymax = chart->yAxis()->getMaxValue();

	int nstartIndex = (int)(a_pChartViewer->getViewPortLeft() * dataSize);

	bool changed = false;
	if (nxmin != xmin)
		changed = true;
	if (nxmax != xmax)
		changed = true;
	if (nymin != ymin)
		changed = true;
	if (nymax != ymax)
		changed = true;
	if (nstartIndex != startIndex)
		changed = true;

	if (changed)
	{
		SaveDimension(a_pChartViewer, dataSize, chart);
	}

	return changed;
}

void VtShapesCollection::DeleteSelectedObject()
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i)->IsSelected() && (*i) == SelectedElement)
		{
			storeDo(BuffOp::Delete, *i);
			delete *i;
			List.remove(*i);
			break;
		}
	}
}

void VtShapesCollection::DeleteAllObjects()
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		delete *i;
	}

	List.clear();
}

void VtShapesCollection::DeleteElement(VtElement* ele)
{
	for (auto i = List.begin(); i != List.end(); i++)
	{
		if ((*i) == ele)
		{
			delete *i;
			List.remove(*i);
			break;
		}
	}
}

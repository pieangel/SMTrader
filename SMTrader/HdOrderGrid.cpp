#include "stdafx.h"
#include "HdOrderGrid.h"
#include "Resource.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "Poco/NumberFormatter.h"
#include "Poco/String.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"
#include "VtOrderManagerSelector.h"
#include "VtGlobal.h"

using Poco::NumberFormatter;
using Poco::trim;

HdOrderGrid::HdOrderGrid()
{
}


HdOrderGrid::~HdOrderGrid()
{
}


void HdOrderGrid::OnSetup()
{
	//m_CancelBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_CANCEL));
	_defFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	SetTH_NumberRows(2);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			//m_data[yIndex].m_colData[xIndex] = rand() % 100;
			CString msg;
			msg.Format(_T("%d, %d value"), yIndex, xIndex);
			//QuickSetText(xIndex, yIndex, msg);
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER);
			if (xIndex == 0)
			{
				QuickSetBitmap(xIndex, yIndex, &m_CancelBitmap);
			}
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void HdOrderGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdOrderGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[12] = { "종목코드", "주문종류", "가격", "수량", "미체결", "수량", "평균단가", "상태", "접수시각", "주문번호", "원주문번호", "주문타입" };
	int colWidth[12] = { 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 };


	for (int i = 0; i < _ColCount; i++)
	{
		if (i <= 1 || i >= 7)
		{
			SetColWidth(i, colWidth[i]);
			QuickSetText(i, -2, title[i]);
			GetCell(i, -2, &cell);
			cell.SetBackColor(VtGlobal::GridTitleBackColor);
			cell.SetTextColor(VtGlobal::GridTitleTextColor);
			SetCell(i, -2, &cell);
			QuickSetFont(i, -2, &_titleFont);
			RedrawCell(i, -2);
		}
	}

	JoinCells(2, -2, 4, -2);
	GetCell(2, -2, &cell);
	cell.SetBackColor(RGB(70, 130, 180));
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetText(_T("주문"));
	SetCell(2, -2, &cell);
	RedrawCell(2, -2);
	QuickSetText(2, -1, title[2]);
	QuickSetText(3, -1, title[3]);
	QuickSetText(4, -1, title[4]);
	RedrawCell(2, -1);
	RedrawCell(3, -1);
	RedrawCell(4, -1);
	JoinCells(5, -2, 6, -2);
	GetCell(5, -2, &cell);
	cell.SetBackColor(RGB(70, 130, 180));
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetText(_T("체결"));
	SetCell(5, -2, &cell);
	RedrawCell(5, -2);

	QuickSetText(5, -1, title[5]);
	QuickSetText(6, -1, title[6]);
	RedrawCell(5, -1);
	RedrawCell(6, -1);
}

void HdOrderGrid::InitGrid()
{
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->ActiveOrderManager();
	int row = 0;
	CUGCell cell;
	for (auto it = orderMgr->AcceptedMap.begin(); it != orderMgr->AcceptedMap.end(); ++it)
	{
		VtOrder* order = it->second;
		QuickSetText(1, row, order->shortCode.c_str());
		std::string position;
		switch (order->orderPosition)
		{
		case VtPositionType::Buy:
			position = _T("매수");
			break;
		case VtPositionType::Sell:
			position = _T("매도");
			break;
		default:
			position = _T("오류");
			break;
		}
		QuickSetText(2, row, position.c_str());

		std::string remainQty = NumberFormatter::format(order->unacceptedQty, 5);
		remainQty = trim(remainQty);
		QuickSetText(3, row, remainQty.c_str());

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByShortCode(order->shortCode);
		std::string price;
		if (symMaster)
			price = NumberFormatter::format(order->orderPrice, 20, symMaster->decimal);
		else
			price = NumberFormatter::format(order->orderPrice, 20, 2);
		price = trim(price);
		QuickSetText(4, row, price.c_str());

		std::string amount = NumberFormatter::format(order->amount, 5);
		amount = trim(amount);
		QuickSetText(5, row, amount.c_str());

		QuickSetText(6, row, order->tradeTime.c_str());
		std::string orderNo = NumberFormatter::format(order->orderNo, 5);
		orderNo = trim(orderNo);
		QuickSetText(7, row, orderNo.c_str());

		std::string oriOrderNo = NumberFormatter::format(order->oriOrderNo, 5);
		oriOrderNo = trim(oriOrderNo);
		QuickSetText(8, row, oriOrderNo.c_str());

		if (order->state == VtOrderState::Accepted)
			QuickSetText(9, row, _T("접수"));
		else if (order->state == VtOrderState::Filled)
			QuickSetText(9, row, _T("체결"));
		else if (order->state == VtOrderState::ConfirmModify)
			QuickSetText(9, row, _T("정정확인"));
		else if (order->state == VtOrderState::ConfirmCancel)
			QuickSetText(9, row, _T("취소확인"));
		else if (order->state == VtOrderState::Ledger)
			QuickSetText(9, row, _T("원장접수"));
		else if (order->state == VtOrderState::Preordered)
			QuickSetText(9, row, _T("예약접수"));

		GetCell(0, row, &cell);
		cell.Tag((void*)order);

		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		QuickRedrawCell(4, row);
		QuickRedrawCell(5, row);
		QuickRedrawCell(6, row);
		QuickRedrawCell(7, row);
		QuickRedrawCell(8, row);
		QuickRedrawCell(9, row);

		row++;
	}

	DataCount(row);
}

void HdOrderGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdOrderGrid::ResetCellText()
{
	for (int i = 0; i < DataCount(); i++)
	{
		for (int j = 1; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickRedrawCell(j, i);
		}
	}
}


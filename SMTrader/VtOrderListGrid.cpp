#include "stdafx.h"
#include "VtOrderListGrid.h"
#include "VtGlobal.h"
#include <string>
#include "VtTotalOrderManager.h"
#include "VtOrder.h"
#include "XFormatNumber.h"
#include "VtGlobal.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"

using Poco::NumberFormatter;

VtOrderListGrid::VtOrderListGrid()
{
}


VtOrderListGrid::~VtOrderListGrid()
{
}

void VtOrderListGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CenterFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CursorFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);


	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, 18);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}


	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetColTitle();
	SetVS_Width(0);
	SetHS_Height(0);

	EnableMenu(TRUE);

	InitGrid();
}

int VtOrderListGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtOrderListGrid::SetColTitle()
{
	const int ColCount = _ColCount;

	JoinCells(0, 0, 0, 1); // 종목코드
	JoinCells(1, 0, 1, 1); // 주문종류
	JoinCells(2, 0, 4, 0); // 주문
	JoinCells(5, 0, 6, 0); // 체결
	JoinCells(7, 0, 7, 1); // 상태
	JoinCells(8, 0, 8, 1); // 접수 시각
	JoinCells(9, 0, 9, 1); // 주문번호
	JoinCells(10, 0, 10, 1); // 원주문번호
	JoinCells(11, 0, 11, 1); // 주문타입
	QuickSetText(0, 0, _T("종목코드"));
	QuickSetText(1, 0, _T("주문종류"));
	QuickSetText(2, 0, _T("주문"));
	QuickSetText(5, 0, _T("체결"));
	QuickSetText(7, 0, _T("상태"));
	QuickSetText(8, 0, _T("접수시작"));
	QuickSetText(9, 0, _T("주문번호"));
	QuickSetText(10, 0, _T("원주문번호"));
	QuickSetText(11, 0, _T("주문타입"));
	QuickSetText(2, 1, _T("가격"));
	QuickSetText(3, 1, _T("수량"));
	QuickSetText(4, 1, _T("미체결"));
	QuickSetText(5, 1, _T("수량"));
	QuickSetText(6, 1, _T("평균단가"));

	std::vector < std::pair<int, int>> cellPosVec;
	cellPosVec.push_back(std::make_pair(0, 0));
	cellPosVec.push_back(std::make_pair(1, 0));
	cellPosVec.push_back(std::make_pair(2, 0));
	cellPosVec.push_back(std::make_pair(5, 0));
	cellPosVec.push_back(std::make_pair(7, 0));
	cellPosVec.push_back(std::make_pair(8, 0));
	cellPosVec.push_back(std::make_pair(9, 0));
	cellPosVec.push_back(std::make_pair(10, 0));
	cellPosVec.push_back(std::make_pair(11, 0));
	cellPosVec.push_back(std::make_pair(2, 1));
	cellPosVec.push_back(std::make_pair(3, 1));
	cellPosVec.push_back(std::make_pair(4, 1));
	cellPosVec.push_back(std::make_pair(5, 1));
	cellPosVec.push_back(std::make_pair(6, 1));
	CUGCell cell;
	for (auto it = cellPosVec.begin(); it != cellPosVec.end(); ++it) {
		std::pair<int, int> pos = *it;
		GetCell(std::get<0>(pos), std::get<1>(pos), &cell);
		cell.SetBackColor(RGB(25, 25, 25));
		cell.SetTextColor(RGB(255, 255, 255));
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_defFont);
		SetCell(std::get<0>(pos), std::get<1>(pos), &cell);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}	
}

void VtOrderListGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtOrderListGrid::InitGrid()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::map<int, VtOrder*>& orderMap = totalOrderMgr->GetOrderMap();
	int i = 2;
	// 주문을 역순으로 표시
	for (auto it = orderMap.rbegin(); it != orderMap.rend(); ++it) {
		VtOrder* order = it->second;
		// 종목코드
		QuickSetText(0, i, order->shortCode.c_str());
		QuickRedrawCell(0, i);
		// 주문종류
		if (order->orderType == VtOrderType::New) {
			QuickSetText(1, i, _T("신규"));
		}
		else if (order->orderType == VtOrderType::Change) {
			QuickSetText(1, i, _T("정정"));
		}
		else if (order->orderType == VtOrderType::Cancel) {
			QuickSetText(1, i, _T("취소"));
		}
		QuickRedrawCell(1, i);

		// 주문가격
		VtSymbol* sym = symMgr->FindSymbol(order->shortCode);
		CUGCell cell;
		GetCell(2, i, &cell);
		std::string temp = NumberFormatter::format(order->orderPrice, sym->IntDecimal);
		CString thVal = XFormatNumber(temp.c_str(), sym->IntDecimal);
		cell.SetText(thVal);
		SetCell(2, i, &cell);
		QuickRedrawCell(2, i);

		// 주문 수량
		QuickSetNumber(3, i, order->amount);
		QuickRedrawCell(3, i);
		// 주문 미체결
		QuickSetNumber(4, i, order->unacceptedQty);
		QuickRedrawCell(4, i);
		// 체결 수량
		QuickSetNumber(5, i, order->filledQty);
		QuickRedrawCell(5, i);
		// 체결 가격
		cell;
		GetCell(6, i, &cell);
		temp = NumberFormatter::format(order->filledPrice, sym->IntDecimal);
		thVal = XFormatNumber(temp.c_str(), sym->IntDecimal);
		cell.SetText(thVal);
		SetCell(6, i, &cell);
		QuickRedrawCell(6, i);
		// 주문 상태
		if (order->state == VtOrderState::Accepted)
			QuickSetText(7, i, _T("접수"));
		else if (order->state == VtOrderState::Filled)
			QuickSetText(7, i, _T("체결"));
		else if (order->state == VtOrderState::ConfirmModify)
			QuickSetText(7, i, _T("정정확인"));
		else if (order->state == VtOrderState::ConfirmCancel)
			QuickSetText(7, i, _T("취소확인"));
		else if (order->state == VtOrderState::Ledger)
			QuickSetText(7, i, _T("원장접수"));
		else if (order->state == VtOrderState::Preordered)
			QuickSetText(7, i, _T("예약접수"));
		else if (order->state == VtOrderState::Settled)
			QuickSetText(7, i, _T("청산"));
		QuickRedrawCell(7, i);
		// 접수시간
		QuickSetText(8, i, order->tradeTime.c_str());
		QuickRedrawCell(8, i);
		// 주문번호
		QuickSetNumber(9, i, order->orderNo);
		QuickRedrawCell(9, i);
		// 원주문번호
		QuickSetNumber(10, i, order->oriOrderNo);
		QuickRedrawCell(10, i);
		// 주문타입
		if (order->priceType == VtPriceType::Price)
			QuickSetText(11, i, _T("지정가"));
		else 
			QuickSetText(11, i, _T("시장가"));
		QuickRedrawCell(11, i);
		i++;
	}
}

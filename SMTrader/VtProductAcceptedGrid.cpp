#include "stdafx.h"
#include "VtProductAcceptedGrid.h"
#include "Resource.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "Poco/NumberFormatter.h"
#include "Poco/String.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"
#include "VtOrderManagerSelector.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"
#include "VtGlobal.h"

using Poco::NumberFormatter;
using Poco::trim;

VtProductAcceptedGrid::VtProductAcceptedGrid()
{
}


VtProductAcceptedGrid::~VtProductAcceptedGrid()
{
}


void VtProductAcceptedGrid::OnSetup()
{
	//m_CancelBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_CANCEL));
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetCellType(0, -1, UGCT_CHECKBOX);
	QuickSetCellTypeEx(0, -1, UGCT_CHECKBOXCHECKMARK);
	QuickSetBackColor(0, -1, RGB(255, 255, 255));
	QuickSetAlignment(0, -1, UG_ALIGNVCENTER | UG_ALIGNCENTER);
	QuickRedrawCell(0, -1);
	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		QuickSetCellType(0, yIndex, UGCT_CHECKBOX);
		QuickSetCellTypeEx(0, yIndex, UGCT_CHECKBOXCHECKMARK);
		QuickSetBackColor(0, yIndex, RGB(255, 255, 255));
		QuickSetAlignment(0, yIndex, UG_ALIGNVCENTER | UG_ALIGNCENTER);
		QuickRedrawCell(0, yIndex);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			//m_data[yIndex].m_colData[xIndex] = rand() % 100;
			CString msg;
			msg.Format(_T("%d, %d value"), yIndex, xIndex);
			//QuickSetText(xIndex, yIndex, msg);
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER);
			
		}
	}
	SetVS_Width(0);
	SetHS_Height(0);
	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtProductAcceptedGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	QuickSetCellType(0, -1, UGCT_CHECKBOX);
	QuickSetCellTypeEx(0, -1, UGCT_CHECKBOXCHECKMARK);
	QuickSetBackColor(0, -1, RGB(255, 255, 255));
	QuickSetAlignment(0, -1, UG_ALIGNVCENTER | UG_ALIGNCENTER);
	QuickRedrawCell(0, -1);
}

void VtProductAcceptedGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn)
	{
		CString msg;
		msg = QuickGetText(1, row);
		if (msg.GetLength() > 0)
		{
			std::string shorCode = msg;
			VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
			VtSymbol* sym = symMgr->FindSymbol(shorCode);
// 			if (sym && _OrderLeftWnd && _OrderLeftWnd->ParentOrderWnd())
// 			{
// 				_OrderLeftWnd->ParentOrderWnd()->ResetSymbol(sym);
// 			}
		}
	}
}

/// <summary>
/// cell.GetNumber() 0 : 체크 박스 해제, 1 : 체크박스 체크
/// </summary>
/// <param name="ID"></param>
/// <param name="col"></param>
/// <param name="row"></param>
/// <param name="msg"></param>
/// <param name="param"></param>
/// <returns></returns>
int VtProductAcceptedGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (row == -1)
	{
		for (int i = 0; i < _DataCount; i++)
		{
			CUGCell cell;
			GetCell(0, i, &cell);
			int style = 0;
			if (cell.IsPropertySet(UGCELL_CELLTYPEEX_SET))
				style = cell.GetCellTypeEx();

			int val = (int)cell.GetNumber();
			cell.SetNumber((val = (val + 1) % ((style & UGCT_CHECKBOX3STATE) ? 3 : 2)));
			SetCell(0, i, &cell);
			RedrawCell(0, i);
		}
	}
	return TRUE;
}

void VtProductAcceptedGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "취소", "종목", "구분" , "미체결" }; //, "주문가격", "주문량", "주문시간", "주문번호", "원주문번호" };
	int colWidth[4] = { 24, 35, 35, 63 }; //, 60, 60, 60, 60, 60 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		if (i != 0) {
			cell.SetBackColor(VtGlobal::GridTitleBackColor);
			cell.SetTextColor(VtGlobal::GridTitleTextColor);
		}
		else {
			cell.SetBackColor(RGB(255, 255, 255));
			cell.SetTextColor(RGB(255, 255, 255));
		}
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

void VtProductAcceptedGrid::SetAcptOrderList()
{
	if (!_OrderConfigMgr->OrderMgr())
		return;

	ResetCellText();
	int row = 0;
	CUGCell cell;
	for (auto it = _OrderConfigMgr->OrderMgr()->AcceptedMap.begin(); it != _OrderConfigMgr->OrderMgr()->AcceptedMap.end(); ++it)
	{
		VtOrder* order = it->second;
		QuickSetText(1, row, order->shortCode.c_str());
		std::string position;
		switch (order->orderPosition)
		{
		case VtPositionType::Buy:
		{
			position = _T("매수");
			QuickSetTextColor(1, row, RGB(255, 0, 0));
			QuickSetTextColor(2, row, RGB(255, 0, 0));
			QuickSetTextColor(3, row, RGB(255, 0, 0));
		}
			break;
		case VtPositionType::Sell:
		{
			position = _T("매도");
			QuickSetTextColor(1, row, RGB(0, 0, 255));
			QuickSetTextColor(2, row, RGB(0, 0, 255));
			QuickSetTextColor(3, row, RGB(0, 0, 255));
		}
			break;
		default:
		{
			position = _T("오류");
		}
			break;
		}
		QuickSetText(2, row, position.c_str());

		std::string remainQty = NumberFormatter::format(order->unacceptedQty, 5);
		remainQty = trim(remainQty);
		QuickSetText(3, row, remainQty.c_str());
		QuickSetAlignment(3, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		/*
		std::string price;
		if (_OrderConfigMgr->Master())
			price = NumberFormatter::format(order->orderPrice, 20, _OrderConfigMgr->Master()->decimal);
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
		*/
		GetCell(0, row, &cell);
		cell.Tag((void*)order);
		SetCell(0, row, &cell);
		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		//QuickRedrawCell(4, row);
		//QuickRedrawCell(5, row);
		//QuickRedrawCell(6, row);
		//QuickRedrawCell(7, row);
		//QuickRedrawCell(8, row);

		row++;
	}

	DataCount(row);
}

void VtProductAcceptedGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtProductAcceptedGrid::ResetCellText()
{
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		cell.Tag(nullptr);
		cell.SetNumber(0);
		SetCell(0, i, &cell);
		RedrawCell(0, i);
		for (int j = 1; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickRedrawCell(j, i);
		}
	}
}

void VtProductAcceptedGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtProductAcceptedGrid::CancelOrder(VtOrder* order)
{
	if (!order)
		return;
	_OrderConfigMgr->OrderMgr()->DirectCancelOrder(order);
}

void VtProductAcceptedGrid::CancelOrderList()
{
	if (_SelectedOrderList.size() == 0)
		return;

	for (auto it = _SelectedOrderList.begin(); it != _SelectedOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		CancelOrder(order);
	}

	_SelectedOrderList.clear();
}

void VtProductAcceptedGrid::CancelOrderAll()
{
	if (_DataCount == 0)
		return;

	_SelectedOrderList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		VtOrder* order = (VtOrder*)cell.Tag();
		_SelectedOrderList.push_back(order);
	}

	GetCell(0, -1, &cell);
	cell.SetNumber(0);
	SetCell(0, -1, &cell);
	RedrawCell(0, -1);

	ResetCellText();

	CancelOrderList();
}

void VtProductAcceptedGrid::CancelOrderSelected()
{
	if (_DataCount == 0)
		return;
	_SelectedOrderList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		int val = (int)cell.GetNumber();
		VtOrder* order = (VtOrder*)cell.Tag();
		if (val == 1 && order)
		{
			_SelectedOrderList.push_back(order);
		}
	}

	CancelOrderList();
}


#include "stdafx.h"
#include "VtRemainOrderGrid.h"
#include "Resource.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "Poco/NumberFormatter.h"
#include "Poco/String.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"
#include "VtSymbol.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtPosition.h"
#include "VtOrderManagerSelector.h"
#include "VtGlobal.h"


using Poco::NumberFormatter;
using Poco::trim;

VtRemainOrderGrid::VtRemainOrderGrid()
{
}


VtRemainOrderGrid::~VtRemainOrderGrid()
{
}


void VtRemainOrderGrid::OnSetup()
{
	//m_OutBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_OUT));
	_defFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

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
				QuickSetBitmap(xIndex, yIndex, &m_OutBitmap);
			}
		}
	}

	SetVS_Width(0);
	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtRemainOrderGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtRemainOrderGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[8] = { "청산", "종목", "구분", "수량", "평가손익", "평균가", "현재가", "청산가능수량" };
	int colWidth[8] = { 26, 60, 60, 60, 60, 60, 60, 60 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
}

void VtRemainOrderGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtRemainOrderGrid::ResetCellText()
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

void VtRemainOrderGrid::SetRemainList()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->ActiveAccount();
	if (!acnt)
		return;	
	int row = 0;
	CUGCell cell;
	
	for (auto it = acnt->PositionMap.begin(); it != acnt->PositionMap.end(); ++it)
	{
		VtPosition* posi = it->second;
		VtSymbol* sym = symMgr->FindSymbolByFullCode(posi->FullCode);
		if (!sym) continue;
		if (posi->OpenQty == 0 ) continue;
		

		QuickSetText(1, row, sym->ShortCode.c_str());
		std::string position;
		switch (posi->Position)
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

		QuickSetNumber(3, row, posi->OpenQty);

		VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByShortCode(sym->ShortCode);
		if (symMaster)
		{
			std::string temp = NumberFormatter::format(posi->OpenProfitLoss, 20, symMaster->decimal);
			QuickSetText(4, row, temp.c_str());

			temp = NumberFormatter::format(posi->AvgPrice, 20, symMaster->decimal);
			QuickSetText(5, row, temp.c_str());

			temp = NumberFormatter::format(posi->CurPrice, 20, symMaster->decimal);
			QuickSetText(6, row, temp.c_str());
		}
		else
		{
			std::string temp = NumberFormatter::format(posi->OpenProfitLoss, 20, 2);
			QuickSetText(4, row, temp.c_str());

			temp = NumberFormatter::format(posi->AvgPrice, 20, 2);
			QuickSetText(5, row, temp.c_str());

			temp = NumberFormatter::format(posi->CurPrice, 20, 2);
			QuickSetText(6, row, temp.c_str());
		}
		
		QuickSetNumber(7, row, posi->RsrbQty);

		GetCell(0, row, &cell);
		cell.Tag((void*)posi);

		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		QuickRedrawCell(4, row);
		QuickRedrawCell(5, row);
		QuickRedrawCell(6, row);
		QuickRedrawCell(7, row);

		row++;
	}

	DataCount(row);

}


void VtRemainOrderGrid::SetFilledOrderList()
{
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->ActiveOrderManager();
	int row = 0;
	CUGCell cell;
	for (auto it = orderMgr->FilledList.begin(); it != orderMgr->FilledList.end(); ++it)
	{
		VtOrder* order = *it;
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
		std::string price = NumberFormatter::format(order->orderPrice, 20, symMaster->decimal);
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

		row++;
	}

	DataCount(row);
}


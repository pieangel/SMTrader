#include "stdafx.h"
#include "VtPreOrderGrid.h"
#include "resource.h"
#include "VtOrderManager.h"
#include "VtOrder.h"
#include "Global/VtDefine.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"
#include "Poco/String.h"
#include "VtOrderManagerSelector.h"
#include "VtGlobal.h"

using Poco::NumberFormatter;
using Poco::trim;

VtPreOrderGrid::VtPreOrderGrid()
{
}


VtPreOrderGrid::~VtPreOrderGrid()
{
}

void VtPreOrderGrid::OnSetup()
{
	//m_CancelBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_CANCEL));
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
			if (xIndex == 0)
			{
				QuickSetBitmap(xIndex, yIndex, &m_CancelBitmap);
			}
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtPreOrderGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	int nCol = col;
	int nRow = row;
}

void VtPreOrderGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[5] = { "취소", "종목", "구분", "수량", "가격" };
	int colWidth[5] = { 26, 60, 60, 60, 60 };
	

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

void VtPreOrderGrid::SetPreOrderList()
{
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->ActiveOrderManager();
	int row = 0;
	CUGCell cell;
	for (auto it = orderMgr->PreOrderList.begin(); it != orderMgr->PreOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->state != VtOrderState::Preordered)
			continue;
		
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

		std::string amount = NumberFormatter::format(order->amount, 5);
		amount = trim(amount);
		QuickSetText(3, row, amount.c_str());
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByShortCode(order->shortCode);
		std::string price = NumberFormatter::format(order->orderPrice, 20, symMaster->decimal);
		price = trim(price);
		QuickSetText(4, row, price.c_str());

		GetCell(0, row, &cell);
		cell.Tag((void*)order);

		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		QuickRedrawCell(4, row);

		row++;
	}

	DataCount(row);
}

void VtPreOrderGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtPreOrderGrid::ResetCellText()
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

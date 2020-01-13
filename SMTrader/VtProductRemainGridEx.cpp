

#include "stdafx.h"
#include "VtProductRemainGridEx.h"
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
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"
#include "VtOrderLeftWnd.h"
#include "VtOrder.h"
#include "VtPosition.h"
#include "VtHoga.h"
#include "VtGlobal.h"

using Poco::NumberFormatter;
using Poco::trim;

VtProductRemainGridEx::VtProductRemainGridEx()
{
}


VtProductRemainGridEx::~VtProductRemainGridEx()
{
}


void VtProductRemainGridEx::OnSetup()
{
	//m_OutBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_OUT));
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

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

void VtProductRemainGridEx::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtProductRemainGridEx::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
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

int VtProductRemainGridEx::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
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

			int val = (int)m_cell.GetNumber();
			cell.SetNumber((val = (val + 1) % ((style & UGCT_CHECKBOX3STATE) ? 3 : 2)));
			SetCell(0, i, &cell);
			RedrawCell(0, i);
		}
	}
	return TRUE;
}

void VtProductRemainGridEx::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "청산", "종목", "구분", "평가손익" };
	int colWidth[4] = { 24, 35, 35, 63 };

	QuickSetCellType(0, -1, UGCT_CHECKBOX);
	QuickSetCellTypeEx(0, -1, UGCT_CHECKBOXCHECKMARK);
	QuickSetBackColor(0, -1, RGB(255, 255, 255));
	QuickSetAlignment(0, -1, UG_ALIGNVCENTER | UG_ALIGNCENTER);
	QuickRedrawCell(0, -1);

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
		RedrawCell(i, -1);
	}
}

void VtProductRemainGridEx::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtProductRemainGridEx::ResetCellText()
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

void VtProductRemainGridEx::SetRemainList()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtAccount* acnt = _OrderConfigMgr->Account();
	if (!acnt)
		return;

	ResetCellText();

	NUMBERFMT numberformat2 = { 0 };
	TCHAR bufILZero[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO, bufILZero, 3);
	numberformat2.LeadingZero = _ttoi(bufILZero);
	TCHAR bufINegNum[3] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, bufINegNum, 3);
	numberformat2.NegativeOrder = _ttoi(bufINegNum);
	numberformat2.Grouping = 3;
	TCHAR bufSThousands[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, bufSThousands, 5);
	numberformat2.lpThousandSep = bufSThousands;
	TCHAR bufSDecimal[5] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, bufSDecimal, 5);
	numberformat2.lpDecimalSep = bufSDecimal;
	numberformat2.NumDigits = 2;
	CTextNumber::s_lpNumberFormat = &numberformat2;
	CTextNumber tn;

	int row = 0;
	CUGCell cell;
	for (auto it = acnt->PositionMap.begin(); it != acnt->PositionMap.end(); ++it)
	{
		VtPosition* posi = it->second;
		VtSymbol* sym = symMgr->FindSymbol(posi->ShortCode);
		if (!sym) continue;
		if (posi->OpenQty == 0) continue;


		QuickSetText(1, row, sym->ShortCode.c_str());
		GetCell(1, row, &cell);
		cell.Tag((void*)sym);
		SetCell(1, row, &cell);

		std::string position;
		switch (posi->Position)
		{
		case VtPositionType::Buy:
			{
			position = _T("매수");
			QuickSetTextColor(1, row, RGB(255, 0, 0));
			QuickSetTextColor(2, row, RGB(255, 0, 0));
			}
			break;
		case VtPositionType::Sell:
			{
			position = _T("매도");
			QuickSetTextColor(1, row, RGB(0, 0, 255));
			QuickSetTextColor(2, row, RGB(0, 0, 255));

			}
			break;
		default:
			position = _T("오류");
			break;
		}


		QuickSetText(2, row, position.c_str());

		tn = posi->OpenProfitLoss;
		if (posi->OpenProfitLoss > 0)
			QuickSetTextColor(3, row, RGB(255, 0, 0));
		else
			QuickSetTextColor(3, row, RGB(0, 0, 255));
		QuickSetAlignment(3, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetText(3, row, tn.GetString());


		GetCell(0, row, &cell);
		cell.Tag((void*)posi);
		SetCell(0, row, &cell);



		QuickRedrawCell(1, row);
		QuickRedrawCell(2, row);
		QuickRedrawCell(3, row);
		row++;
	}

	DataCount(row);

}


void VtProductRemainGridEx::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtProductRemainGridEx::LiqSelectedPos()
{
	if (_DataCount == 0)
		return;
	_SelectedPosList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		int val = (int)cell.GetNumber();
		VtPosition* posi = (VtPosition*)cell.Tag();
		if (val == 1 && posi)
		{
			_SelectedPosList.push_back(posi);
		}
	}

	LiqList();

}

void VtProductRemainGridEx::LiqAll()
{
	if (_DataCount == 0)
		return;

	_SelectedPosList.clear();
	CUGCell cell;
	for (int i = 0; i < DataCount(); i++)
	{
		GetCell(0, i, &cell);
		VtPosition* posi = (VtPosition*)cell.Tag();
		_SelectedPosList.push_back(posi);
	}

	GetCell(0, -1, &cell);
	cell.SetNumber(0);
	SetCell(0, -1, &cell);
	RedrawCell(0, -1);

	ResetCellText();

	LiqList();
}

void VtProductRemainGridEx::LiqList()
{
	if (_SelectedPosList.size() == 0)
		return;

	for (auto it = _SelectedPosList.begin(); it != _SelectedPosList.end(); ++it)
	{
		VtPosition* posi = *it;
		LiqudRemain(posi);
	}

	_SelectedPosList.clear();
}

void VtProductRemainGridEx::LiqudRemain(VtPosition* posi)
{
	if (!posi || !_OrderConfigMgr->Symbol())
		return;
	VtOrderRequest request;
	VtSymbol* sym = _OrderConfigMgr->Symbol();
	if (posi->Position == VtPositionType::Buy)
	{
		request.orderPosition = VtPositionType::Sell;
		request.priceType = VtPriceType::Market;
		request.tradeType = VtTradeType::Market;

		request.orderPrice = _OrderConfigMgr->Hoga()->Ary[4].BuyPrice;
	}
	else if (posi->Position == VtPositionType::Sell)
	{
		request.orderPosition = VtPositionType::Buy;
		request.priceType = VtPriceType::Price;
		request.tradeType = VtTradeType::Price;
		request.orderPrice = _OrderConfigMgr->Hoga()->Ary[4].SellPrice;
	}
	request.accountNo = _OrderConfigMgr->Account()->AccountNo;
	request.shortCode = _OrderConfigMgr->Symbol()->ShortCode;
	request.amount = posi->OpenQty;
	request.orderType = VtOrderType::New;
	request.password = _OrderConfigMgr->Account()->Password;
	request.productDecimal = _OrderConfigMgr->Master()->decimal;
	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

void VtProductRemainGridEx::SetFilledOrderList()
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



#include "stdafx.h"
#include "VtSymbolMasterGrid.h"
#include "VtSymbolMaster.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "Util/VtTime.h"
#include "Poco/NumberFormatter.h"
#include "VtGlobal.h"
#include <functional>
#include "SmCallbackManager.h"
#include "VtSymbol.h"
#include "VtFundOrderManager.h"
#include "Log/loguru.hpp"
#include "SmOrderPanelOut.h"
#include "VtOrderWnd.h"
#include "XFormatNumber.h"
#include "Format/format.h"

using namespace std;
using namespace std::placeholders;
using Poco::NumberFormatter;

VtSymbolMasterGrid::VtSymbolMasterGrid()
{
	_Symbol = nullptr;
	_SymbolMaster = nullptr;
}


VtSymbolMasterGrid::~VtSymbolMasterGrid()
{
	_SymbolMaster = nullptr;
	_Symbol = nullptr;
	UnregisterAllCallback();
}


void VtSymbolMasterGrid::UnregisterAllCallback()
{
	UnregisterQuoteCallback();
}

void VtSymbolMasterGrid::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&VtSymbolMasterGrid::OnQuoteEvent, this, _1));
}

void VtSymbolMasterGrid::UnregisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void VtSymbolMasterGrid::OnQuoteEvent(VtSymbol* symbol)
{
	if (!_Symbol || !symbol)
		return;
	if (_Symbol->ShortCode.compare(symbol->ShortCode) != 0)
		return;
	UpdateSymbol(symbol);
}

void VtSymbolMasterGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(60);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetRowTitle();
	SetVS_Width(0);

	JoinCells(0, 0, 1, 0);
	SetRowHeight(0, 40);
	EnableMenu(TRUE);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	RedrawAll();

	RegisterQuoteCallback();
}


void VtSymbolMasterGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[11] = { "상품명", "시가", "고가", "저가", "종가", "전일대비", "총거래량", "틱가치", "틱크기", "거래소", "만기일" };

	SetColWidth(0, 60);
	SetColWidth(1, 103);

	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(0, i, title[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(0, i, &cell);
		QuickSetFont(0, i, &_titleFont);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(0, i);
	}

	QuickSetCellTypeEx(0, 0, UGCT_NORMALMULTILINE);
}

void VtSymbolMasterGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSymbolMasterGrid::SetSymbolMaster(VtSymbolMaster* symMaster)
{
	if (!symMaster)
		return;
	_SymbolMaster = symMaster;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbolByFullCode(symMaster->FullCode);
	if (sym)
		QuickSetText(0, 0, sym->Name.c_str());
	std::string temp;
	temp = NumberFormatter::format(symMaster->close1, symMaster->decimal);
	QuickSetText(1, 1, temp.c_str());
	QuickSetText(1, 2, temp.c_str());
	QuickSetText(1, 3, temp.c_str());
	QuickSetText(1, 4, temp.c_str());
	QuickSetText(1, 5, temp.c_str());
	QuickSetNumber(1, 6, 0);
	temp = NumberFormatter::format(symMaster->tickVal, symMaster->decimal);
	QuickSetText(1, 7, temp.c_str());
	temp = NumberFormatter::format(symMaster->tickSize, symMaster->decimal);
	QuickSetText(1, 8, temp.c_str());

	QuickSetText(1, 9, symMaster->Scn_Exch_cd.c_str());
	std::string expDate = VtTimeUtil::GetDate((char*)symMaster->ExpiryDate.c_str());
	QuickSetText(1, 10, expDate.c_str());

	RedrawAll();
}

void VtSymbolMasterGrid::InitSymMaster(VtSymbol* sym)
{
	if (!sym)
		return;

	_Symbol = sym;

	try
	{
		QuickSetText(0, 0, sym->Name.c_str());
		CString thVal;
		std::string temp = NumberFormatter::format(sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 1, thVal);
		temp = NumberFormatter::format(sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 2, thVal);
		temp = NumberFormatter::format(sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 3, thVal);
		temp = NumberFormatter::format(sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 4, thVal);
		temp = fmt::format("{:.{}f}", sym->ComToPrev / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 5, temp.c_str());

		// 누적 거래량
		
		QuickSetNumber(1, 6, sym->AccAmount);
		// 틱가치
		temp = fmt::format("{:.{}f}", sym->TickValue, sym->Decimal);
		QuickSetText(1, 7, temp.c_str());
		// 틱크기
		temp = NumberFormatter::format(sym->intTickSize / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 8, temp.c_str());
		// 거래소
		QuickSetText(1, 9, sym->Exchange.c_str());
		// 만기일
		std::string ex_date;
		if (sym->ExpDate.length() >= 8) {
			ex_date.append(sym->ExpDate.substr(0, 4));
			ex_date.append("년");
			ex_date.append(sym->ExpDate.substr(4, 2));
			ex_date.append("월");
			ex_date.append(sym->ExpDate.substr(6, 2));
			ex_date.append("일");
			QuickSetText(1, 10, ex_date.c_str());
		}
		for (int i = 0; i < 11; i++)
			QuickRedrawCell(1, i);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
}

void VtSymbolMasterGrid::UpdateSymbol(VtSymbol* sym)
{
	if (!sym)
		return;

	_Symbol = sym;

	try
	{
		QuickSetText(0, 0, sym->Name.c_str());
		CString thVal;
		std::string temp = NumberFormatter::format(sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 1, thVal);
		temp = NumberFormatter::format(sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 2, thVal);
		temp = NumberFormatter::format(sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 3, thVal);
		temp = NumberFormatter::format(sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(1, 4, thVal);
		temp = fmt::format("{:.{}f}", sym->ComToPrev / std::pow(10, sym->Decimal), sym->Decimal);
		QuickSetText(1, 5, temp.c_str());

		// 누적 거래량

		QuickSetNumber(1, 6, sym->AccAmount);
		
		for (int i = 1; i < 7; i++)
			QuickRedrawCell(1, i);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
}

void VtSymbolMasterGrid::OnReceivedSymbolMaster(VtSymbolMaster* symMaster)
{
	if (!symMaster)
		return;
	SetSymbolMaster(symMaster);
}

void VtSymbolMasterGrid::OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster)
{
	if (!_SymbolMaster)
		return;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	
	std::string temp;
	temp = NumberFormatter::format(symMaster->OpenPrice, _SymbolMaster->decimal);
	QuickSetText(1, 1, temp.c_str());
	temp = NumberFormatter::format(symMaster->HighPrice, _SymbolMaster->decimal);
	QuickSetText(1, 2, temp.c_str());
	temp = NumberFormatter::format(symMaster->LowPrice, _SymbolMaster->decimal);
	QuickSetText(1, 3, temp.c_str());
	temp = NumberFormatter::format(symMaster->ClosePrice, _SymbolMaster->decimal);
	QuickSetText(1, 4, temp.c_str());
	temp = NumberFormatter::format(symMaster->CmpPrice, _SymbolMaster->decimal);
	QuickSetText(1, 5, temp.c_str());

	temp = NumberFormatter::format(symMaster->TotQty, 0);
	QuickSetText(1, 6, temp.c_str());

	for(int i = 1; i < 7; i++)
		QuickRedrawCell(1, i);
}

void VtSymbolMasterGrid::OnReceiveRealtimeSymbolMaster(VtSymbolMaster* symMaster)
{
	if (!symMaster)
		return;

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
	numberformat2.NumDigits = 0;
	CTextNumber::s_lpNumberFormat = &numberformat2;
	CTextNumber tn;

	try
	{
		std::string temp;
		temp = NumberFormatter::format(symMaster->OpenPrice, symMaster->decimal);
		QuickSetText(1, 1, temp.c_str());
		temp = NumberFormatter::format(symMaster->HighPrice, symMaster->decimal);
		QuickSetText(1, 2, temp.c_str());
		temp = NumberFormatter::format(symMaster->LowPrice, symMaster->decimal);
		QuickSetText(1, 3, temp.c_str());
		temp = NumberFormatter::format(symMaster->close1, symMaster->decimal);
		QuickSetText(1, 4, temp.c_str());
		temp = NumberFormatter::format(symMaster->CmpPrice, symMaster->decimal);
		QuickSetText(1, 5, temp.c_str());

		//temp = NumberFormatter::format(symMaster->TotQty, 0);
		tn = symMaster->TotQty;
		QuickSetText(1, 6, tn.GetString());

		for (int i = 1; i < 7; i++)
			QuickRedrawCell(1, i);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
}

void VtSymbolMasterGrid::ResetSymbol()
{
	_SymbolMaster = nullptr;
	_Symbol = nullptr;
}

void VtSymbolMasterGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}


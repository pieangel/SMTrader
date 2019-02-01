#include "stdafx.h"
#include "VtAccountRemainGrid.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtAccount.h"
#include "UGrid/UGStrOp.h"
#include "VtGlobal.h"

using Poco::NumberFormatter;

VtAccountRemainGrid::VtAccountRemainGrid()
{
}


VtAccountRemainGrid::~VtAccountRemainGrid()
{
}

void VtAccountRemainGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetRowTitle();
	SetVS_Width(0);

	EnableMenu(TRUE);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			if (xIndex != 0)
			{
				QuickSetFont(xIndex, yIndex, &_defFont);
				QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			}
		}
	}

	RedrawAll();

}


void VtAccountRemainGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	return;
}

void VtAccountRemainGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[6] = { "예탁금잔액", "평가손익", "청산손익", "수수료", "주문가능액", "통화코드" };
	SetColWidth(0, 60);
	SetColWidth(1, 100);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(0, i, title[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(0, i, &cell);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(0, i, &_titleFont);
		RedrawCell(0, i);
	}
}

void VtAccountRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtAccountRemainGrid::ShowAccountInfo(VtAccount* acnt)
{
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

	CUGCell cell;
	CString msg;
	tn = acnt->Deposit;
	msg = tn.GetString();
	QuickSetText(1, 0, msg);
	QuickSetAlignment(1, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 0);

	tn = acnt->OpenPL;
	msg = tn.GetString();
	QuickSetText(1, 1, msg);
	if (acnt->OpenPL < 0)
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
	else if (acnt->OpenPL > 0)
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
	QuickSetAlignment(1, 1, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 1);

	tn = acnt->TradePL;
	msg = tn.GetString();
	QuickSetText(1, 2, msg);
	if (acnt->TradePL < 0)
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
	else if (acnt->TradePL > 0)
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
	QuickSetAlignment(1, 2, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 2);

	tn = acnt->Fee;
	msg = tn.GetString();
	QuickSetText(1, 3, msg);
	QuickSetAlignment(1, 3, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 3);

	tn = acnt->OrdableAmt;
	msg = tn.GetString();
	QuickSetText(1, 4, msg);
	QuickSetAlignment(1, 4, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 4);

	QuickSetText(1, 5, acnt->Crc_cd.c_str());
	QuickRedrawCell(1, 5);
}

void VtAccountRemainGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtAccountRemainGrid::OnReceiveAccountDeposit(VtAccount* acnt)
{
	if (!_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Account() != acnt)
		return;

	ShowAccountInfo(acnt);
}


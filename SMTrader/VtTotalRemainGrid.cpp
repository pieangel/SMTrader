#include "stdafx.h"
#include "VtTotalRemainGrid.h"
#include "VtAccount.h"
#include "VtOrderConfigManager.h"
#include "VtGlobal.h"

VtTotalRemainGrid::VtTotalRemainGrid()
{
}


VtTotalRemainGrid::~VtTotalRemainGrid()
{
}


void VtTotalRemainGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);



	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetRowTitle();
	SetVS_Width(0);
	SetHS_Height(0);

	EnableMenu(TRUE);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	RedrawAll();

}


void VtTotalRemainGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	return;
}

void VtTotalRemainGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[6] = { "Æò°¡¼ÕÀÍ", "½ÇÇö¼ÕÀÍ", "ÃÑ¼ÕÀÍ" };
	SetColWidth(0, 56);
	SetColWidth(1, 100);
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
}

void VtTotalRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtTotalRemainGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtTotalRemainGrid::OnReceiveAccountDeposit(VtAccount* acnt)
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	if (_OrderConfigMgr->Account() != acnt)
		return;

	ShowAccountInfo(acnt);
}

void VtTotalRemainGrid::ShowAccountInfo(VtAccount* acnt)
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

	//msg.Format(_T("%.2f"), acnt->OpenPL);
	//thousandsep(acnt->OpenPL, buffer, sizeof(buffer), 2);
	//msg = buffer;
	tn = acnt->OpenPL;
	QuickSetText(1, 0, tn.GetString());
	if (acnt->OpenPL < 0)
		QuickSetTextColor(1, 0, RGB(0, 0, 255));
	else if (acnt->OpenPL > 0)
		QuickSetTextColor(1, 0, RGB(255, 0, 0));
	QuickSetAlignment(1, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 0);

	//msg.Format(_T("%.2f"), acnt->TradePL);
	//thousandsep(acnt->TradePL, buffer, sizeof(buffer), 2);
	//msg = buffer;
	tn = acnt->TradePL;
	QuickSetText(1, 1, tn.GetString());
	if (acnt->TradePL < 0)
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
	else if (acnt->TradePL > 0)
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
	QuickSetAlignment(1, 1, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	QuickRedrawCell(1, 1);

	//msg.Format(_T("%.2f"), acnt->OpenPL + acnt->TradePL);
	//thousandsep(acnt->OpenPL + acnt->TradePL, buffer, sizeof(buffer), 2);
	//msg = buffer;
	double total = (acnt->OpenPL + acnt->TradePL);
	tn = total;
	QuickSetAlignment(1, 2, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	if (acnt->OpenPL + acnt->TradePL < 0)
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
	else if (acnt->OpenPL + acnt->TradePL > 0)
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
	QuickSetText(1, 2, tn.GetString());
	QuickRedrawCell(1, 2);
}


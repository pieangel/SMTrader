#include "stdafx.h"
#include "HdAccountPLGrid.h"
#include "HdAccountPLDlg.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtQuote.h"
#include "Global/VtDefine.h"
#include "XFormatNumber.h"
#include "VtGlobal.h"
using Poco::NumberFormatter;
HdAccountPLGrid::HdAccountPLGrid()
{
	_AcntPLDlg = nullptr;
}


HdAccountPLGrid::~HdAccountPLGrid()
{
}


void HdAccountPLGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	SetSH_Width(60);
	SetHS_Height(0);
	SetVS_Width(0);
	SetTH_Height(0);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetAlignment(0, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);


	SetRowTitle();

	InitGrid();

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.right = rcWnd.left + 192;
	SetWindowPos(nullptr, 0, 0, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE);
}

void HdAccountPLGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdAccountPLGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void HdAccountPLGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[4] = { "평가손익", "실현손익", "수수료", "순손익" };
	SetColWidth(-1, 66);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.right = rcWnd.left + 192;
	SetColWidth(0, rcWnd.Width() - 66);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(-1, i, title[i]);
		GetCell(-1, i, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(-1, i, &cell);
		QuickSetAlignment(-1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(-1, i, &_titleFont);
		RedrawCell(-1, i);
	}
}

void HdAccountPLGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdAccountPLGrid::Begin()
{

}

void HdAccountPLGrid::End()
{

}

void HdAccountPLGrid::InitGrid()
{
	if (!_AcntPLDlg || !_AcntPLDlg->Account())
		return;

	VtAccount* acnt = _AcntPLDlg->Account();

	//acnt->UpdateRemain();

	std::string temp = NumberFormatter::format(acnt->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->OpenPL > 0) {
		QuickSetTextColor(0, 0, RGB(255, 0, 0));
		QuickSetText(0, 0, profitLoss);
	} else if (acnt->OpenPL < 0) {
		QuickSetTextColor(0, 0, RGB(0, 0, 255));
		QuickSetText(0, 0, profitLoss);
	} else {
		QuickSetTextColor(0, 0, RGB(0, 0, 0));
		QuickSetNumber(0, 0, 0);
	}

	temp = NumberFormatter::format(acnt->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TradePL > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, profitLoss);
	} else if (acnt->TradePL < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, profitLoss);
	} else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	temp = NumberFormatter::format(acnt->Fee, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(0, 2, profitLoss);

	temp = NumberFormatter::format(acnt->TotalPL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TotalPL > 0) {
		QuickSetTextColor(0, 3, RGB(255, 0, 0));
		QuickSetText(0, 3, profitLoss);
	} else if (acnt->TotalPL < 0) {
		QuickSetTextColor(0, 3, RGB(0, 0, 255));
		QuickSetText(0, 3, profitLoss);
	} else {
		QuickSetTextColor(0, 3, RGB(0, 0, 0));
		QuickSetNumber(0, 3, 0);
	}

	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);
	QuickRedrawCell(0, 2);
	QuickRedrawCell(0, 3);
}

void HdAccountPLGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	InitGrid();
}

void HdAccountPLGrid::OnOrderFilledHd(VtOrder* order)
{
	InitGrid();
}

void HdAccountPLGrid::ClearValues()
{
	QuickSetText(0, 0, _T(""));
	QuickSetText(0, 1, _T(""));
	QuickSetText(0, 2, _T(""));
	QuickSetText(0, 3, _T(""));

	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);
	QuickRedrawCell(0, 2);
	QuickRedrawCell(0, 3);
}


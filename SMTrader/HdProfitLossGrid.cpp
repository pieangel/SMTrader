#include "stdafx.h"
#include "HdProfitLossGrid.h"
#include "VtOrderConfigManager.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "VtFund.h"
#include "VtGlobal.h"
using Poco::NumberFormatter;
HdProfitLossGrid::HdProfitLossGrid()
{
	_OrderConfigMgr = nullptr;
}


HdProfitLossGrid::~HdProfitLossGrid()
{
}

void HdProfitLossGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

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
}

void HdProfitLossGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdProfitLossGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void HdProfitLossGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[3] = { "Æò°¡¼ÕÀÍ", "½ÇÇö¼ÕÀÍ", "ÃÑ¼ÕÀÍ" };
	SetColWidth(-1, 60);
	SetColWidth(0, 110);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(-1, i, title[i]);
		GetCell(-1, i, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(-1, i, &cell);
		QuickSetAlignment(-1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(-1, i, &_defFont);
		RedrawCell(-1, i);
	}
}

void HdProfitLossGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdProfitLossGrid::Begin()
{

}

void HdProfitLossGrid::End()
{

}

void HdProfitLossGrid::InitGrid()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
		ShowAccountProfitLoss();
	else
		ShowFundProfitLoss();
}

void HdProfitLossGrid::OnOutstanding()
{
	InitGrid();
}

void HdProfitLossGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	InitGrid();
}

void HdProfitLossGrid::OnReceiveAccountInfo()
{
	InitGrid();
}

void HdProfitLossGrid::ShowAccountProfitLoss()
{
	VtAccount* acnt = _OrderConfigMgr->Account();

	if (!acnt)
		return;


	std::string temp = NumberFormatter::format(acnt->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->OpenPL > 0)
	{
		QuickSetTextColor(0, 0, RGB(255, 0, 0));
		QuickSetText(0, 0, profitLoss);
	}
	else if (acnt->OpenPL < 0)
	{
		QuickSetTextColor(0, 0, RGB(0, 0, 255));
		QuickSetText(0, 0, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 0, RGB(0, 0, 0));
		QuickSetNumber(0, 0, 0);
	}

	temp = NumberFormatter::format(acnt->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TradePL > 0)
	{
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, profitLoss);
	}
	else if (acnt->TradePL < 0)
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	temp = NumberFormatter::format(acnt->TotalPL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TotalPL > 0)
	{
		QuickSetTextColor(0, 2, RGB(255, 0, 0));
		QuickSetText(0, 2, profitLoss);
	}
	else if (acnt->TotalPL < 0)
	{
		QuickSetTextColor(0, 2, RGB(0, 0, 255));
		QuickSetText(0, 2, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 2, RGB(0, 0, 0));
		QuickSetNumber(0, 2, 0);
	}
	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);

	QuickRedrawCell(0, 2);
}

void HdProfitLossGrid::ShowFundProfitLoss()
{
	VtFund* fund = _OrderConfigMgr->Fund();
	if (!fund)
		return;


	//acnt->UpdateRemain();

	std::string temp = NumberFormatter::format(fund->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->OpenPL > 0)
	{
		QuickSetTextColor(0, 0, RGB(255, 0, 0));
		QuickSetText(0, 0, profitLoss);
	}
	else if (fund->OpenPL < 0)
	{
		QuickSetTextColor(0, 0, RGB(0, 0, 255));
		QuickSetText(0, 0, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 0, RGB(0, 0, 0));
		QuickSetNumber(0, 0, 0);
	}

	temp = NumberFormatter::format(fund->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->TradePL > 0)
	{
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, profitLoss);
	}
	else if (fund->TradePL < 0)
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	temp = NumberFormatter::format(fund->TotalPL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->TotalPL > 0)
	{
		QuickSetTextColor(0, 2, RGB(255, 0, 0));
		QuickSetText(0, 2, profitLoss);
	}
	else if (fund->TotalPL < 0)
	{
		QuickSetTextColor(0, 2, RGB(0, 0, 255));
		QuickSetText(0, 2, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 2, RGB(0, 0, 0));
		QuickSetNumber(0, 2, 0);
	}
	QuickRedrawCell(0, 0);
	QuickRedrawCell(0, 1);

	QuickRedrawCell(0, 2);
}

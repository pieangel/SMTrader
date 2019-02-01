#include "stdafx.h"
#include "HdAssetGrid.h"
#include "VtOrderConfigManager.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "VtFund.h"
#include "VtGlobal.h"
using Poco::NumberFormatter;

HdAssetGrid::HdAssetGrid()
{
	_OrderConfigMgr = nullptr;
}


HdAssetGrid::~HdAssetGrid()
{
}

void HdAssetGrid::OnSetup()
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
}

void HdAssetGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdAssetGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void HdAssetGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR title[2] = { "평가예탁", "주문가능" };
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

void HdAssetGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdAssetGrid::Begin()
{

}

void HdAssetGrid::End()
{

}

void HdAssetGrid::InitGrid()
{
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		std::string temp = NumberFormatter::format(acnt->OpenDeposit, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(0, 0, profitLoss);

		temp = NumberFormatter::format(acnt->OrdableAmt, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(0, 1, profitLoss);

		QuickRedrawCell(0, 0);
		QuickRedrawCell(0, 1);
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
		if (acntVec.size() == 0)
			return;
		// 대표 계좌 정보를 보여주는 것으로 한다.
		VtAccount* acnt = acntVec.front();
		acnt = acnt->ParentAccount();
		if (!acnt)
			return;
		std::string temp = NumberFormatter::format(acnt->OpenDeposit, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(0, 0, profitLoss);

		temp = NumberFormatter::format(acnt->OrdableAmt, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(0, 1, profitLoss);

		QuickRedrawCell(0, 0);
		QuickRedrawCell(0, 1);
	}
}

void HdAssetGrid::OnReceiveAccountInfo()
{
	InitGrid();
}


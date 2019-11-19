#include "stdafx.h"
#include "SmAssetGrid.h"
#include "VtOrderConfigManager.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "VtFund.h"
#include "VtGlobal.h"
using Poco::NumberFormatter;

SmAssetGrid::SmAssetGrid()
{
	_OrderConfigMgr = nullptr;
}


SmAssetGrid::~SmAssetGrid()
{
}

void SmAssetGrid::Init()
{
	_CellHeight = 20;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 0; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < _ColCount; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				if (j == 0)
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				else // 텍스트 정렬
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);

	SetRowTitle();
}

void SmAssetGrid::SetRowTitle()
{
	LPCTSTR title[2] = { "평가예탁", "주문가능" };
	SetColumnWidth(0, 60);
	SetColumnWidth(1, 110);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(i, 0, title[i]);
		QuickSetBackColor(i, 0, VtGlobal::GridTitleBackColor);
		QuickSetTextColor(i, 0, VtGlobal::GridTitleTextColor);
		InvalidateCellRect(i, 0);
	}
}

void SmAssetGrid::InitGrid()
{
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		std::string temp = NumberFormatter::format(acnt->OpenDeposit, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(0, 1, profitLoss);

		temp = NumberFormatter::format(acnt->OrdableAmt, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(1, 1, profitLoss);

		InvalidateCellRect(0, 1);
		InvalidateCellRect(1, 1);
	}
	else {
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

		QuickSetText(0, 1, profitLoss);

		temp = NumberFormatter::format(acnt->OrdableAmt, 0);
		profitLoss = XFormatNumber(temp.c_str(), -1);

		QuickSetText(1, 1, profitLoss);

		InvalidateCellRect(0, 1);
		InvalidateCellRect(1, 1);
	}
}

void SmAssetGrid::OnReceiveAccountInfo()
{
	InitGrid();
}


void SmAssetGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmAssetGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmAssetGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmAssetGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

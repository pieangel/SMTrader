#include "stdafx.h"
#include "VtAccountAssetGrid.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "Global/VtDefine.h"
#include "XFormatNumber.h"
#include "VtAssetPage.h"
#include <numeric>
using Poco::NumberFormatter;

VtAccountAssetGrid::VtAccountAssetGrid()
{
	_AssetPage = nullptr;
}


VtAccountAssetGrid::~VtAccountAssetGrid()
{
}

void VtAccountAssetGrid::OnSetup()
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

	SetSH_Width(0);
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
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidVec.begin(), _ColWidVec.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

void VtAccountAssetGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtAccountAssetGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtAccountAssetGrid::SetRowTitle()
{
	const int rowCount = _RowCount;
	CUGCell cell;
	LPCTSTR titleLeft[5] = { "예탁총액", "평가손익", "실현손익", "위탁증거금", "유지증거금" };
	LPCTSTR titleRight[5] = { "평가예탁총액", "수수료", "순손익", "주문가능금액", "추가증거금" };
	//SetColWidth(-1, 60);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	_ColWidVec.push_back(74);
	_ColWidVec.push_back(80);
	SetColWidth(0, _ColWidVec[0]);
	SetColWidth(1, _ColWidVec[1]);
	SetColWidth(2, _ColWidVec[2]);
	SetColWidth(3, _ColWidVec[3]);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(0, i, titleLeft[i]);
		GetCell(0, i, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(0, i, &cell);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(0, i, &_titleFont);
		RedrawCell(0, i);

		QuickSetText(2, i, titleRight[i]);
		GetCell(2, i, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(2, i, &cell);
		QuickSetAlignment(2, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(2, i, &_titleFont);
		RedrawCell(2, i);
	}
}

void VtAccountAssetGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtAccountAssetGrid::InitGrid()
{
	if (!_AssetPage || !_AssetPage->Account())
		return;

	VtAccount* acnt = _AssetPage->Account();

	std::string val = NumberFormatter::format(acnt->Deposit);
	CString strDeposit = XFormatNumber(val.c_str(), -1);
	QuickSetTextColor(1, 0, RGB(0, 0, 0));
	QuickSetText(1, 0, strDeposit);

	val = NumberFormatter::format(acnt->OpenDeposit);
	CString strOpenDeposit = XFormatNumber(val.c_str(), -1);
	QuickSetTextColor(3, 0, RGB(0, 0, 0));
	QuickSetText(3, 0, strOpenDeposit);

	std::string temp = NumberFormatter::format(acnt->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->OpenPL > 0) {
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, profitLoss);
	} else if (acnt->OpenPL < 0) {
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, profitLoss);
	} else {
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}

	temp = NumberFormatter::format(acnt->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TradePL > 0) {
		QuickSetTextColor(1, 2, RGB(255, 0, 0));
		QuickSetText(1, 2, profitLoss);
	} else if (acnt->TradePL < 0) {
		QuickSetTextColor(1, 2, RGB(0, 0, 255));
		QuickSetText(1, 2, profitLoss);
	} else {
		QuickSetTextColor(1, 2, RGB(0, 0, 0));
		QuickSetNumber(1, 2, 0);
	}

	temp = NumberFormatter::format(acnt->Fee, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(3, 1, profitLoss);

	temp = NumberFormatter::format(acnt->TotalPL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (acnt->TotalPL > 0) {
		QuickSetTextColor(3, 2, RGB(255, 0, 0));
		QuickSetText(3, 2, profitLoss);
	} else if (acnt->TotalPL < 0) {
		QuickSetTextColor(3, 2, RGB(0, 0, 255));
		QuickSetText(3, 2, profitLoss);
	} else {
		QuickSetTextColor(3, 2, RGB(0, 0, 0));
		QuickSetNumber(3, 2, 0);
	}

	temp = NumberFormatter::format(acnt->Trst_mgn, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(1, 3, profitLoss);

	temp = NumberFormatter::format(acnt->OrdableAmt, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(3, 3, profitLoss);

	temp = NumberFormatter::format(acnt->Mnt_mgn, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(1, 4, profitLoss);

	
	temp = NumberFormatter::format(acnt->Add_mgn, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	QuickSetText(3, 4, profitLoss);

	for (int i = 0; i < _ColCount; ++i) {
		for (int j = 0; j < _RowCount; ++j) {
			if (i == 1 || i == 3) {
				QuickSetAlignment(i, j, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
				QuickRedrawCell(i, j);
			}
		}
	}
}

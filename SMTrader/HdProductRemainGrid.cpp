#include "stdafx.h"
#include "HdProductRemainGrid.h"
#include "VtAccountManager.h"
#include "VtPosition.h"
#include "VtAccount.h"
#include "HdAccountPLDlg.h"
#include "Poco/NumberFormatter.h"
#include "VtQuote.h"
#include "Global/VtDefine.h"
#include "XFormatNumber.h"
#include "VtSymbol.h"
#include "VtSymbolManager.h"
#include <numeric>
#include "VtGlobal.h"
using Poco::NumberFormatter;

HdProductRemainGrid::HdProductRemainGrid()
{
	_AcntPLDlg = nullptr;
}


HdProductRemainGrid::~HdProductRemainGrid()
{
}


void HdProductRemainGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetHS_Height(0);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetAlignment(0, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(3, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetColTitle();
	InitGrid();
	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidthVector.begin(), _ColWidthVector.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

void HdProductRemainGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int HdProductRemainGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void HdProductRemainGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "ÄÚµå", "±¸ºÐ", "Æò±Õ°¡", "Æò°¡¼ÕÀÍ" };
	_ColWidthVector.push_back(58);
	_ColWidthVector.push_back(22);
	_ColWidthVector.push_back(50);
	_ColWidthVector.push_back(62);

	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, _ColWidthVector[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

void HdProductRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdProductRemainGrid::InitGrid()
{
	if (!_AcntPLDlg || !_AcntPLDlg->Account())
		return;

	VtAccount* acnt = _AcntPLDlg->Account();
	std::map<VtCellPos, VtCellPos> posMap;
	ClearOldValuse(posMap);
	int i = 0;
	VtCellPos pos;
	for (auto it = acnt->PositionMap.begin(); it != acnt->PositionMap.end(); ++it) {
		VtPosition* posi = it->second;
		if (std::abs(posi->OpenQty) > 0) {
			for (int c = 0; c < 4; c++) {
				pos.Row = i;
				pos.Col = c;
				posMap[pos] = pos;
			}
			ShowPosition(posi, acnt, i++);
		}
	}

	RefreshCells(posMap);
}

void HdProductRemainGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	InitGrid();
}

void HdProductRemainGrid::OnOrderFilledHd(VtOrder* order)
{
	InitGrid();
}

void HdProductRemainGrid::ClearValues()
{
	if (!_AcntPLDlg || !_AcntPLDlg->Account())
		return;

	VtAccount* acnt = _AcntPLDlg->Account();
	int index = 0;
	for (auto it = acnt->PositionMap.begin(); it != acnt->PositionMap.end(); ++it) {
		VtPosition* posi = it->second;

		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
		index++;
	}
}

void HdProductRemainGrid::ShowPosition(VtPosition* posi, VtAccount* acnt, int index)
{
	if (!posi || !acnt)
		return;

	VtCellPos pos;
	for (int i = 0; i < 4; i++) {
		pos.Row = index;
		pos.Col = i;
		_OldPosMap[pos] = pos;
	}

	if (!posi) {
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
		return;
	}

	if (posi->OpenQty == 0) {
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
		return;
	}

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(posi->FullCode);
	if (!sym)
		return;

	QuickSetText(0, index, posi->FullCode.c_str());
	
	if (posi->Position == VtPositionType::Buy) {
		QuickSetTextColor(1, index, RGB(255, 0, 0));
		QuickSetTextColor(2, index, RGB(255, 0, 0));
		QuickSetTextColor(3, index, RGB(255, 0, 0));
	} else if (posi->Position == VtPositionType::Sell) {
		QuickSetTextColor(1, index, RGB(0, 0, 255));
		QuickSetTextColor(2, index, RGB(0, 0, 255));
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetNumber(1, index, std::abs(posi->OpenQty));
	}

	std::string symCode = posi->ShortCode;
	if (symCode.find(_T("201")) != std::string::npos) { // coloring the call 
		QuickSetBackColor(0, index, RGB(252, 226, 228));
		QuickSetBackColor(1, index, RGB(252, 226, 228));
		QuickSetBackColor(2, index, RGB(252, 226, 228));
		QuickSetBackColor(3, index, RGB(252, 226, 228));
	}

	if (symCode.find(_T("301")) != std::string::npos) { // coloring the put 
		QuickSetBackColor(0, index, RGB(218, 226, 245));
		QuickSetBackColor(1, index, RGB(218, 226, 245));
		QuickSetBackColor(2, index, RGB(218, 226, 245));
		QuickSetBackColor(3, index, RGB(218, 226, 245));
	}

	CString strValue;
	strValue.Format(_T("%d"), posi->OpenQty);
	QuickSetText(1, index, strValue);

	std::string temp = NumberFormatter::format(posi->AvgPrice, sym->IntDecimal);
	CString thVal = XFormatNumber(temp.c_str(), sym->IntDecimal);
	QuickSetText(2, index, thVal);

	temp = NumberFormatter::format(posi->OpenProfitLoss, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (posi->OpenProfitLoss > 0) {
		QuickSetTextColor(3, index, RGB(255, 0, 0));
		QuickSetText(3, index, profitLoss);
	} else if (posi->OpenProfitLoss < 0) {
		QuickSetTextColor(3, index, RGB(0, 0, 255));
		QuickSetText(3, index, profitLoss);
	} else {
		QuickSetTextColor(3, index, RGB(0, 0, 0));
		QuickSetNumber(3, index, 0);
	}
}


void HdProductRemainGrid::ClearOldValuse(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = _OldPosMap.begin(); it != _OldPosMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickSetText(pos.Col, pos.Row, _T(""));
		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
		posMap[pos] = pos;
	}
}

void HdProductRemainGrid::RefreshCells(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickRedrawCell(pos.Col, pos.Row);
	}
}
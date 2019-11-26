#include "stdafx.h"
#include "VtFundProductRemainGrid.h"
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
#include "VtFundMiniJango.h"
#include "VtFund.h"
#include <vector>
#include <string>
#include <numeric>
#include "VtGlobal.h"
using Poco::NumberFormatter;

VtFundProductRemainGrid::VtFundProductRemainGrid()
{
	_AcntPLDlg = nullptr;
	_FundPLDlg = nullptr;
	_Mode = 0;
}


VtFundProductRemainGrid::~VtFundProductRemainGrid()
{
}


void VtFundProductRemainGrid::OnSetup()
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
	SetColTitle(0);
	InitGrid();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidthVector.begin(), _ColWidthVector.end(), 0), rcWnd.Height(), SWP_NOMOVE);
}

void VtFundProductRemainGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int VtFundProductRemainGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtFundProductRemainGrid::SetColTitle(int mode)
{
	const int ColCount = _ColCount;
	CUGCell cell;
	std::vector<std::string> titleVec;
	_ColWidthVector.clear();
	if (mode == 0) {
		titleVec.push_back(_T("ÄÚµå"));
		titleVec.push_back(_T("±¸ºÐ"));
		titleVec.push_back(_T("Æò±Õ°¡"));
		titleVec.push_back(_T("Æò°¡¼ÕÀÍ"));
		_ColWidthVector.push_back(56);
		_ColWidthVector.push_back(22);
		_ColWidthVector.push_back(50);
		_ColWidthVector.push_back(72);
	} else {
		titleVec.push_back(_T("°èÁÂ"));
		titleVec.push_back(_T("ÄÚµå"));
		titleVec.push_back(_T("ÀÜ°í"));
		titleVec.push_back(_T("Æò±Õ°¡"));
		_ColWidthVector.push_back(54);
		_ColWidthVector.push_back(46);
		_ColWidthVector.push_back(36);
		_ColWidthVector.push_back(64);
	}

	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, _ColWidthVector[i]);
		QuickSetText(i, -1, titleVec[i].c_str());
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

void VtFundProductRemainGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtFundProductRemainGrid::InitGrid()
{
	if (!_FundPLDlg || !_FundPLDlg->Fund())
		return;

	VtFund* fund = _FundPLDlg->Fund();
	std::map<VtCellPos, VtCellPos> posMap;
	ClearOldValuse(posMap);
	VtCellPos pos;
	int i = 0;
	if (_Mode == 1) {
		std::vector<VtPosition*> posiVec = fund->GetPositionListBySubAccount();
		for (auto it = posiVec.begin(); it != posiVec.end(); ++it) {
			VtPosition* posi = *it;
			if (std::abs(posi->OpenQty) > 0) {
				for (int c = 0; c < 4; c++) {
					pos.Row = i;
					pos.Col = c;
					posMap[pos] = pos;
				}
				ShowPosition(posi, i++);
			}
		}
	} else {
		std::map<std::string, std::shared_ptr<VtPosition>> posiMap = fund->GetPositionListByProduct();
		for (auto it = posiMap.begin(); it != posiMap.end(); ++it) {
			std::shared_ptr<VtPosition> posi = it->second;
			if (std::abs(posi->OpenQty) > 0) {
				for (int c = 0; c < 4; c++) {
					pos.Row = i;
					pos.Col = c;
					posMap[pos] = pos;
				}
				ShowPosition(posi.get(), i++);
			}
		}
	}

	RefreshCells(posMap);
}

void VtFundProductRemainGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	InitGrid();
}

void VtFundProductRemainGrid::OnOrderFilledHd(VtOrder* order)
{
	InitGrid();
}

void VtFundProductRemainGrid::ClearValues()
{
	for (int index = 0; index < _RowCount; index++)
	{
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
	}
}

void VtFundProductRemainGrid::ShowPosition(VtPosition* posi, int index)
{
	if (!posi)
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
	VtSymbol* sym = symMgr->FindHdSymbol(posi->ShortCode);
	if (!sym)
		return;

	if (posi->Position == VtPositionType::Buy){
		QuickSetTextColor(1, index, RGB(255, 0, 0));
		QuickSetTextColor(2, index, RGB(255, 0, 0));
		QuickSetTextColor(3, index, RGB(255, 0, 0));
	} else if (posi->Position == VtPositionType::Sell) {
		QuickSetTextColor(1, index, RGB(0, 0, 255));
		QuickSetTextColor(2, index, RGB(0, 0, 255));
		QuickSetTextColor(3, index, RGB(0, 0, 255));
	}

	if (_Mode == 0) { // °èÁÂ º°·Î º¸±â
		QuickSetText(0, index, posi->ShortCode.c_str());

		CString strValue;
		strValue.Format(_T("%d"), posi->OpenQty);
		QuickSetText(1, index, strValue);

		std::string temp = NumberFormatter::format(posi->AvgPrice, sym->Decimal);
		CString thVal = XFormatNumber(temp.c_str(), sym->Decimal);
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
	} else if (_Mode == 1) { // Á¾¸ñº°·Î º¸±â
		QuickSetText(0, index, posi->AccountNo.c_str());

		QuickSetText(1, index, posi->ShortCode.c_str());

		CString strValue;
		strValue.Format(_T("%d"), posi->OpenQty);
		QuickSetText(2, index, strValue);

		std::string temp = NumberFormatter::format(posi->AvgPrice, sym->Decimal);
		CString thVal = XFormatNumber(temp.c_str(), sym->Decimal);
		QuickSetText(3, index, thVal);
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

}

void VtFundProductRemainGrid::ClearOldValuse(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = _OldPosMap.begin(); it != _OldPosMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickSetText(pos.Col, pos.Row, _T(""));
		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
		posMap[pos] = pos;
	}
}

void VtFundProductRemainGrid::RefreshCells(std::map<VtCellPos, VtCellPos>& posMap)
{
	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		VtCellPos pos = it->second;
		QuickRedrawCell(pos.Col, pos.Row);
	}
}

void VtFundProductRemainGrid::ClearCells()
{
	for (int index = 0; index < _RowCount; index++) {
		QuickSetText(0, index, _T(""));
		QuickSetText(1, index, _T(""));
		QuickSetText(2, index, _T(""));
		QuickSetText(3, index, _T(""));
	}
}

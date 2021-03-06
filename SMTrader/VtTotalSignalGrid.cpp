#include "stdafx.h"
#include "VtTotalSignalGrid.h"
#include "VtGlobal.h"
#include "VtOutSystemOrderManager.h"
#include "System/VtSystem.h"
#include "VtAccount.h"
#include "VtSymbol.h"
#include "VtPosition.h"
#include "VtOutSignalDef.h"
#include "XFormatNumber.h"
#include "VtFund.h"
#include <array>
#include <numeric>
#include "Poco/NumberFormatter.h"
using Poco::NumberFormatter;

VtTotalSignalGrid::VtTotalSignalGrid()
{
}


VtTotalSignalGrid::~VtTotalSignalGrid()
{
}

void VtTotalSignalGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	_RowCount = 100;
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(GetSystemMetrics(SM_CXVSCROLL));
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtTotalSignalGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int VtTotalSignalGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtTotalSignalGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[9] = { "계좌번호", "종목", "포지션", "평균단가", "현재가", "평가손익", "매도주문", "매수주문", "신호차트" };
	int colWidth[9] = { 80, 110, 50, 80, 80, 80, 80, 80, 80 - GetSystemMetrics(SM_CXVSCROLL) };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void VtTotalSignalGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtTotalSignalGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

void VtTotalSignalGrid::RefreshOrders()
{
	VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
	SharedSystemVec& sysVec = outSysOrderMgr->GetSysVec();

	for (size_t i = 0; i < sysVec.size(); ++i) {
		SharedSystem sys = sysVec[i];
		if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
			if (sys->Account()) QuickSetText(0, i, sys->Account()->AccountNo.c_str());
		}
		else {
			if (sys->Fund()) QuickSetText(0, i, sys->Fund()->Name.c_str());
		}

		if (sys->Symbol()) QuickSetText(1, i, sys->Symbol()->ShortCode.c_str());

		VtPosition posi = sys->GetPosition();
		posi.Position == VtPositionType::None ? QuickSetText(2, i, _T("없음")) : posi.Position == VtPositionType::Buy ? QuickSetText(2, i, _T("매수")) : QuickSetText(2, i, _T("매도"));
		CString thVal;
		std::string temp;
		if (sys->Symbol()) {
			temp = NumberFormatter::format(posi.AvgPrice, sys->Symbol()->Decimal);
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		QuickSetText(3, i, thVal);



		CUGCell cell;
		GetCell(4, i, &cell);
		int curValue = sys->Symbol() ? sys->Symbol()->Quote.intClose : 0;
		if (sys->Symbol()) {
			temp = NumberFormatter::format(curValue / std::pow(10, sys->Symbol()->Decimal), sys->Symbol()->Decimal);
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		cell.SetText(thVal);
		cell.LongValue(curValue);
		SetCell(4, i, &cell);

		temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		if (posi.OpenProfitLoss > 0) {
			QuickSetTextColor(5, i, RGB(255, 0, 0));
			QuickSetText(5, i, profitLoss);
		}
		else if (posi.OpenProfitLoss < 0) {
			QuickSetTextColor(5, i, RGB(0, 0, 255));
			QuickSetText(5, i, profitLoss);
		}
		else {
			QuickSetTextColor(5, i, RGB(0, 0, 0));
			QuickSetNumber(5, i, 0);
		}

		if (posi.OpenQty != 0) {
			if (posi.OpenQty > 0) {
				QuickSetNumber(7, i, std::abs(posi.OpenQty));
				QuickSetNumber(6, i, 0);
			}
			else if (posi.OpenQty < 0) {
				QuickSetNumber(6, i, std::abs(posi.OpenQty));
				QuickSetNumber(7, i, 0);
			}
		}
		else {
			QuickSetNumber(6, i, 0);
			QuickSetNumber(7, i, 0);
		}
		QuickSetText(8, i, sys->OutSignal()->SignalName.c_str());
		for (int j = 0; j < _ColCount; ++j) {
			QuickRedrawCell(j, i);
		}
	}
}

void VtTotalSignalGrid::InitGrid()
{
	VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
	SharedSystemVec& sysVec = outSysOrderMgr->GetSysVec();
	
	for (size_t i = 0; i < sysVec.size(); ++i) {
		SharedSystem sys = sysVec[i];
		if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
			if (sys->Account()) QuickSetText(0, i, sys->Account()->AccountNo.c_str());
		}
		else {
			if (sys->Fund()) QuickSetText(0, i, sys->Fund()->Name.c_str());
		}
		if (sys->Symbol()) QuickSetText(1, i, sys->Symbol()->ShortCode.c_str());
		VtPosition posi = sys->GetPosition();
		posi.Position == VtPositionType::None ? QuickSetText(2, i, _T("없음"))  : posi.Position == VtPositionType::Buy ? QuickSetText(2, i, _T("매수")) : QuickSetText(2, i, _T("매도"));
		CString thVal;
		std::string temp;
		if (sys->Symbol()) {
			temp = NumberFormatter::format(posi.AvgPrice, sys->Symbol()->Decimal);
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		QuickSetText(3, i, thVal);



		CUGCell cell;
		GetCell(4, i, &cell);
		int curValue = sys->Symbol() ? sys->Symbol()->Quote.intClose : 0;
		if (sys->Symbol()) {
			temp = NumberFormatter::format(curValue / std::pow(10, sys->Symbol()->Decimal), sys->Symbol()->Decimal);
			thVal = XFormatNumber(temp.c_str(), sys->Symbol()->Decimal);
		}
		else {
			temp = NumberFormatter::format(0, 0);
			thVal = XFormatNumber(temp.c_str(), -1);
		}
		cell.SetText(thVal);
		cell.LongValue(curValue);
		SetCell(4, i, &cell);

		temp = NumberFormatter::format(posi.OpenProfitLoss, 0);
		CString profitLoss = XFormatNumber(temp.c_str(), -1);

		if (posi.OpenProfitLoss > 0) {
			QuickSetTextColor(5, i, RGB(255, 0, 0));
			QuickSetText(5, i, profitLoss);
		}
		else if (posi.OpenProfitLoss < 0) {
			QuickSetTextColor(5, i, RGB(0, 0, 255));
			QuickSetText(5, i, profitLoss);
		}
		else {
			QuickSetTextColor(5, i, RGB(0, 0, 0));
			QuickSetNumber(5, i, 0);
		}

		if (posi.OpenQty != 0) {
			if (posi.OpenQty > 0) {
				QuickSetNumber(7, i, std::abs(posi.OpenQty));
				QuickSetNumber(6, i, 0);
			}
			else if (posi.OpenQty < 0) {
				QuickSetNumber(6, i, std::abs(posi.OpenQty));
				QuickSetNumber(7, i, 0);
			}
		}
		else {
			QuickSetNumber(6, i, 0);
			QuickSetNumber(7, i, 0);
		}
		QuickSetText(8, i, sys->OutSignal()->SignalName.c_str());
		for (int j = 0; j < _ColCount; ++j) {
			QuickRedrawCell(j, i);
		}
	}

	_RowNumber = sysVec.size();
}

void VtTotalSignalGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowNumber + 1; i++)
	{
		for (int j = 0; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
		}
	}
}

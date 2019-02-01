#include "stdafx.h"
#include "HdOptionGrid.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtOrderLeftWndHd.h"
#include "VtOptionMonthSection.h"
#include "VtSymbolManager.h"
#include "HdOptionGrid.h"
#include "HdOptionPage.h"
#include "VtHdClient.h"
#include "VtOrderConfigManager.h"
#include "VtOrderCenterWndHd.h"
#include "VtGlobal.h"
using Poco::NumberFormatter;

HdOptionGrid::HdOptionGrid()
{
	_CurPrdtSec = nullptr;
	_OptionPage = nullptr;
	_EqualCol = -2;
	_EqualRow = -2;
	_UsdConfigDlg = nullptr;
}


HdOptionGrid::~HdOptionGrid()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetOptionGrid(nullptr);
}

void HdOptionGrid::OnSetup()
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

	SetSH_Width(0);
	SetHS_Height(0);
	SetVS_Width(0);
	SetTH_Height(22);
	LockColumns(0);
	LockColumns(2);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetBackColor(0, i, RGB(252, 226, 228));
		QuickSetBackColor(2, i, RGB(218, 226, 245));
	}

	SetDefFont(&_defFont);


	SetColTitle();

	InitSymbol();
	InitYearMonth();
	InitGrid();
}

void HdOptionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	int i = 0;
	CUGCell cell;
	GetCell(col, row, &cell);
	VtSymbol* sym = (VtSymbol*)cell.Tag();
	if (sym)
	{
		if (_OrderConfigMgr)
		{
			_OrderConfigMgr->Symbol(sym);

			if (_OrderConfigMgr->_HdCenterWnd)
			{
				_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
			}
		}
	}
}


void HdOptionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "Call", "Çà»ç°¡", "Put" };
	int colWidth[3] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
}

void HdOptionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdOptionGrid::Begin()
{

}

void HdOptionGrid::End()
{

}

void HdOptionGrid::InitGrid()
{
	if (!_OptionPage)
		return;
	if (!_CurPrdtSec)
		return;

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;

	QuickSetBackColor(_EqualCol, _EqualRow, RGB(255, 255, 255));

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	CUGCell cell, centerCell;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		ClearInfo();
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			GetCell(0, i, &cell);
			//cell.SetText(sym->ShortCode.c_str());
			int curValue = sym->Quote.intClose;
			std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
			cell.SetText(strVal.c_str());
			cell.Tag(sym);
			SetCell(0, i, &cell);
			RedrawCell(0, i);
			
			_CallSymbolRowMap[sym] = std::make_pair(0, i);

			GetCell(1, i, &centerCell);
			std::string centerVal = sym->ShortCode.substr(5, 3);
			char centerTip = sym->ShortCode.at(7);
			int intCenter = static_cast<int>(std::stoi(centerVal) * std::pow(10, sym->IntDecimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;

			delta = std::abs(symMgr->Kospi200Current - intCenter);
			if (delta < minVal)
			{
				minVal = delta;
				eIndex = i;
			}

			centerCell.SetNumberDecimals(sym->IntDecimal);
			centerCell.SetNumber(intCenter / std::pow(10, sym->IntDecimal));
			SetCell(1, i, &centerCell);

			RedrawCell(1, i);
			i++;
		}
	}

	SetEqualRow(eIndex);
	QuickSetBackColor(1, eIndex, RGB(255, 255, 0));
	QuickRedrawCell(1, eIndex);

	_EqualCol = 1;
	_EqualRow = eIndex;

	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	opSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			GetCell(2, i, &cell);
			//cell.SetText(sym->ShortCode.c_str());
			int curValue = sym->Quote.intClose;
			std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
			cell.SetText(strVal.c_str());
			cell.Tag(sym);
			SetCell(2, i, &cell);
			RedrawCell(2, i);
			_PutSymbolRowMap[sym] = std::make_pair(2, i);
			i++;
		}
	}
}

void HdOptionGrid::InitSymbol()
{
	if (!_OptionPage)
		return;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	for (auto it = prdtCatMgr->MainOptionVector.begin(); it != prdtCatMgr->MainOptionVector.end(); ++it)
	{
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section)
		{
			int index = _OptionPage->_ComboOptionMarket.AddString(section->Name.c_str());
			_OptionPage->_ComboOptionMarket.SetItemDataPtr(index, section);
		}
	}

	_OptionPage->_ComboOptionMarket.SetCurSel(0);
	_CurPrdtSec = (VtProductSection*)_OptionPage->_ComboOptionMarket.GetItemDataPtr(0);
}

void HdOptionGrid::InitYearMonth()
{
	if (!_OptionPage)
		return;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	for (auto it = prdtCatMgr->MainOptionVector.begin(); it != prdtCatMgr->MainOptionVector.end(); ++it)
	{
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section)
		{
			_CurPrdtSec = section;
			SetYearMonth();
			break;
		}
	}

	_OptionPage->_ComboOptionMonth.SetCurSel(0);
}

void HdOptionGrid::SetYearMonth()
{
	if (!_CurPrdtSec)
		return;

	_OptionPage->_ComboOptionMonth.ResetContent();
	VtProductSubSection* subSection = _CurPrdtSec->SubSectionVector.front();
	for (auto it = subSection->_OptionMap.begin(); it != subSection->_OptionMap.end(); ++it)
	{
		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _OptionPage->_ComboOptionMonth.AddString(yearMon);
		//_LeftWnd->_ComboOption.SetItemDataPtr(index, code.c_str());
	}
	_OptionPage->_ComboOptionMonth.SetCurSel(0);
}

void HdOptionGrid::SetProductSection()
{
	if (!_OptionPage)
		return;
	int selPrdt = _OptionPage->_ComboOptionMarket.GetCurSel();
	if (selPrdt == -1)
		return;
	_CurPrdtSec = (VtProductSection*)_OptionPage->_ComboOptionMarket.GetItemDataPtr(selPrdt);
}

void HdOptionGrid::GetSymbolMaster()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetOptionGrid(this);

	if (!_CurPrdtSec)
		return;

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;
	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			if (sym->Quote.intClose == 0)
				sym->GetSymbolMaster();
		}
	}
	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	opSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			if (sym->Quote.intClose == 0)
				sym->GetSymbolMaster();
		}
	}
}

void HdOptionGrid::GetSymbolMasterLately()
{
	if (_EqualRow < 0)
		GetSymbolMaster();

	int centerRow = _EqualRow;

	VtHdClient* client = VtHdClient::GetInstance();
	client->SetOptionGrid(this);

	if (!_CurPrdtSec)
		return;

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;

	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);

	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	VtOptionMonthSection* opSec2 = putSec->FindOptionMap((LPCTSTR)curYearMon);

	if (opSec && opSec2)
	{
		int startIndex = 0;
		int endIndex = opSec->_SymbolVector.size() - 1;
		int addNum = 0;
		int curIndex = _EqualRow;
		bool upRange = false, downRange = false;
		while (1)
		{
			if (addNum % 2 == 0)
				curIndex = curIndex + addNum;
			else
				curIndex = curIndex - addNum;

			if (curIndex < startIndex)
				upRange = true;
			if (curIndex > endIndex)
				downRange = true;

			if (curIndex >= startIndex && curIndex <= endIndex)
			{
				VtSymbol* sym = opSec->_SymbolVector[curIndex];
				if (sym->Quote.intClose == 0)
					sym->GetSymbolMaster();
				sym = opSec2->_SymbolVector[curIndex];
				if (sym->Quote.intClose == 0)
					sym->GetSymbolMaster();
			}

			if (upRange && downRange)
				break;

			addNum++;
		}
	}
}

void HdOptionGrid::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _CallSymbolRowMap.find(sym);
	if (it != _CallSymbolRowMap.end())
	{
		auto pos = _CallSymbolRowMap[sym];
		GetCell(pos.first, pos.second, &cell);
		int curValue = sym->Quote.intClose;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		
		cell.SetText(strVal.c_str());
		SetCell(pos.first, pos.second, &cell);
		RedrawCell(pos.first, pos.second);
	}

	it = _PutSymbolRowMap.find(sym);
	if (it != _PutSymbolRowMap.end())
	{
		auto pos = _PutSymbolRowMap[sym];
		GetCell(pos.first, pos.second, &cell);
		int curValue = sym->Quote.intClose;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		cell.SetText(strVal.c_str());
		SetCell(pos.first, pos.second, &cell);
		RedrawCell(pos.first, pos.second);
	}
}

void HdOptionGrid::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_UsdConfigDlg = ConfigDlg;
}

void HdOptionGrid::ClearInfo()
{
	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetText(0, i, _T(""));
		QuickSetText(2, i, _T(""));
		QuickRedrawCell(0, i);
		QuickRedrawCell(2, i);
	}
}

void HdOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;

	SetTopRow(equalRow - centerRow);
}

int HdOptionGrid::GetMaxRow()
{
	int rowHeight = DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 2;
}

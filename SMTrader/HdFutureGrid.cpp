#include "stdafx.h"
#include "HdFutureGrid.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "HdFuturePage.h"
#include "VtHdClient.h"
#include "VtOrderConfigManager.h"
#include "VtOrderCenterWndHd.h"
#include "VtUsdStrategyConfigDlg.h"
#include "VtGlobal.h"
#include "HdSymbolSelecter.h"
#include "VtSignalConnectionGrid.h"
#include "VtAddConnectSignalDlg.h"
#include "SmOrderPanel.h"
#include "VtChartTimeToolBar.h"
#include "Market/SmMarket.h"
#include "Market/SmMarketManager.h"
#include "Market/SmProduct.h"
#include "Market/SmProductYearMonth.h"
using Poco::NumberFormatter;

HdFutureGrid::HdFutureGrid()
{
	_FuturePage = nullptr;
	_PrdtSec = nullptr;
	_OrderConfigMgr = nullptr;
	_UsdConfigDlg = nullptr;
}


HdFutureGrid::~HdFutureGrid()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetFutureGrid(nullptr);
}

void HdFutureGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetHS_Height(0);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 1; i < _ColCount; i++)
	{
		QuickSetAlignment(i, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	for (int i = 0; i < _RowCount; i++)
	{
		QuickSetAlignment(0, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetColTitle();

	InitMarket();
	SetProductSection();
	InitGrid();
}

void HdFutureGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	int i = 0;
	CUGCell cell;
	GetCell(col, row, &cell);
	VtSymbol* sym = (VtSymbol*)cell.Tag();
	if (sym) {
		if (_UsdConfigDlg) {
			_UsdConfigDlg->SetSymbol(sym);
			if (_SymSelecter)
				_SymSelecter->SendMessage(WM_CLOSE, 0, 0);
			return;
		}
		if (_AddConSigDlg) {
			_AddConSigDlg->SetSymbol(sym);
			if (_SymSelecter)
				_SymSelecter->SendMessage(WM_CLOSE, 0, 0);
			return;
		}
		if (_SigConGrid) {
			_SigConGrid->SetSymbol(sym);
			if (_SymSelecter)
				_SymSelecter->SendMessage(WM_CLOSE, 0, 0);
			return;
		}
		if (_TimeToolbar) {
			_TimeToolbar->ChangeChartData(sym);
			if (_SymSelecter)
				_SymSelecter->SendMessage(WM_CLOSE, 0, 0);
			return;
		}
		if (_OrderConfigMgr) {
			_OrderConfigMgr->Symbol(sym);

			VtChartDataRequest req;
			if (row == 0)
				req.chartType = VtChartType::TICK;
			else if (row == 1)
				req.chartType = VtChartType::MIN;
			else if (row == 2)
				req.chartType = VtChartType::DAY;
			else if (row == 3)
				req.chartType = VtChartType::WEEK;
			else if (row == 4)
				req.chartType = VtChartType::MONTH;

			req.productCode = sym->ShortCode;
			req.count = 9999;
			req.cycle = 1;
			req.next = 0;
			req.reqKey = 0;
			req.seq = 0;

			VtHdClient* client = VtHdClient::GetInstance();
			//client->GetChartData(std::move(req));

			if (_OrderConfigMgr->_HdCenterWnd) {
				_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
			}
		}
	}
}

void HdFutureGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[3] = { "코드", "이름", "현재가"};
	int colWidth[3] = { 60, 120, 60 };


	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickRedrawCell(i, -1);
	}
}

void HdFutureGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdFutureGrid::InitMarket()
{
	if (!_FuturePage)
		return;
	_FuturePage->_ComboFutureMarket.ResetContent();

	std::vector<SmRunInfo> run_info = SmMarketManager::GetInstance()->GetFutureRunVector();
	int i = 0;
	for (auto it = run_info.begin(); it != run_info.end(); ++it) {
		SmRunInfo item = *it;
		int index = _FuturePage->_ComboFutureMarket.AddString(item.Name.c_str());
	}

	_FuturePage->_ComboFutureMarket.SetCurSel(0);

	_ProductIndex = 0;
}

void HdFutureGrid::SetProductSection()
{
	if (!_FuturePage)
		return;
	_ProductIndex = _FuturePage->_ComboFutureMarket.GetCurSel();
}

void HdFutureGrid::InitGrid()
{
	std::vector<SmRunInfo> run_info = SmMarketManager::GetInstance()->GetFutureRunVector();
	if (run_info.size() == 0)
		return;
	SmRunInfo item = run_info[_ProductIndex];
	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(item.Code.substr(0, 3));
	if (!product)
		return;

	ClearSymbolInfo();
	std::vector<VtSymbol*>& symbol_list = product->GetSymbolList();
	for (size_t i = 0; i < symbol_list.size(); ++i) {
		CUGCell cell, centerCell;
		VtSymbol* sym = symbol_list[i];
		if (sym) {
			GetCell(0, i, &cell);
			cell.SetText(sym->ShortCode.c_str());
			cell.Tag(sym);
			SetCell(0, i, &cell);
			QuickRedrawCell(0, i);
			GetCell(1, i, &cell);
			cell.Tag(sym);
			SetCell(1, i, &cell);

			SymbolRowMap[sym] = i;

			QuickSetText(1, i, sym->Name.c_str());
			QuickRedrawCell(1, i);

			GetCell(2, i, &centerCell);
			int intCenter = sym->Quote.intClose;
			centerCell.SetNumberDecimals(sym->Decimal);
			centerCell.SetNumber(intCenter / std::pow(10, sym->Decimal));
			centerCell.Tag(sym);
			SetCell(2, i, &centerCell);
			QuickRedrawCell(2, i);
		}
	}
}

void HdFutureGrid::GetSymbolMaster()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetFutureGrid(this);

	std::vector<SmRunInfo> run_info = SmMarketManager::GetInstance()->GetFutureRunVector();
	if (run_info.size() == 0)
		return;
	SmRunInfo item = run_info[_ProductIndex];
	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(item.Code.substr(0, 3));
	if (!product)
		return;

	std::vector<VtSymbol*>& symbol_list = product->GetSymbolList();
	for (size_t i = 0; i < symbol_list.size(); ++i) {
		VtSymbol* sym = symbol_list[i];
		if (sym->Quote.intClose == 0)
			sym->GetSymbolMaster();
	}
}

void HdFutureGrid::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	int row = SymbolRowMap[sym];
	CUGCell cell;
	GetCell(2, row, &cell);
	int intCenter = sym->Quote.intClose;
	cell.SetNumberDecimals(sym->Decimal);
	cell.SetNumber(intCenter / std::pow(10, sym->Decimal));
	cell.Tag(sym);
	SetCell(2, row, &cell);
	QuickRedrawCell(2, row);
}

void HdFutureGrid::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_UsdConfigDlg = ConfigDlg;
}

void HdFutureGrid::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	_TimeToolbar = timeToolBar;
}

void HdFutureGrid::ClearSymbolInfo()
{
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(0, i, _T(""));
		QuickSetText(1, i, _T(""));
		QuickSetText(2, i, _T(""));

		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
	}
}


void HdFutureGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void HdFutureGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
}

void HdFutureGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}
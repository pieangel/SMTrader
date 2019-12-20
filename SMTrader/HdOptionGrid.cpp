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
#include "HdSymbolSelecter.h"
#include "VtSignalConnectionGrid.h"
#include "VtUsdStrategyConfigDlg.h"
#include "SmOrderPanel.h"
#include "VtChartTimeToolBar.h"
#include "Market/SmMarket.h"
#include "Market/SmMarketManager.h"
#include "Market/SmProduct.h"
#include "Market/SmProductYearMonth.h"
#include "VtRealtimeRegisterManager.h"
using Poco::NumberFormatter;

HdOptionGrid::HdOptionGrid()
{
	_CurPrdtSec = nullptr;
	_OptionPage = nullptr;
	_EqualCol = -2;
	_EqualRow = -2;
	_UsdConfigDlg = nullptr;
	_OldSelCell = std::make_pair(-2, -2);
	_ClickedCell = std::make_pair(-2, -2);
}


HdOptionGrid::~HdOptionGrid()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetOptionGrid(nullptr);
}

void HdOptionGrid::OnSetup()
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
		if (_UsdConfigDlg) {
			_UsdConfigDlg->SetSymbol(sym);
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
	LPCTSTR title[3] = { "Call", "행사가", "Put" };
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

	QuickSetBackColor(_EqualCol, _EqualRow, RGB(255, 255, 255));

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();
	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	int intCenter = 0;
	int eCenter = 0;
	
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);

	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(_RunInfo[_SelectedProduct].CallCode);
	if (!product)
		return ;

	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();

	SmProductYearMonth* year_month = product->GetYearMonth((LPCTSTR)curYearMon);
	// 등가를 찾아 낸다.
	if (year_month) {
		std::vector<VtSymbol*> symbol_list = year_month->SymbolList();
		SetNumberRows(symbol_list.size());
		for (int i = 0; i < symbol_list.size(); ++i) {
			VtSymbol* sym = symbol_list[i];
			if (product->Code().compare("206") == 0) {
				std::string centerVal = sym->Name.substr(18, 6);
				centerVal.erase(std::remove(centerVal.begin(), centerVal.end(), ','), centerVal.end());
				intCenter = std::stoi(centerVal) * (int)(std::pow(10, 2));
				delta = std::abs(symMgr->Kosdaq150Current - intCenter);
				if (delta < minVal) {
					minVal = delta;
					eCenter = intCenter;
					eIndex = i;
				}
			}
			else {
				std::string centerVal = sym->ShortCode.substr(5, 3);
				char centerTip = sym->ShortCode.at(7);
				intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->Decimal));
				// 행사가가 2나 7로 끝나면 .5가 붙는다. 그래서 자릿수를 감안하여 정수로 환산해서 50을 더해 준다.
				if (centerTip == '2' || centerTip == '7')
					intCenter += 50;
				delta = std::abs(symMgr->Kospi200Current - intCenter);
				if (delta < minVal) {
					minVal = delta;
					eCenter = intCenter;
					eIndex = i;
				}
			}
		}
		// 등가 심볼을 저장한다.
		_EqualSymbol = symbol_list[eIndex];
	}

	SetEqualRow(eIndex);
	QuickSetBackColor(1, eIndex, RGB(255, 255, 0));
	QuickRedrawCell(1, eIndex);

	_EqualCol = 1;
	_EqualRow = eIndex;

	SetCellInfo();
}

void HdOptionGrid::InitSymbol()
{
	if (!_OptionPage)
		return;
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	_RunInfo = mrktMgr->GetOptionRunVector();
	for (auto it = _RunInfo.begin(); it != _RunInfo.end(); ++it) {
		SmRunInfo& run_info = *it;
		int index = _OptionPage->_ComboOptionMarket.AddString(run_info.Name.c_str());
	}

	_OptionPage->_ComboOptionMarket.SetCurSel(0);
	_SelectedProduct = 0;
}

void HdOptionGrid::InitYearMonth()
{
	if (!_OptionPage)
		return;

	SetYearMonth();

	_OptionPage->_ComboOptionMonth.SetCurSel(0);
}

void HdOptionGrid::SetYearMonth()
{
	int cur_sel = _OptionPage->_ComboOptionMarket.GetCurSel();
	if (cur_sel < 0)
		return;

	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(_RunInfo[cur_sel].CallCode);
	if (!product)
		return;
	_OptionPage->_ComboOptionMonth.ResetContent();

	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); ++it) {
		SmProductYearMonth* year_month = it->second;

		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _OptionPage->_ComboOptionMonth.AddString(yearMon);
	}

	_OptionPage->_ComboOptionMonth.SetCurSel(0);
}

void HdOptionGrid::SetProductSection()
{
	if (!_OptionPage)
		return;
	_SelectedProduct = _OptionPage->_ComboOptionMarket.GetCurSel();
}

void HdOptionGrid::GetSymbolMaster()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SetOptionGrid(this);

	if (_EqualRow == -1)
		return;

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();

	std::pair<SmProduct*, SmProduct*> product_pair = SmMarketManager::GetInstance()->GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;

	if (call_year_month && put_year_month) {
		std::vector<VtSymbol*> call_symbol_list = call_year_month->SymbolList();
		std::vector<VtSymbol*> put_symbol_list = put_year_month->SymbolList();
		int startIndex = 0;
		int endIndex = call_symbol_list.size() - 1;
		int addNum = 0;
		int curIndex = _EqualRow;
		bool upRange = false, downRange = false;
		while (1) {
			if (addNum % 2 == 0)
				curIndex = curIndex + addNum;
			else
				curIndex = curIndex - addNum;

			if (curIndex < startIndex)
				upRange = true;
			if (curIndex > endIndex)
				downRange = true;

			if (curIndex >= startIndex && curIndex <= endIndex) {
				VtSymbol* sym = nullptr;
				if (curIndex < call_symbol_list.size()) {
					sym = call_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}

				if (curIndex < put_symbol_list.size()) {
					sym = put_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
			}

			if (upRange && downRange)
				break;

			addNum++;
		}
	}
}

void HdOptionGrid::GetSymbolMasterLately()
{
	if (_EqualRow == -1)
		return;

	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();

	std::pair<SmProduct*, SmProduct*> product_pair = SmMarketManager::GetInstance()->GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;

	if (call_year_month && put_year_month) {
		std::vector<VtSymbol*> call_symbol_list = call_year_month->SymbolList();
		std::vector<VtSymbol*> put_symbol_list = put_year_month->SymbolList();
		int startIndex = 0;
		int endIndex = call_symbol_list.size() - 1;
		int addNum = 0;
		int curIndex = _EqualRow;
		bool upRange = false, downRange = false;
		while (1) {
			if (addNum % 2 == 0)
				curIndex = curIndex + addNum;
			else
				curIndex = curIndex - addNum;

			if (curIndex < startIndex)
				upRange = true;
			if (curIndex > endIndex)
				downRange = true;

			if (curIndex >= startIndex && curIndex <= endIndex) {
				VtSymbol* sym = nullptr;
				if (curIndex < call_symbol_list.size()) {
					sym = call_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}

				if (curIndex < put_symbol_list.size()) {
					sym = put_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
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
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		
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
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		SetCell(pos.first, pos.second, &cell);
		RedrawCell(pos.first, pos.second);
	}
}

void HdOptionGrid::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_UsdConfigDlg = ConfigDlg;
}

void HdOptionGrid::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	_TimeToolbar = timeToolBar;
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


void HdOptionGrid::SetCellInfo()
{
	int selMon = _OptionPage->_ComboOptionMonth.GetCurSel();
	if (selMon == -1)
		return;

	std::pair<SmProduct*, SmProduct*> product_pair = SmMarketManager::GetInstance()->GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_OptionPage->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	if (call_year_month) {
		std::vector<VtSymbol*> call_symbol_list = call_year_month->SymbolList();
		// 콜과 풋의 심볼을 순회한다.
		for (int i = 0, j = 0; i < call_symbol_list.size(); ++i, ++j) {			
			VtSymbol* call_sym = call_symbol_list[i];
			std::string strVal = "";
			// 코스닥일때 
			if (product_pair.first->Code().compare("206") == 0) {
				std::string centerVal = call_sym->Name.substr(18, 6);
				centerVal.erase(std::remove(centerVal.begin(), centerVal.end(), ' '), centerVal.end());
				// 중앙값을 설정한다.
				strVal = centerVal;
			}
			else { // 코스피 일때
				std::string centerVal = call_sym->ShortCode.substr(5, 3);
				char centerTip = call_sym->ShortCode.at(7);
				int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, call_sym->Decimal));
				// 행사가가 2나 7로 끝나면 .5가 붙기 때문에 50을 더해준다.
				// 나머지 행사가는 0으로 끝나기 때문에 더해 주지 않아도 된다.
				if (centerTip == '2' || centerTip == '7')
					intCenter += 50;

				// 중앙값을 설정한다.
				strVal = NumberFormatter::format(intCenter / std::pow(10, call_sym->Decimal), call_sym->Decimal);
			}

			CUGCell cell;
			GetCell(1, j, &cell);
			// 새로운 등가를 설정한다.
			if (call_sym == _EqualSymbol) {
				cell.SetBackColor(RGB(255, 255, 0));
			}
			cell.SetText(strVal.c_str());
			SetCell(1, j, &cell);
			QuickRedrawCell(1, j);

			GetCell(0, j, &cell);
			strVal = NumberFormatter::format(call_sym->Quote.intClose / std::pow(10, call_sym->Decimal), call_sym->Decimal);
			cell.SetText(strVal.c_str());
			cell.Tag(call_sym);
			cell.SetBackColor(RGB(252, 226, 228));
			SetCell(0, j, &cell);
			QuickRedrawCell(0, j);

			_CallSymbolRowMap[call_sym] = std::make_pair(0, j);
		}
	}

	if (put_year_month) {
		std::vector<VtSymbol*> put_symbol_list = put_year_month->SymbolList();
		// 콜과 풋의 심볼을 순회한다.
		for (int i = 0, j = 0; i < put_symbol_list.size(); ++i, ++j) {
			VtSymbol* put_sym = put_symbol_list[i];
			CUGCell cell;
			GetCell(2, j, &cell);
			std::string strVal = NumberFormatter::format(put_sym->Quote.intClose / std::pow(10, put_sym->Decimal), put_sym->Decimal);
			cell.SetText(strVal.c_str());
			cell.Tag(put_sym);
			cell.SetBackColor(RGB(218, 226, 245));
			SetCell(2, j, &cell);
			QuickRedrawCell(2, j);
			_PutSymbolRowMap[put_sym] = std::make_pair(2, j);
		}
	}
}

void HdOptionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == TRUE) {
		if (_ClickedCell.first == 0) {
			QuickSetBackColor(_ClickedCell.first, _ClickedCell.second, _CallColor);
		}
		else if (_ClickedCell.first == 2) {
			QuickSetBackColor(_ClickedCell.first, _ClickedCell.second, _PutColor);
		}
		QuickRedrawCell(_ClickedCell.first, _ClickedCell.second);

		if (col == 1)
			return;

		std::pair<int, int> curSel = std::make_pair(col, row);
		QuickSetBackColor(curSel.first, curSel.second, _ClickedColor);
		QuickRedrawCell(curSel.first, curSel.second);
		_ClickedCell = curSel;
	}
}

void HdOptionGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	std::pair<int, int> curSel = std::make_pair(col, row);
	if (_OldSelCell == curSel)
		return;
	if (_OldSelCell != _ClickedCell) {
		if (_OldSelCell.first == 0) {
			QuickSetBackColor(_OldSelCell.first, _OldSelCell.second, _CallColor);
		}
		else if (_OldSelCell.first == 2) {
			QuickSetBackColor(_OldSelCell.first, _OldSelCell.second, _PutColor);
		}
		QuickRedrawCell(_OldSelCell.first, _OldSelCell.second);
	}

	if (col != 1) {
		if (curSel != _ClickedCell) {
			QuickSetBackColor(curSel.first, curSel.second, _SelColor);
			QuickRedrawCell(curSel.first, curSel.second);
		}
		else {
			QuickSetBackColor(curSel.first, curSel.second, _ClickedColor);
			QuickRedrawCell(curSel.first, curSel.second);
		}
	}
	_OldSelCell = curSel;
}

void HdOptionGrid::OnMouseLeaveFromMainGrid()
{
	
}
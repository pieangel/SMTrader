#include "stdafx.h"
#include "SmOptionGrid.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtOrderLeftWndHd.h"
#include "VtOptionMonthSection.h"
#include "VtSymbolManager.h"
#include "VtOrderCenterWndHd.h"
#include "VtPosition.h"
#include "VtAccount.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtFund.h"
#include "VtRealtimeRegisterManager.h"
#include "VtGlobal.h"
#include "SmOrderPanel.h"
#include "SmCallbackManager.h"
#include "Market/SmMarketManager.h"
#include <functional>
#include "Market/SmProductYearMonth.h"
#include "Market/SmProduct.h"
using Poco::NumberFormatter;

using namespace std;
using namespace std::placeholders;

SmOptionGrid::SmOptionGrid()
{
}


SmOptionGrid::~SmOptionGrid()
{
}

void SmOptionGrid::RegisterMasterCallback()
{
	SmCallbackManager::GetInstance()->SubscribeMasterCallback((long)this, std::bind(&SmOptionGrid::OnMasterEvent, this, _1));
}

void SmOptionGrid::UnregisterAllCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeOrderCallback((long)this);
	SmCallbackManager::GetInstance()->UnsubscribeMasterCallback((long)this);
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void SmOptionGrid::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&SmOptionGrid::OnQuoteEvent, this, _1));
}

void SmOptionGrid::RegisterOrderCallback()
{
	SmCallbackManager::GetInstance()->SubscribeOrderCallback((long)this, std::bind(&SmOptionGrid::OnOrderEvent, this, _1));
}

void SmOptionGrid::OnMasterEvent(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnOrderEvent(VtOrder* order)
{
	CString msg;
	msg.Format("OnOrderEvent code = %s\n", order->shortCode.c_str());
	TRACE(msg);
	if (!_OrderConfigMgr || !order)
		return;

	// 체결 이벤트
	if (order->state == VtOrderState::Filled) {
		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtPosition* posi = acnt->FindPosition(order->shortCode);
			if (posi)
				SetRemain(posi);
		}
		else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;
			int count = 0;
			VtPosition posi = fund->GetPosition(order->shortCode, count);
			SetRemain(&posi);
		}
	}
	else { // 나머지 주문 이벤트 
		VtSymbol* symbol = VtSymbolManager::GetInstance()->FindSymbol(order->shortCode);
		if (symbol) {
			SetRemain(symbol);
		}
	}
}

void SmOptionGrid::OnQuoteEvent(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

BEGIN_MESSAGE_MAP(SmOptionGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

void SmOptionGrid::Init()
{
	_EqualIndex = -1;
	_EqualCell.row = -1;
	_EqualCell.col = -1;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	SetColTitle();
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
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// 텍스트 정렬
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);
	RegisterQuoteCallback();
	RegisterMasterCallback();
	RegisterOrderCallback();

	SetColTitle();

	_RunInfo = SmMarketManager::GetInstance()->GetOptionRunVector();

	InitSymbol();
	InitYearMonth();
}

void SmOptionGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	VtSymbol* sym = (VtSymbol*)pCell->GetData();
	if (sym) {
		sym->GetSymbolMaster();
		if (_OrderConfigMgr->_HdCenterWnd) {
			_OrderConfigMgr->Symbol(sym);
			_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
		}
	}
}

void SmOptionGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;
}

BOOL SmOptionGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int distance = (zDelta / 60);
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	_ValueStartRow = _ValueStartRow + -1 * (distance);
	if (_ValueStartRow > _MaxIndex - (_MaxRow - 2))
		_ValueStartRow = _MaxIndex - _MaxRow + 2;
	if (_ValueStartRow < 0)
		_ValueStartRow = 0;

	
	// 기존맵을 삭제한다.
	_SymbolRowMap.clear();
	// 새로운 맵을 만든다.
	MakeSymbolRowMap(_ValueStartRow, _ValueMaxRow);

	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
	
	//InitGrid();
	return CGridCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void SmOptionGrid::SetColTitle()
{
	LPCTSTR title[3] = { "Call", "행사가", "Put" };
	int colWidth[9] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColumnWidth(i, colWidth[i]);
		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		if (i == 0) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(240, 51, 58));
		}
		else if (i == 2) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(19, 137, 255));
		}
		else {
			pCell->SetTextClr(VtGlobal::GridTitleBackColor);
			pCell->SetBackClr(VtGlobal::GridTitleTextColor);
		}
		InvalidateCellRect(0, i);
	}
}

void SmOptionGrid::InitGrid()
{
	CRect rcGrid;
	GetClientRect(rcGrid);
	InitGrid(rcGrid.Height());
}

void SmOptionGrid::InitGrid(int height)
{
	std::pair<int, int> start_max = FindValueStartRow(height);
	_ValueStartRow = start_max.first;
	_ValueMaxRow = start_max.second;
	if (start_max.first == 0 && start_max.second == 0)
		return;
	_RowCount = start_max.second;
	SetRowCount(_RowCount);
	// 셀이 옵션을 다 표시하고도 남으면 그리드를 줄여준다.
	if (_RowCount + 1 > _MaxIndex) {
		int total_cell_height = _CellHeight * (_MaxIndex + 2);
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		SetWindowPos(nullptr, 0, 0, rcWnd.Width(), total_cell_height + 4, SWP_NOMOVE);
	}

	for (int i = 1; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// 텍스트 정렬
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				InvalidateCellRect(i, j);
			}
		}
	}

	// 기존맵을 삭제한다.
	_SymbolRowMap.clear();
	// 모든 텍스트를 지운다.
	ClearAllText();
	// 새로운 맵을 만든다.
	MakeSymbolRowMap(start_max.first, start_max.second);

	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

void SmOptionGrid::InitSymbol()
{
	if (!_LeftWnd)
		return;

	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	for (auto it = _RunInfo.begin(); it != _RunInfo.end(); ++it) {
		SmRunInfo& run_info = *it;
		int index = _LeftWnd->_ComboProduct.AddString(run_info.Name.c_str());
	}

	_LeftWnd->_ComboProduct.SetCurSel(0);
	_SelectedProduct = 0;
}

void SmOptionGrid::InitYearMonth()
{
	if (!_LeftWnd)
		return;
	
	SetYearMonth();
	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void SmOptionGrid::SetYearMonth()
{
	int cur_sel = _LeftWnd->_ComboProduct.GetCurSel();
	if (cur_sel < 0)
		return;

	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(_RunInfo[cur_sel].CallCode);
	if (!product)
		return;
	_LeftWnd->_ComboOptionMonth.ResetContent();

	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();
	for (auto it = year_month_map.begin(); it != year_month_map.end(); ++it) {
		SmProductYearMonth* year_month = it->second;

		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _LeftWnd->_ComboOptionMonth.AddString(yearMon);
	}

	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void SmOptionGrid::SetProductSection()
{
	if (!_LeftWnd)
		return;
	_SelectedProduct = _LeftWnd->_ComboProduct.GetCurSel();
}

void SmOptionGrid::GetSymbolMaster()
{
	if (_EqualIndex == -1)
		return;

	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();

	std::pair<SmProduct*, SmProduct*> product_pair = SmMarketManager::GetInstance()->GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
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
		int curIndex = _EqualIndex;
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
				VtSymbol* sym =nullptr;
				if (curIndex < call_symbol_list.size()) {
					sym = call_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						//VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
		
				if (curIndex < put_symbol_list.size()) {
					sym = put_symbol_list[curIndex];
					if (sym->Quote.intClose == 0) {
						sym->GetSymbolMaster();
						//VtRealtimeRegisterManager::GetInstance()->RegisterProduct(sym->ShortCode);
					}
				}
			}

			if (upRange && downRange)
				break;

			addNum++;
		}
	}
}

void SmOptionGrid::RefreshMode()
{
	ResetRemainCells();
	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

std::pair<int, int> SmOptionGrid::FindValueStartRow(int height)
{
	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1)
		return std::make_pair(0, 0);

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	int intCenter = 0;
	int eCenter = 0;
	// 값을 표시하는 시작 인덱스
	int start_index = 0;
	int max_row = height / DefaultCellHeight;
	_MaxRow = max_row;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);

	SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(_RunInfo[_SelectedProduct].CallCode);
	if (!product)
		return std::make_pair(0, 0);
	
	std::map<std::string, SmProductYearMonth*>& year_month_map = product->GetYearMonthMap();

	SmProductYearMonth* year_month = product->GetYearMonth((LPCTSTR)curYearMon);
	if (year_month) {
		std::vector<VtSymbol*> symbol_list = year_month->SymbolList();
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
		int center_index = (int)(symbol_list.size() / 2);
		_MaxIndex = symbol_list.size() - 1;
		// 등가 인덱스를 저장한다.
		_EqualIndex = eIndex;
		// 등가 심볼을 저장한다.
		_EqualSymbol = symbol_list[eIndex];
		int half = (int)(max_row / 2);
		// 등가 인덱스가 값의 중앙보다 클때 
		if (eIndex > center_index) {
			if (eIndex + half > _MaxIndex) {
				// 맨위 한줄은 제목이므로 1을 더해 준다.
				// 인덱스는 원래 0에서 시작하는데 우리는 1에서 시작하므로 1을 다시한번 더해준다.
				start_index = _MaxIndex - max_row + 1 + 1;
			}
			else {
				start_index = eIndex - half;
			}
			if (start_index < 0)
				start_index = 0;
		}
		else { // 등가 인덱스가 값의 중앙보다 작을 때
			if (eIndex - half < 0) {
				start_index = 0;
			}
			else {
				start_index = eIndex - half;
			}
		}
	}

	return std::make_pair(start_index, max_row);
}

void SmOptionGrid::ClearAllText()
{
	for (int i = 1; i < _RowCount; ++i) {
		for (int j = 0; j < _ColCount; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			pCell->SetText("");
			InvalidateCellRect(i, j);
		}
	}
}

void SmOptionGrid::ResetRemainCells()
{
	for (auto it = _RemainPos.begin(); it != _RemainPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		if (pos.second == 0)
			QuickSetBackColor(pos.first, pos.second, RGB(252, 226, 228));
		else
			QuickSetBackColor(pos.first, pos.second, RGB(218, 226, 245));

		QuickSetText(pos.first, pos.second, _T(""));
	}
}

void SmOptionGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _SymbolRowMap.find(symCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[symCode];
		int col = std::get<1>(pos);
		int row = std::get<0>(pos);
		if (col == 0) { // 콜일 경우
			if (!posi) {
				if (acptCnt > 0) { // 포지션 없고 접수확인 주문만 있을 때 - 회색 배경에 0
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(212, 186, 188));
				}
				else { // 접수 확인 주문이 없을 때
					if (init) { // 한번이라도 주문이 나갔으면 흰색배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // 한번도 주문이 안 나갔으면 매수 배경색
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(252, 226, 228));
					}
				}
			}
			else { // 포지션이 있는 경우 - 최소 한번이라도 주문이 나간것으로 간주한다. - 이전에 나갔어도 
				if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // 접수 확인 주문이 있는 경우  회색배경에 잔고 갯수
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					else // 접수 확인 주문이 없는 경우는 흰색 배경에 잔고 갯수
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // 잔고가 없는 경우
					if (acptCnt > 0) { // 한번이라도 주문이 나간경우 회색배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(212, 186, 188));
					}
					else { // 잔고, 접수확인 둘 다 없을 때 흰색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
			}
		}
		else { // 풋일 경우
			if (!posi) { // 포지션이 없는 경우 - 주문이 나간적이 없을 경우
				if (acptCnt > 0) { // 접수확인된 주문이 있을 경우 - 0에 회색 배경
					QuickSetText(row, col, _T("0"));
					QuickSetBackColor(row, col, RGB(178, 186, 205));
				}
				else {
					if (init) { // 잔고 없이 한번이라도 주문이 나간경우 - 0에 흰색 배경
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
					else { // 잔고도 없고 한번도 주문이 나가지 않은 경우 - 숫자없이 매도 배경색
						QuickSetText(row, col, _T(""));
						QuickSetBackColor(row, col, RGB(218, 226, 245));
					}
				}
			}
			else { // 포지션이 있는 경우 - 주문이 나간적이 있는 경우 - 한번이라도 주문이 나갔다
				if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
					QuickSetNumber(row, col, posi->OpenQty);
					if (acptCnt > 0) // 접수확인이 있는 경우 회색 배경
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					else // 잔고만 있을 때는 흰색 배경
						QuickSetBackColor(row, col, RGB(255, 255, 255));
				}
				else { // 잔고가 없는 경우
					if (acptCnt > 0) { // 접수확인이 있는 경우 - 회색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(178, 186, 205));
					}
					else { // 잔고, 접수확인 없을 때 흰색 배경에 0
						QuickSetText(row, col, _T("0"));
						QuickSetBackColor(row, col, RGB(255, 255, 255));
					}
				}
			}
		}
		InvalidateCellRect(row, col);
		_RemainPos.insert(std::make_pair(row, col));
	}
}

void SmOptionGrid::ShowExpected(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowExpected(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
}

void SmOptionGrid::ShowExpected(int row, int col, VtSymbol* sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		int curValue = sym->Quote.intExpected;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
}

void SmOptionGrid::ShowCurrent(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end()) {
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
}

void SmOptionGrid::ShowCurrent(int row, int col, VtSymbol* sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		int curValue = sym->Quote.intClose;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
}

void SmOptionGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	auto it = _SymbolRowMap.find(posi->ShortCode);
	if (it != _SymbolRowMap.end())
	{
		auto pos = _SymbolRowMap[posi->ShortCode];
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (std::abs(posi->OpenQty) > 0) {
			QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
			if (acnt->GetAcceptedOrderCount(posi->ShortCode) > 0)
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			else
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		else {
			QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
			QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));

		CString msg;
		msg.Format("SetRemain row = %d, col = %d\n", std::get<0>(pos), std::get<1>(pos));
		TRACE(msg);
	}
}

void SmOptionGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(order->shortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, order->shortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, order->shortCode);
	}
}

void SmOptionGrid::SetRemain(VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
	}
}

void SmOptionGrid::SetRemain2()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);

		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
			VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
			if (!orderMgr)
				return;
			VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
			VtPosition* posi = acnt->FindPosition(sym->ShortCode);
			if (prdtOrderMgr)
				ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
			else
				ShowPosition(false, 0, nullptr, sym->ShortCode);
		}
		else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;

			std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
			int count = 0;
			VtPosition posi = fund->GetPosition(sym->ShortCode, count);
			if (count != 0)
				ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
			else
				ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
		}
	}
}

void SmOptionGrid::SetCurrent2()
{
	if (_Mode != 1)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);
		ShowCurrent(sym);
	}
}

void SmOptionGrid::SetExpected2()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmOptionGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;
}

int SmOptionGrid::GetMaxRow()
{
	int rowHeight = DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count;
}

void SmOptionGrid::MakeSymbolRowMap(int start_index, int max_row)
{
	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1)
		return;

	std::pair<SmProduct*, SmProduct*> product_pair = SmMarketManager::GetInstance()->GetProductPair(_RunInfo[_SelectedProduct]);
	if (!product_pair.first || !product_pair.second)
		return;

	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	SmProductYearMonth* call_year_month = product_pair.first->GetYearMonth((LPCTSTR)curYearMon);	
	SmProductYearMonth* put_year_month = product_pair.second->GetYearMonth((LPCTSTR)curYearMon);
	if (!call_year_month || !put_year_month)
		return;

	// 기존 등가 색을 지운다.
	if (_EqualCell.IsValid()) {
		CGridCellBase* pCell = GetCell(_EqualCell.row, _EqualCell.col);
		if (pCell) {
			pCell->SetBackClr(RGB(255, 255, 255));
			InvalidateCellRect(_EqualCell.row, _EqualCell.col);
		}
	}

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	if (call_year_month) {
		std::vector<VtSymbol*> call_symbol_list = call_year_month->SymbolList();
		// 콜과 풋의 심볼을 순회한다.
		for (int i = start_index, j = 1; i < call_symbol_list.size(); ++i, ++j) {
			// 행의 끝에 도달하면 탈출한다.
			if (i < 0 || j >= _MaxRow)
				break;
			VtSymbol* call_sym = call_symbol_list[i];
			_SymbolRowMap[call_sym->ShortCode] = std::make_tuple(j, 0, call_sym);
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

			
			CGridCellBase* pCell = GetCell(j, 1);
			if (pCell) {
				// 새로운 등가를 설정한다.
				if (call_sym == _EqualSymbol) {
					pCell->SetBackClr(RGB(255, 255, 0));
					_EqualCell.row = j;
					_EqualCell.col = 1;
				}
				pCell->SetText(strVal.c_str());
				InvalidateCellRect(j, 1);
			}
			pCell = GetCell(j, 0);
			if (pCell) {
				pCell->SetText("");
				pCell->SetData((LPARAM)call_sym);
				pCell->SetBackClr(RGB(252, 226, 228));
				InvalidateCellRect(j, 0);
			}
		}
	}

	if (put_year_month) {
		std::vector<VtSymbol*> put_symbol_list = put_year_month->SymbolList();
		// 콜과 풋의 심볼을 순회한다.
		for (int i = start_index, j = 1; i < put_symbol_list.size(); ++i, ++j) {
			// 행의 끝에 도달하면 탈출한다.
			if (i < 0 || j >= _MaxRow)
				break;
			VtSymbol* put_sym = put_symbol_list[i];
			_SymbolRowMap[put_sym->ShortCode] = std::make_tuple(j, 2, put_sym);

			CGridCellBase* pCell = GetCell(j, 2);
			if (pCell) {
				pCell->SetText("");
				pCell->SetData((LPARAM)put_sym);
				pCell->SetBackClr(RGB(218, 226, 245));
				InvalidateCellRect(j, 2);
			}
		}
	}
}

void SmOptionGrid::OnSymbolMaster(VtSymbol* sym)
{
	ShowCurrent(sym);
}

void SmOptionGrid::OnOrderFilled(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (posi)
			SetRemain(posi);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
}

void SmOptionGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}

void SmOptionGrid::OnExpected(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 2) {
		ShowExpected(sym);
	}
}

#include "stdafx.h"
#include "SmRealtickGrid.h"
#include "VtQuote.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtQuoteManager.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"
#include "VtGlobal.h"
#include <numeric>
#include <functional>
#include "VtGlobal.h"
#include "SmOrderPanelOut.h"
#include "SmCallbackManager.h"
using Poco::NumberFormatter;
using namespace std;
using namespace std::placeholders;
using Poco::NumberFormatter;

SmRealtickGrid::SmRealtickGrid()
{
}


SmRealtickGrid::~SmRealtickGrid()
{
	UnregisterAllCallback();
}


void SmRealtickGrid::UnregisterAllCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void SmRealtickGrid::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&SmRealtickGrid::OnQuoteEvent, this, _1));
}

void SmRealtickGrid::OnQuoteEvent(VtSymbol* sym)
{
	if (!sym || !_Symbol)
		return;

	if (_Symbol->ShortCode.compare(sym->ShortCode) != 0)
		return;

	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		if (!pCell)
			break;
		pCell->SetText(item.Time.c_str());
		InvalidateCellRect(i, 0);
		std::string close = NumberFormatter::format(item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);
		pCell = GetCell(i, 1);
		pCell->SetText(close.c_str());
		InvalidateCellRect(i, 1);
		CString qty;
		qty.Format("%d", item.ContQty);
		pCell = GetCell(i, 2);
		pCell->SetText(qty);
		if (item.MatchKind == 1)
			pCell->SetTextClr(RGB(255, 0, 0));
		else
			pCell->SetTextClr(RGB(0, 0, 255));
		InvalidateCellRect(i, 2);
		i++;
	}

	if (sym->Quote.QuoteItemQ.size() > m_nRows)
		sym->Quote.QuoteItemQ.pop_back();
}

BEGIN_MESSAGE_MAP(SmRealtickGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void SmRealtickGrid::Init()
{
	_CellHeight = 18;
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

	for (int i = 1; i < _RowCount; i++) {
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
}

void SmRealtickGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	LPCTSTR title[3] = { "시각", "체결가", "체결" };
	_ColWidths.push_back(60);
	_ColWidths.push_back(55);
	_ColWidths.push_back(41);
	SetRowHeight(0, 22);

	for (int i = 0; i < _ColCount; i++) {
		SetColumnWidth(i, _ColWidths[i]);
		CGridCellBase* pCell = GetCell(0, i);
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pCell->SetText(title[i]);
		pCell->SetBackClr(VtGlobal::GridTitleBackColor);
		pCell->SetTextClr(VtGlobal::GridTitleTextColor);
		InvalidateCellRect(0, i);
	}
}

void SmRealtickGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	ClearText();
	int i = 1;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		CGridCellBase* pCell = nullptr;
		pCell = GetCell(i, 0);
		if (pCell) {
			pCell->SetText(item.Time.c_str());
			InvalidateCellRect(i, 0);
			std::string close = NumberFormatter::format(item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);
			pCell = GetCell(i, 1);
			pCell->SetText(close.c_str());
			InvalidateCellRect(i, 1);
			CString qty;
			qty.Format("%d", item.ContQty);
			pCell = GetCell(i, 2);
			pCell->SetText(qty);
			if (item.MatchKind == 1)
				pCell->SetTextClr(RGB(255, 0, 0));
			else
				pCell->SetTextClr(RGB(0, 0, 255));
			InvalidateCellRect(i, 2);
		}
		i++;
		if (m_nRows == i)
			break;
	}

	if (sym->Quote.QuoteItemQ.size() > m_nRows)
		sym->Quote.QuoteItemQ.pop_back();
}

void SmRealtickGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void SmRealtickGrid::ClearText()
{
	for (int i = 1; i < _RowCount; i++) {
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				pCell->SetText("");
				InvalidateCellRect(i, j);
			}
		}
	}
}

void SmRealtickGrid::MaxRow(int val)
{
	_RowCount = val;
	SetRowCount(val);
	for (int i = 1; i < _RowCount; i++) {
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
}

void SmRealtickGrid::ClearValues()
{
	for (int i = 1; i < _RowCount; i++) {
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			pCell->SetText("");
			InvalidateCellRect(i, j);
		}
	}
}

int SmRealtickGrid::GetGridWidth()
{
	return std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0) + 6;
}

void SmRealtickGrid::Symbol(VtSymbol* val)
{
	_Symbol = val;
	OnReceiveQuote(val);
}

void SmRealtickGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
// 	if (_OrderConfigMgr && _OrderConfigMgr->orderWnd && _CenterWnd) {
// 		_OrderConfigMgr->orderWnd->SetActiveCenterWnd(_CenterWnd);
// 	}
}

void SmRealtickGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
// 	if (_OrderConfigMgr && _OrderConfigMgr->orderWnd && _CenterWnd) {
// 		_OrderConfigMgr->orderWnd->SetActiveCenterWnd(_CenterWnd);
// 	}
}

int SmRealtickGrid::GetMaxRow()
{
	int rowHeight = _CellHeight;

	RECT rect;
	GetWindowRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 1;
}


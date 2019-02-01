#include "stdafx.h"
#include "VtRealtimeTickQuoteGrid.h"
#include "VtQuote.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtQuoteManager.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWndHd.h"
#include "VtGlobal.h"
#include <numeric>
using Poco::NumberFormatter;

VtRealtimeTickQuoteGrid::VtRealtimeTickQuoteGrid()
{
	_MaxRow = 20;
	_CellHeight = 18;
}


VtRealtimeTickQuoteGrid::~VtRealtimeTickQuoteGrid()
{
}

void VtRealtimeTickQuoteGrid::OnReceiveRealtimeQuote(VtQuote* quote)
{
	if (!quote)
		return;
	int i = 0;
	int maxRow = _MaxRow;
	for (auto it = quote->QuoteItemQ.begin(); it != quote->QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		QuickSetText(0, i, item.Time.c_str());
		std::string close = NumberFormatter::format(item.ClosePrice, item.Decimal);
		QuickSetText(1, i, close.c_str());
		QuickSetNumber(2, i, item.ContQty);
		if (item.MatchKind == 1)
			QuickSetTextColor(2, i, RGB(255, 0, 0));
		else
			QuickSetTextColor(2, i, RGB(0, 0, 255));
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		i++;
		if (maxRow == i)
			break;
	}

	if (quote->QuoteItemQ.size() > maxRow)
		quote->QuoteItemQ.pop_back();
}

void VtRealtimeTickQuoteGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtRealtimeTickQuoteGrid::ClearText()
{
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(0, i, _T(""));
		QuickRedrawCell(0, i);

		QuickSetText(1, i, _T(""));
		QuickRedrawCell(1, i);

		QuickSetText(2, i, _T(""));
		QuickRedrawCell(2, i);
	}
}

void VtRealtimeTickQuoteGrid::SetQuote()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Symbol())
		return;

	VtSymbol* sym = _OrderConfigMgr->Symbol();
	VtQuoteManager* quoteMgr = VtQuoteManager::GetInstance();
	VtQuote* quote = quoteMgr->FindQuote(sym->FullCode);
	if (quote) {
		OnReceiveRealtimeQuote(quote);
	}
}

void VtRealtimeTickQuoteGrid::MaxRow(int val)
{
	int maxRow = GetMaxRow();
	if (val < 3 || val > maxRow)
		return;
	SetNumberRows(val);
	_MaxRow = val;
	for (int i = 0; i < val; i++) {
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);

		QuickSetFont(0, i, &_defFont);
		QuickSetFont(1, i, &_defFont);
		QuickSetFont(2, i, &_defFont);
		SetRowHeight(i, _CellHeight);
	}
	RedrawAll();
}

void VtRealtimeTickQuoteGrid::ClearValues()
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

int VtRealtimeTickQuoteGrid::GetGridWidth()
{
	return std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0) + 6;
}

void VtRealtimeTickQuoteGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	//SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(60);
	_RowCount = _MaxRow;

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);

		QuickSetFont(0, i, &_defFont);
		QuickSetFont(1, i, &_defFont);
		QuickSetFont(2, i, &_defFont);
		SetRowHeight(i, _CellHeight);
	}

	SetDefFont(&_defFont);
	SetSH_Width(1);
	SetVS_Width(0);
	SetHS_Height(0);
	SetColTitle();
}

void VtRealtimeTickQuoteGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[3] = { "시각", "체결가", "체결" };
	_ColWidths.push_back(60);
	_ColWidths.push_back(55);
	_ColWidths.push_back(35);

	//SetRowHeight(-1, 22);

	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, _ColWidths[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_defFont);
		RedrawCell(i, -1);
	}
}

void VtRealtimeTickQuoteGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd && _CenterWnd) {
		_OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
}

void VtRealtimeTickQuoteGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtRealtimeTickQuoteGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtRealtimeTickQuoteGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	int i = 0;
	int maxRow = _MaxRow;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		QuickSetText(0, i, item.Time.c_str());
		std::string close = NumberFormatter::format(item.ClosePrice / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		QuickSetText(1, i, close.c_str());
		QuickSetNumber(2, i, item.ContQty);
		if (item.MatchKind == 1)
			QuickSetTextColor(2, i, RGB(255, 0, 0));
		else
			QuickSetTextColor(2, i, RGB(0, 0, 255));
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		i++;
		if (maxRow == i)
			break;
	}

	if (sym->Quote.QuoteItemQ.size() > maxRow)
		sym->Quote.QuoteItemQ.pop_back();
}

int VtRealtimeTickQuoteGrid::GetMaxRow()
{
	int rowHeight = _CellHeight;

	RECT rect;
	GetWindowRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 1;
}


#include "stdafx.h"
#include "VtSysLogGrid.h"
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

VtSysLogGrid::VtSysLogGrid()
{
	_CellHeight = 18;
	_RowCount = 120;
	_MaxRow = _RowCount;
}


VtSysLogGrid::~VtSysLogGrid()
{
}

void VtSysLogGrid::OnReceiveRealtimeQuote(VtQuote* quote)
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

void VtSysLogGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtSysLogGrid::ClearText()
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

void VtSysLogGrid::SetQuote()
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

void VtSysLogGrid::MaxRow(int val)
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

void VtSysLogGrid::ClearValues()
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

int VtSysLogGrid::GetGridWidth()
{
	return std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0) + 6;
}

void VtSysLogGrid::UpdateLog()
{
	int i = 0;
	int maxRow = _MaxRow;
	std::list<VtSysLog>& logList = VtGlobal::GetSysLogList();
	for (auto it = logList.begin(); it != logList.end(); ++it) {
		VtSysLog item = *it;
		QuickSetText(0, i, item.LogTime.c_str());
		QuickSetNumber(1, i, item.ErrorCode);
		QuickSetText(2, i, item.LogText.c_str());
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		i++;
		if (maxRow == i)
			break;
	}
}

void VtSysLogGrid::UpdateOrderLog(std::vector<std::pair<std::string, std::string>> log_vec)
{
	_OrderLogVec = log_vec;
	InitOrderLog();
}

void VtSysLogGrid::Resize(CRect& rect)
{
	_MaxRow = GetMaxRow(rect);
	SetColWidth(2, rect.Width() - 120 - 5);
	InitOrderLog();
}

void VtSysLogGrid::OnSetup()
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
		if (_Mode == 0)
			QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		else
			QuickSetAlignment(2, i, UG_ALIGNLEFT | UG_ALIGNVCENTER);

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
	if (_Mode == 0)
		UpdateLog();
}

void VtSysLogGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	if (_Mode == 0) {
		CUGCell cell;
		LPCTSTR title[3] = { "시간", "오류코드", "오류내용" };
		_ColWidths.push_back(120);
		_ColWidths.push_back(80);
		_ColWidths.push_back(262);

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
	else {
		CUGCell cell;
		LPCTSTR title[3] = { "시간", "코드", "주문내용" };
		_ColWidths.push_back(120);
		_ColWidths.push_back(0);
		_ColWidths.push_back(342);

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
}

void VtSysLogGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd && _CenterWnd) {
		_OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
}

void VtSysLogGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtSysLogGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSysLogGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	int i = 0;
	int maxRow = _MaxRow;
	for (auto it = sym->Quote.QuoteItemQ.begin(); it != sym->Quote.QuoteItemQ.end(); ++it) {
		VtQuoteItem item = *it;
		QuickSetText(0, i, item.Time.c_str());
		std::string close = NumberFormatter::format(item.ClosePrice / std::pow(10, sym->Decimal), sym->Decimal);
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

void VtSysLogGrid::InitOrderLog()
{
	int i = 0;
	for (auto it = _OrderLogVec.begin(); it != _OrderLogVec.end(); ++it) {
		std::pair<std::string, std::string> item = *it;
		QuickSetText(0, i, item.first.c_str());
		QuickSetNumber(1, i, i);
		QuickSetText(2, i, item.second.c_str());
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		i++;
		if (_MaxRow == i)
			break;
	}
}

int VtSysLogGrid::GetMaxRow()
{
	int rowHeight = _CellHeight;

	RECT rect;
	GetWindowRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 1;
}

int VtSysLogGrid::GetMaxRow(CRect& rect)
{
	int rowHeight = _CellHeight;

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 1;
}


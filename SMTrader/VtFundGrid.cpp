#include "stdafx.h"
#include "VtFundGrid.h"
#include "VtFundAccount.h"
#include "VtFundEditor.h"
#include "VtFDMultiAcntPage.h"
#include "Format/format.h"
#include "VtAccount.h"
#include <vector>
#include "VtGlobal.h"
VtFundGrid::VtFundGrid()
{
	_SelRow = 0;
	_FundEditor = nullptr;
	_FundPage = nullptr;
	_Mode = 0;
}


VtFundGrid::~VtFundGrid()
{
}

void VtFundGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtFundGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (_FundEditor && _FundEditor->TargetVector.size() == 0)
		return;
	if (_Mode == 1) {
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

		CUGCell cell;
		GetCell(0, _ClickedRow, &cell);
		if (_FundEditor) {
			_FundEditor->SelFundAcnt((VtAccount*)cell.Tag());
			_FundEditor->SelectedTargetRow(_ClickedRow);
		}

		if (_FundPage) {
			_FundPage->SelectedTargetRow(_ClickedRow);
		}

		_FundEditor->OnBnClickedBtnAcntOut();
	}
}

void VtFundGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_FundEditor && _FundEditor->TargetVector.size() == 0)
		return;

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

	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	if (_FundEditor) {
		_FundEditor->SelFundAcnt((VtAccount*)cell.Tag());
		_FundEditor->SelectedTargetRow(_ClickedRow);
	}

	if (_FundPage) {
		_FundPage->SelectedTargetRow(_ClickedRow);
	}
}

void VtFundGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_FundEditor && _FundEditor->TargetVector.size() == 0)
		return;

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

	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	if (_FundEditor) {
		_FundEditor->SelFundAcnt((VtAccount*)cell.Tag());
		_FundEditor->SelectedTargetRow(_ClickedRow);
	}

	if (_FundPage) {
		_FundPage->SelectedTargetRow(_ClickedRow);
	}
}

int VtFundGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtFundGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
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

void VtFundGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtFundGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[4] = { "°èÁÂ¹øÈ£", "°èÁÂ¸í", "½Â¼ö", "ºñÀ²" };

	std::vector<int> colWidth; // [4] = { 80, 160, 40, 75 };
	if (_Mode == 0)
	{
		colWidth.push_back(80);
		colWidth.push_back(160);
		colWidth.push_back(40);
		colWidth.push_back(62);
	}
	else
	{
		colWidth.push_back(72);
		colWidth.push_back(88);
		colWidth.push_back(28);
		colWidth.push_back(54);
	}


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

void VtFundGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtFundGrid::InitGrid(std::map<std::string, VtAccount*>& fundAcntMap)
{
	int i = 0;
	CUGCell cell;
	for (auto it = fundAcntMap.begin(); it != fundAcntMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);
		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		QuickSetNumber(2, i, acnt->SeungSu);
		std::string val = fmt::format("{:.{}f}", acnt->Ratio, 2);
		QuickSetText(3, i, val.c_str());
		if (i == _ClickedRow && fundAcntMap.size() > 0)
		{
			if (_FundEditor)
			{
				_FundEditor->SelFundAcnt(acnt);
			}
			for (int j = 0; j < _ColCount; j++)
			{
				QuickSetBackColor(j, i, _ClickedColor);
			}
		}
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		QuickRedrawCell(3, i);
		i++;
	}
}

void VtFundGrid::InitGrid(std::vector<VtAccount*>& fundAcntVector)
{
	int i = 0;
	CUGCell cell;
	for (auto it = fundAcntVector.begin(); it != fundAcntVector.end(); ++it) {
		VtAccount* acnt = *it;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);
		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		QuickSetNumber(2, i, acnt->SeungSu);
		std::string val = fmt::format("{:.{}f}%", acnt->Ratio * 100, 2);
		QuickSetText(3, i, val.c_str());
		if (i == _ClickedRow && fundAcntVector.size() > 0) {
			for (int j = 0; j < _ColCount; j++) {
				QuickSetBackColor(j, i, _ClickedColor);
			}
		}
		for (int j = 0; j < _ColCount; j++) {
			_RefreshArea.insert(std::make_pair(j, i));
		}
		i++;
	}

	GetCell(0, _ClickedRow, &cell);
	if (_FundEditor) {
		_FundEditor->SelFundAcnt((VtAccount*)cell.Tag());
	}

	for (auto it = _RefreshArea.begin(); it != _RefreshArea.end(); ++it) {
		std::pair<int, int> item = *it;
		QuickRedrawCell(item.first, item.second);
	}
}

void VtFundGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++)
	{
		for (int j = 0; j < _ColCount; j++)
		{
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickSetText(j, i, _T(""));
			//QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
			_RefreshArea.insert(std::make_pair(j, i));
		}
	}
}

void VtFundGrid::ChangeSelectedRow(int oldRow, int newRow)
{
	_SelRow = newRow;
	CUGCell cell;
	GetCell(0, _SelRow, &cell);
	if (_FundEditor)
	{
		_FundEditor->SelFundAcnt((VtAccount*)cell.Tag());
		_FundEditor->SelectedTargetRow(newRow);
	}

	if (_FundPage)
	{
		_FundPage->SelectedTargetRow(newRow);
	}

	for (int j = 0; j < _ColCount; j++)
	{
		QuickSetBackColor(j, oldRow, RGB(255, 255, 255));
		QuickRedrawCell(j, oldRow);
	}

	for (int j = 0; j < _ColCount; j++)
	{
		QuickSetBackColor(j, newRow, _SelColor);
		QuickRedrawCell(j, newRow);
	}
}

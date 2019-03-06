#include "stdafx.h"
#include "VtSubAcntGrid.h"
#include "VtAccount.h"
#include "VtFDSubAcntPage.h"
#include "VtGlobal.h"
VtSubAcntGrid::VtSubAcntGrid()
{
	_SubAcntPage = nullptr;
	_ParentAccount = nullptr;
}


VtSubAcntGrid::~VtSubAcntGrid()
{
}

void VtSubAcntGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

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
	SetVS_Width(0);
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtSubAcntGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtSubAcntGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == FALSE)
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

	if (_SubAcntPage)
	{
		_SubAcntPage->SelSubAccount((VtAccount*)cell.Tag());
	}
}

void VtSubAcntGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == FALSE)
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

	VtAccount* subAcnt = (VtAccount*)cell.Tag();
	if (subAcnt && _ParentAccount)
	{
		if (subAcnt->Prime())
		{
			AfxMessageBox(_T("기본계좌는 지울 수 없습니다.!"));
			return;
		}
		_ParentAccount->RemoveSubAccount(subAcnt->AccountNo);
		if (_SubAcntPage)
		{
			_SubAcntPage->RefreshSubAccountList(_ParentAccount);
		}
	}

}

void VtSubAcntGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
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

void VtSubAcntGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtSubAcntGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "계좌번호", "계좌명", "구분" };
	int colWidth[3] = { 80, 120, 60 };


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

void VtSubAcntGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);

}

void VtSubAcntGrid::InitGrid(VtAccount* acnt)
{
	if (!acnt)
		return;
	_ParentAccount = acnt;

	_RefreshCells.clear();

	ClearCells();

	int i = 0;
	CUGCell cell;
	std::vector<VtAccount*>& acntMap = acnt->GetSubAccountList();
	for (auto it = acntMap.begin(); it != acntMap.end(); ++it)
	{
		VtAccount* acnt = *it;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);

		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		if (acnt->Prime())
			QuickSetText(2, i, _T("기본"));
		else
			QuickSetText(2, i, _T("일반"));
		if (i == _ClickedRow && acntMap.size() > 0)
		{
			for (int j = 0; j < _ColCount; j++)
			{
				QuickSetBackColor(j, i, _ClickedColor);
			}
		}

		_RefreshCells.insert(std::make_pair(0, i));
		_RefreshCells.insert(std::make_pair(1, i));
		_RefreshCells.insert(std::make_pair(2, i));
		i++;
	}

	for (auto it = _RefreshCells.begin(); it != _RefreshCells.end(); ++it) {
		std::pair<int, int> cellInfo = *it;
		QuickRedrawCell(cellInfo.first, cellInfo.second);
	}
}

void VtSubAcntGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			_RefreshCells.insert(std::make_pair(j, i));
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
		}
	}
}

VtAccount* VtSubAcntGrid::GetSelectedAccount()
{
	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	return (VtAccount*)cell.Tag();
}

void VtSubAcntGrid::SetDefaultSelect()
{
	ChangeSelectedRow(_ClickedRow, 0);
	_ClickedRow = 0;
	CUGCell cell;
	GetCell(0, _ClickedRow, &cell);
	if (_SubAcntPage) {
		_SubAcntPage->SelSubAccount((VtAccount*)cell.Tag());
	}
}

void VtSubAcntGrid::ChangeSelectedRow(int oldRow, int newRow)
{
	for (int j = 0; j < _ColCount; j++) {
		QuickSetBackColor(j, oldRow, RGB(255, 255, 255));
		QuickRedrawCell(j, oldRow);
	}

	for (int j = 0; j < _ColCount; j++) {
		QuickSetBackColor(j, newRow, _SelColor);
		QuickRedrawCell(j, newRow);
	}
}

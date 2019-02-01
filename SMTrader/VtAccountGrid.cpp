#include "stdafx.h"
#include "VtAccountGrid.h"
#include "VtAccount.h"
#include "VtFundEditor.h"
#include "VtFundAccount.h"
#include "VtAccountManager.h"
#include "VtFDSubAcntPage.h"
#include "VtGlobal.h"

VtAccountGrid::VtAccountGrid()
{
	_SelRow = 0;
	_FundEditor = nullptr;
	_SubAcntPage = nullptr;
}


VtAccountGrid::~VtAccountGrid()
{
}

void VtAccountGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

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

void VtAccountGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (_FundEditor && _FundEditor->SourceVector.size() == 0)
		return;
	ChangeSelectedRow(_SelRow, row);
	_FundEditor->OnBnClickedBtnAcntIn();
}

void VtAccountGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_FundEditor && _FundEditor->SourceVector.size() == 0)
		return;

	ChangeSelectedRow(_SelRow, row);
	CUGCell cell;
	GetCell(0, _SelRow, &cell);
	if (_SubAcntPage)
	{
		_SubAcntPage->SelAccount((VtAccount*)cell.Tag());
	}
	if (_FundEditor)
	{
		_FundEditor->SelAcnt((VtAccount*)cell.Tag());
	}
}

void VtAccountGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_FundEditor && _FundEditor->SourceVector.size() == 0)
		return;

	ChangeSelectedRow(_SelRow, row);
	CUGCell cell;
	GetCell(0, _SelRow, &cell);
	if (_FundEditor)
	{
		_FundEditor->SelAcnt((VtAccount*)cell.Tag());
	}
}

void VtAccountGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "°èÁÂ¹øÈ£", "°èÁÂ¸í", "±¸ºÐ" };
	int colWidth[3] = { 80, 110, 50 };


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

void VtAccountGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtAccountGrid::InitGrid(std::vector<VtAccount*>& acntVector)
{
	int i = 0;
	CUGCell cell;
	for (auto it = acntVector.begin(); it != acntVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);

		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		if (acnt->Prime())
			QuickSetText(2, i, _T("±âº»"));
		else
			QuickSetText(2, i, _T("ÀÏ¹Ý"));
		if (i == _SelRow && acntVector.size() > 0)
		{
			if (_FundEditor)
			{
				_FundEditor->SelAcnt(acnt);
			}
			for (int j = 0; j < _ColCount; j++)
			{
				QuickSetBackColor(j, i, RGB(234, 0, 234));
			}
		}
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		i++;
	}
}

void VtAccountGrid::InitGrid()
{
	int i = 0;
	CUGCell cell;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		GetCell(0, i, &cell);
		cell.Tag(acnt);
		SetCell(0, i, &cell);

		QuickSetText(0, i, acnt->AccountNo.c_str());
		QuickSetText(1, i, acnt->AccountName.c_str());
		if (i == _SelRow && acntMgr->AccountMap.size() > 0)
		{
			for (int j = 0; j < _ColCount; j++)
			{
				QuickSetBackColor(j, i, RGB(234, 0, 234));
			}
		}
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		i++;
	}
}

void VtAccountGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++)
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

VtAccount* VtAccountGrid::GetSelectedAccount()
{
	CUGCell cell;
	GetCell(0, _SelRow, &cell);
	
	return (VtAccount*)cell.Tag();
}

void VtAccountGrid::ChangeSelectedRow(int oldRow, int newRow)
{
	for (int j = 0; j < _ColCount; j++)
	{
		QuickSetBackColor(j, oldRow, RGB(255, 255, 255));
		QuickRedrawCell(j, oldRow);
	}

	for (int j = 0; j < _ColCount; j++)
	{
		QuickSetBackColor(j, newRow, RGB(234, 0, 234));
		QuickRedrawCell(j, newRow);
	}
	_SelRow = newRow;
}

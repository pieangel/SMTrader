#include "stdafx.h"
#include "VtFavoritGrid.h"
#include "VtGlobal.h"
#include "VtSymbol.h"
#include <string>
#include "VtSymbolFavoriteDlg.h"
#include "VtOrderCenterWnd.h"

VtFavoritGrid::VtFavoritGrid()
{
}


VtFavoritGrid::~VtFavoritGrid()
{
}

void VtFavoritGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		SetRowHeight(yIndex, 30);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			//m_data[yIndex].m_colData[xIndex] = rand() % 100;
			CString msg;
			msg.Format(_T("%d"), yIndex + 1);
			if (xIndex == 0)
				QuickSetText(xIndex, yIndex, msg);
			QuickSetFont(xIndex, yIndex, &_defFont);
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetCellTypeEx(xIndex, yIndex, UGCT_NORMALMULTILINE);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtFavoritGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);

	if (col == 2)
	{
		StartEdit();
	}
}

void VtFavoritGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			if (yIndex != row)
			{
				QuickSetBackColor(xIndex, yIndex, RGB(255, 255, 255));
			}
			else
			{
				QuickSetBackColor(xIndex, yIndex, RGB(255, 255, 0));
				if (xIndex == 0)
				{
					GetCell(xIndex, yIndex, &cell);
					if (_SymFvrtDlg && cell.Tag())
					{
						_SymFvrtDlg->DstSymbol((VtSymbol*)cell.Tag());
					}
				}
			}
			
			RedrawCell(xIndex, yIndex);
		}
	}
}

int VtFavoritGrid::OnEditStart(int col, long row, CWnd **edit)
{
	return TRUE;
}

int VtFavoritGrid::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	return TRUE;
}

int VtFavoritGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	CString msg;
	edit->GetWindowText(msg);
	CUGCell cell;
	GetCell(0, row, &cell);
	VtSymbol* sym = (VtSymbol*)cell.Tag();
	if (sym)
	{
		std::string alias = msg;
		sym->UserDefinedName = alias;
		if (_SymFvrtDlg->OrderCenterWnd)
		{
			_SymFvrtDlg->OrderCenterWnd->SetFavorite(row, sym);
		}
	}
	return TRUE;
}

int VtFavoritGrid::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

void VtFavoritGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "버튼순서", "종목명", "별칭입력"};
	int colWidth[9] = { 44, 160, 80 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
}


void VtFavoritGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}
void VtFavoritGrid::SetFavorite()
{
	ClearAllText();
	VtGlobal* global = VtGlobal::GetInstance();
	int i = 0;
	CUGCell cell;
	for (auto it = global->FavoriteSymbolList.begin(); it != global->FavoriteSymbolList.end(); ++it)
	{
		VtSymbol* sym = *it;
		GetCell(0, i, &cell);
		cell.Tag(sym);
		SetCell(0, i, &cell);
		CString shortName;
		shortName = sym->Name.c_str();
		int found = shortName.Find(_T("년"));
		shortName = shortName.Left(found + 4);
		shortName.Replace(_T("년"), _T("."));
		//shortName.Replace(_T("월"), _T(""));

		QuickSetText(1, i, shortName);
		QuickRedrawCell(1, i);
		if (sym->UserDefinedName.length() > 0)
		{
			QuickSetText(2, i, sym->UserDefinedName.c_str());
			QuickRedrawCell(2, i);
		}
		i++;
	}
}

void VtFavoritGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtFavoritGrid::ClearAllText()
{
	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			if (xIndex == 0)
			{
				GetCell(0, yIndex, &cell);
				cell.Tag(nullptr);
			}
			else
				QuickSetText(xIndex, yIndex, _T(""));
			RedrawCell(xIndex, yIndex);
		}
	}
}

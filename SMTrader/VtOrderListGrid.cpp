#include "stdafx.h"
#include "VtOrderListGrid.h"
#include "VtGlobal.h"
#include <string>

VtOrderListGrid::VtOrderListGrid()
{
}


VtOrderListGrid::~VtOrderListGrid()
{
}

void VtOrderListGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	_CenterFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	_CursorFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);


	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, 18);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}


	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetColTitle();
	SetVS_Width(0);
	SetHS_Height(0);

	EnableMenu(TRUE);
}

int VtOrderListGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void VtOrderListGrid::SetColTitle()
{
	const int ColCount = _ColCount;

	JoinCells(0, 0, 0, 1); // �����ڵ�
	JoinCells(1, 0, 1, 1); // �ֹ�����
	JoinCells(2, 0, 4, 0); // �ֹ�
	JoinCells(5, 0, 6, 0); // ü��
	JoinCells(7, 0, 7, 1); // ����
	JoinCells(8, 0, 8, 1); // ���� �ð�
	JoinCells(9, 0, 9, 1); // �ֹ���ȣ
	JoinCells(10, 0, 10, 1); // ���ֹ���ȣ
	JoinCells(11, 0, 11, 1); // �ֹ�Ÿ��
	QuickSetText(0, 0, _T("�����ڵ�"));
	QuickSetText(1, 0, _T("�ֹ�����"));
	QuickSetText(2, 0, _T("�ֹ�"));
	QuickSetText(5, 0, _T("ü��"));
	QuickSetText(7, 0, _T("����"));
	QuickSetText(8, 0, _T("��������"));
	QuickSetText(9, 0, _T("�ֹ���ȣ"));
	QuickSetText(10, 0, _T("���ֹ���ȣ"));
	QuickSetText(11, 0, _T("�ֹ�Ÿ��"));
	QuickSetText(2, 1, _T("����"));
	QuickSetText(3, 1, _T("����"));
	QuickSetText(4, 1, _T("��ü��"));
	QuickSetText(5, 1, _T("����"));
	QuickSetText(6, 1, _T("��մܰ�"));

	std::vector < std::pair<int, int>> cellPosVec;
	cellPosVec.push_back(std::make_pair(0, 0));
	cellPosVec.push_back(std::make_pair(1, 0));
	cellPosVec.push_back(std::make_pair(2, 0));
	cellPosVec.push_back(std::make_pair(5, 0));
	cellPosVec.push_back(std::make_pair(7, 0));
	cellPosVec.push_back(std::make_pair(8, 0));
	cellPosVec.push_back(std::make_pair(9, 0));
	cellPosVec.push_back(std::make_pair(10, 0));
	cellPosVec.push_back(std::make_pair(11, 0));
	cellPosVec.push_back(std::make_pair(2, 1));
	cellPosVec.push_back(std::make_pair(3, 1));
	cellPosVec.push_back(std::make_pair(4, 1));
	cellPosVec.push_back(std::make_pair(5, 1));
	cellPosVec.push_back(std::make_pair(6, 1));
	CUGCell cell;
	for (auto it = cellPosVec.begin(); it != cellPosVec.end(); ++it) {
		std::pair<int, int> pos = *it;
		GetCell(std::get<0>(pos), std::get<1>(pos), &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_defFont);
		SetCell(std::get<0>(pos), std::get<1>(pos), &cell);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}	
}

void VtOrderListGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

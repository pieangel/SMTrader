#include "stdafx.h"
#include "VtChartFavoriteGrid.h"
#include "VtChartWindow.h"
#include "VtChartData.h"
#include <vector>
#include "VtColorManager.h"
#include "VtSymbol.h"
#include "VtChartContainer.h"
#include "Format/format.h"
#include "conversion.hpp"
#include "Chart/SmChartDefine.h"

VtChartFavoriteGrid::VtChartFavoriteGrid()
{
	_ColorMgr = new VtColorManager();
	_ChartContainer = nullptr;
}


VtChartFavoriteGrid::~VtChartFavoriteGrid()
{
	if (_ColorMgr)
	{
		delete _ColorMgr;
		_ColorMgr = nullptr;
	}
}

void VtChartFavoriteGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
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
		SetRowHeight(i, 25);
	}


	SetDefFont(&_defFont);

	JoinCells(0, -1, 2, -1);
	CUGCell cell;
	GetCell(0, -1, &cell);
	SetRowHeight(-1, 30);
	cell.SetText("즐겨찾기");
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetBackColor(RGB(0, 67, 178));
	SetCell(0, -1, &cell);


	SetColTitle();

	InitGrid();
}

void VtChartFavoriteGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == TRUE && col != 0) {
		CUGCell cell;
		GetCell(0, row, &cell);
		SmChartDataSource* data = (SmChartDataSource*)cell.Tag();
		if (_ChartContainer && data) {
			std::string selectedCode;
			if (row == 1) {
				selectedCode =data->symbol->ShortCode;
			}
			else {
				selectedCode = data->symbol->ShortCode;
			}
			_ChartContainer->SetSelectedChartData(selectedCode);
		}
	}
}

int VtChartFavoriteGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		CUGCell cell;
		GetCell(col, row, &cell);
		SmChartDataSource* data = (SmChartDataSource*)cell.Tag();
		double num = cell.GetNumber();
		if (num == 0) {
			if (_ChartContainer && data)
				_ChartContainer->ShowRefChart(data->symbol->ShortCode, false);
		}
		else {
			if (_ChartContainer && data)
				_ChartContainer->ShowRefChart(data->symbol->ShortCode, true);
		}
	}
	return 0;
}

void VtChartFavoriteGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "선택", "종목코드", "현재가" };
	_ColWidthVector.push_back(30);
	_ColWidthVector.push_back(70);
	_ColWidthVector.push_back(100);
	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, _ColWidthVector[i]);
		QuickSetText(i, 0, title[i]);
		GetCell(i, 0, &cell);
		cell.SetBackColor(RGB(0, 67, 178));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(i, 0, &cell);
		QuickSetFont(i, 0, &_titleFont);
		RedrawCell(i, 0);
	}
}

void VtChartFavoriteGrid::ResizeColWidth(int parendWidth)
{
	SetColWidth(0, _ColWidthVector[0]);
	SetColWidth(1, _ColWidthVector[1]);
	SetColWidth(2, parendWidth - (_ColWidthVector[0]  + _ColWidthVector[1]));
}

void VtChartFavoriteGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtChartFavoriteGrid::InitGrid()
{

}

void VtChartFavoriteGrid::SetChartData(VtChartWindow* activeWnd)
{
	if (!activeWnd)
		return;

	std::vector<SmChartDataSource*> dataList = activeWnd->GetChartDataList();
	int i = 1;
	for (auto it = dataList.begin(); it != dataList.end(); ++it) {
		SmChartDataSource* data = *it;
		QuickSetBackColor(0, i, _ColorMgr->GetColor(data->colorIndex, true));
		QuickSetBackColor(1, i, _ColorMgr->GetColor(data->colorIndex, true));
		QuickSetBackColor(2, i, _ColorMgr->GetColor(data->colorIndex, true));
		QuickSetTextColor(0, i, RGB(255, 255, 255));
		QuickSetTextColor(1, i, RGB(255, 255, 255));
		QuickSetTextColor(2, i, RGB(255, 255, 255));
		QuickSetText(1, i, data->symbol->ShortCode.c_str());
		QuickSetText(2, i, _T("0"));
		QuickSetCellType(0, i, UGCT_CHECKBOX);
		CUGCell cell;
		GetCell(0, i, &cell);
		cell.Tag(data);
		SetCell(0, i, &cell);
		QuickSetCellTypeEx(0, i, UGCT_CHECKBOXCHECKMARK | UGCT_CHECKBOX3DRAISED);
		QuickSetNumber(0, i, 1);
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		_DataMap[data->symbol->ShortCode] = std::make_pair(i, data);
		i++;
	}
}

void VtChartFavoriteGrid::OnReceiveChartData(std::string symCode)
{
	auto it = _DataMap.find(symCode);
	if (it != _DataMap.end()) {
		std::pair<int, SmChartDataSource*> item = it->second;
		std::string strValue = convert::string_cast<double>(std::get<1>(item)->symbol->Quote.close, std::get<1>(item)->symbol->Decimal, convert::thou_sep);
		QuickSetText(2, std::get<0>(item), strValue.c_str());
		QuickRedrawCell(2, std::get<0>(item));
	}
}

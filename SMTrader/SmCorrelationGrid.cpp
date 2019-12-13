#include "stdafx.h"
#include "SmCorrelationGrid.h"
#include <string>
#include <vector>
#include <map>
#include "VtGlobal.h"

SmCorrelationGrid::SmCorrelationGrid()
{
}


SmCorrelationGrid::~SmCorrelationGrid()
{
}

void SmCorrelationGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	SetUGMode(UGMode::OrderNormal);
	m_noteBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_S2));

	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(13, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CenterFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CursorFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);

	_CellHeight = 26;


	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _CellHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetBorderColor(xIndex, yIndex, &m_Pen);
		}
	}


	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetColTitle(_mode);
	SetRowTitle(_mode);
	SetVS_Width(0);
	SetHS_Height(0);

	for (int i = 1; i < 8; ++i) {
		for(int j = 1; j < 8; ++j)
		JoinCells(i * 2, j, i * 2 + 1, j);
	}

	for (int i = 0; i < 2; ++i) {
		JoinCells(0, i, 1, i);
	}

	for (int i = 2; i <= 7; ++i) {
		QuickSetBackColor(i * 2, i, RGB(202, 202, 202));
	}
}

void SmCorrelationGrid::SetColTitle(int mode)
{
	if (mode == 0) {
		std::vector<std::string> col_tile;
		col_tile.push_back("상품포트폴리오");
		col_tile.push_back("지수");
		col_tile.push_back("통화");
		col_tile.push_back("금리");
		col_tile.push_back("에너지");
		col_tile.push_back("금속");
		col_tile.push_back("농축산");
		std::vector<int> col_width;
		for (int i = 0; i < 8; ++i) {
			col_width.push_back(90);
			col_width.push_back(26);
		}
		for (size_t i = 0; i < col_width.size(); ++i) {
			SetColWidth(i, col_width[i]);
		}
		for (size_t i = 1; i <= col_tile.size(); ++i) {
			QuickSetText(i * 2, 0, col_tile[i - 1].c_str());
			QuickSetAlignment(i * 2, 0, UG_ALIGNLEFT | UG_ALIGNVCENTER);
			QuickSetBackColor(i * 2, 0, VtGlobal::GridTitleBackColor);
			QuickSetTextColor(i * 2, 0, VtGlobal::GridTitleTextColor);
		}

		CUGCell cell;
		for (size_t i = 1; i <= col_tile.size(); ++i) {
			GetCell(i * 2 + 1, 0, &cell);
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS | UGCT_NORMALELLIPSIS);
			cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
			//cell.SetParam(BUTTON_CLICK1);
			cell.SetBitmap(&m_noteBitmap);
			SetCell(i * 2 + 1, 0, &cell);
		}
	}
	else
	{
		std::vector<std::string> col_tile;
		col_tile.push_back("신호포트폴리오");
		col_tile.push_back("지수신호");
		col_tile.push_back("통화신호");
		col_tile.push_back("금리신호");
		col_tile.push_back("에너지신호");
		col_tile.push_back("금속신호");
		col_tile.push_back("농축산신호");
		std::vector<int> col_width;
		for (int i = 0; i < 8; ++i) {
			col_width.push_back(90);
			col_width.push_back(26);
		}
		for (size_t i = 0; i < col_width.size(); ++i) {
			SetColWidth(i, col_width[i]);
		}
		for (size_t i = 1; i <= col_tile.size(); ++i) {
			QuickSetText(i * 2, 0, col_tile[i - 1].c_str());
			QuickSetAlignment(i * 2, 0, UG_ALIGNLEFT | UG_ALIGNVCENTER);
			QuickSetBackColor(i * 2, 0, VtGlobal::GridTitleBackColor);
			QuickSetTextColor(i * 2, 0, VtGlobal::GridTitleTextColor);
		}

		CUGCell cell;
		for (size_t i = 1; i <= col_tile.size(); ++i) {
			GetCell(i * 2 + 1, 0, &cell);
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS | UGCT_NORMALELLIPSIS);
			cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
			//cell.SetParam(BUTTON_CLICK1);
			cell.SetBitmap(&m_noteBitmap);
			SetCell(i * 2 + 1, 0, &cell);
		}
	}
}

void SmCorrelationGrid::SetRowTitle(int mode)
{
	if (mode == 0) {
		std::vector<std::string> row_title;
		row_title.push_back("지수");
		row_title.push_back("통화");
		row_title.push_back("금리");
		row_title.push_back("에너지");
		row_title.push_back("금속");
		row_title.push_back("농축산");

		for (int i = 2; i < 8; i++) {
			QuickSetText(0, i, row_title[i - 2].c_str());
			QuickSetAlignment(0, i, UG_ALIGNLEFT | UG_ALIGNVCENTER);
			QuickSetBackColor(0, i, VtGlobal::GridTitleBackColor);
			QuickSetTextColor(0, i, VtGlobal::GridTitleTextColor);
		}

		CUGCell cell;
		for (size_t i = 2; i < 8; ++i) {
			GetCell(1, i, &cell);
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS | UGCT_NORMALELLIPSIS);
			cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
			//cell.SetParam(BUTTON_CLICK1);
			cell.SetBitmap(&m_noteBitmap);
			SetCell(1, i, &cell);
		}
	}
	else {
		std::vector<std::string> row_title;
		row_title.push_back("지수신호");
		row_title.push_back("통화신호");
		row_title.push_back("금리신호");
		row_title.push_back("에너지신호");
		row_title.push_back("금속신호");
		row_title.push_back("농축산신호");

		for (int i = 2; i < 8; i++) {
			QuickSetText(0, i, row_title[i - 2].c_str());
			QuickSetAlignment(0, i, UG_ALIGNLEFT | UG_ALIGNVCENTER);
			QuickSetBackColor(0, i, VtGlobal::GridTitleBackColor);
			QuickSetTextColor(0, i, VtGlobal::GridTitleTextColor);
		}

		CUGCell cell;
		for (size_t i = 2; i < 8; ++i) {
			GetCell(1, i, &cell);
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS | UGCT_NORMALELLIPSIS);
			cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
			//cell.SetParam(BUTTON_CLICK1);
			cell.SetBitmap(&m_noteBitmap);
			SetCell(1, i, &cell);
		}
	}
	SetContents(mode);
}

void SmCorrelationGrid::QuickRedrawCell(int col, long row)
{

}

void SmCorrelationGrid::SetContents(int mode)
{
	if (mode == 0) {
		std::vector<std::string> row_title;
		row_title.push_back("상품포트폴리오3");
		row_title.push_back("");
		row_title.push_back("EUROFX");
		row_title.push_back("T-bill");
		row_title.push_back("Crude Oil");
		row_title.push_back("Gold");
		row_title.push_back("");

		for (int i = 1; i < 8; i++) {
			QuickSetText(2, i, row_title[i - 1].c_str());
			QuickSetText(i * 2, 1, row_title[i - 1].c_str());
			QuickSetFont(i * 2, 1, &_titleFont);
		}

		std::map<std::pair<int, int>, double> value_map;
		value_map[std::make_pair(8, 3)] = 0.0125;
		value_map[std::make_pair(10, 3)] = 0.1407;
		value_map[std::make_pair(12, 3)] = -0.0142;
		value_map[std::make_pair(10, 4)] = 0.2685;
		value_map[std::make_pair(12, 4)] = 0.1077;
		value_map[std::make_pair(12, 5)] = -0.1208;
		value_map[std::make_pair(6, 4)] = 0.0125;
		value_map[std::make_pair(6, 5)] = 0.1407;
		value_map[std::make_pair(8, 5)] = 0.2685;
		value_map[std::make_pair(6, 6)] = -0.0142;
		value_map[std::make_pair(8, 6)] = 0.1077;
		value_map[std::make_pair(10, 6)] = -0.1208;

		for (auto it = value_map.begin(); it != value_map.end(); ++it) {
			std::pair<int, int> key = it->first;
			double item = it->second;
			//QuickSetNumber(key.first, key.second, item);
			CString text;
			text.Format("%.4f", item);
			QuickSetText(key.first, key.second, text);
			if (item < 0) {
				QuickSetTextColor(key.first, key.second, RGB(255, 0, 0));
			}
			else {
				QuickSetTextColor(key.first, key.second, RGB(0, 0, 0));
			}
		}
	}
	else {
		std::vector<std::string> row_title;
		row_title.push_back("신호포트폴리오3");
		row_title.push_back("Sig_Dow");
		row_title.push_back("Sig_Japan");
		row_title.push_back("Sig_note5");
		row_title.push_back("Sig_Crude");
		row_title.push_back("Sig_Gold");
		row_title.push_back("Sig_Soy");

		for (int i = 1; i < 8; i++) {
			QuickSetText(2, i, row_title[i - 1].c_str());
			QuickSetText(i * 2, 1, row_title[i - 1].c_str());
			QuickSetFont(i * 2, 1, &_titleFont);
		}

		std::map<std::pair<int, int>, double> value_map;
		value_map[std::make_pair(6, 2)] = 0.1283;
		value_map[std::make_pair(8, 2)] = -0.1052;
		value_map[std::make_pair(10, 2)] = -0.0096;
		value_map[std::make_pair(12, 2)] = 0.0142;
		value_map[std::make_pair(14, 2)] = 0.0114;

		value_map[std::make_pair(8, 3)] = 0.1685;
		value_map[std::make_pair(10, 3)] = 0.1005;
		value_map[std::make_pair(12, 3)] = 0.1077;
		value_map[std::make_pair(14, 3)] = -0.0083;

		value_map[std::make_pair(10, 4)] = 0.3309;
		value_map[std::make_pair(12, 4)] = 0.0642;
		value_map[std::make_pair(14, 4)] = -0.0778;

		value_map[std::make_pair(12, 5)] = 0.0858;
		value_map[std::make_pair(14, 5)] = -0.2538;

		value_map[std::make_pair(14, 6)] = 0.01;

		value_map[std::make_pair(4, 3)] = 0.1283;

		value_map[std::make_pair(4, 4)] = -0.1052;
		value_map[std::make_pair(6, 4)] = 0.1685;

		value_map[std::make_pair(4, 5)] = -0.0096;
		value_map[std::make_pair(6, 5)] = 0.1005;
		value_map[std::make_pair(8, 5)] = 0.3309;
		
		value_map[std::make_pair(4, 6)] = 0.0142;
		value_map[std::make_pair(6, 6)] = 0.1077;
		value_map[std::make_pair(8, 6)] = 0.0642;
		value_map[std::make_pair(10, 6)] = 0.0858;
		


		value_map[std::make_pair(4, 7)] = 0.0114;
		value_map[std::make_pair(6, 7)] = -0.0083;
		value_map[std::make_pair(8, 7)] = -0.0778;
		value_map[std::make_pair(10, 7)] = -0.2358;
		value_map[std::make_pair(12, 7)] = 0.01;
		

		for (auto it = value_map.begin(); it != value_map.end(); ++it) {
			std::pair<int, int> key = it->first;
			double item = it->second;
			//QuickSetNumber(key.first, key.second, item);
			CString text;
			text.Format("%.4f", item);
			QuickSetText(key.first, key.second, text);
			if (item < 0) {
				QuickSetTextColor(key.first, key.second, RGB(255, 0, 0));
			}
			else {
				QuickSetTextColor(key.first, key.second, RGB(0, 0, 0));
			}
		}
	}
}

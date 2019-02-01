#include "stdafx.h"
#include "VtMainSiseGrid.h"
#include <numeric>
#include "VtGlobal.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "Format/format.h"
#include "resource.h"

VtMainSiseGrid::VtMainSiseGrid()
{
}


VtMainSiseGrid::~VtMainSiseGrid()
{
}

void VtMainSiseGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(175, 25, 192));
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
	SetTH_Height(0);

	
	for (int i = 0; i < _ColCount; i++) {
		for (int j = 0; j < _RowCount; j++) {
			if (i == 0 || i == 3)
				QuickSetAlignment(i, j, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			else
				QuickSetAlignment(i, j, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
		}
	}


	// Add bitmaps
	AddBitmap(IDB_BITMAP_UP_TRI);
	AddBitmap(IDB_BITMAP_DOWN_TRI);

	// Add cell type
	m_nArrowCellIndx = AddCellType(&m_celllArrow);

	SetDefFont(&_defFont);


	SetRowTitle();
}

void VtMainSiseGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtMainSiseGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void VtMainSiseGrid::OnReceiveComplete()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	int i = 0;
	CUGCell cell;
	for (auto it = prdtCatMgr->MainFutureVector.begin(); it != prdtCatMgr->MainFutureVector.end(); ++it) {
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section) {
			if (section->SubSectionVector.size() > 0) {
				VtProductSubSection* subSection = section->SubSectionVector.front();
				if (subSection->_SymbolVector.size() > 0) {
					VtSymbol* sym = subSection->_SymbolVector.front();	
					std::string code = sym->ShortCode.substr(0, 3);
					auto pos = FindPos(code);
					if (std::get<0>(pos) != -1 && std::get<1>(pos) != -1) {
						ShowCurrent(sym, std::get<0>(pos), std::get<1>(pos));
					}
				}
			}
			i++;
		}
	}
}

void VtMainSiseGrid::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;
	std::string code = sym->ShortCode.substr(0, 3);
	auto pos = FindPos(code);
	if (std::get<0>(pos) != -1 && std::get<1>(pos) != -1) {
		ShowCurrent(sym, std::get<0>(pos), std::get<1>(pos));
	}
}

void VtMainSiseGrid::ShowCurrent(VtSymbol* sym, int col, int row)
{
	if (!sym)
		return;
	std::string val = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->IntDecimal), sym->IntDecimal);
	CString thVal = XFormatNumber(val.c_str(), sym->IntDecimal);
	
	QuickSetText(col, row, thVal);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickRedrawCell(col, row);

	val = fmt::format("{:.{}f}", sym->ComToPrev / std::pow(10, sym->IntDecimal), sym->IntDecimal);
	thVal = XFormatNumber(val.c_str(), sym->IntDecimal);
	CUGCell cell;
	GetCell(col + 1, row, &cell);
	if (sym->UpdownRate > 0) {
		cell.SetParam(0);
		cell.SetTextColor(RGB(255, 0, 0));
		QuickSetTextColor(col, row, RGB(255, 0, 0));
	}
	else if (sym->UpdownRate < 0){
		cell.SetParam(1);
		cell.SetTextColor(RGB(0, 0, 255));
		QuickSetTextColor(col, row, RGB(0, 0, 255));
	}
	else {
		cell.SetParam(1);
		cell.SetTextColor(RGB(0, 0, 0));
		QuickSetTextColor(col, row, RGB(0, 0, 0));
	}
	cell.SetText(thVal);
	SetCell(col + 1, row, &cell);
	QuickRedrawCell(col + 1, row);
}

std::pair<int, int> VtMainSiseGrid::FindPos(std::string code)
{
	auto it = _ItemMap.find(code);
	if (it != _ItemMap.end()) {
		return it->second;
	}

	return std::make_pair(-1, -1);
}

void VtMainSiseGrid::SetRowTitle()
{
	int temp = VtGlobal::GetHorWidthByScaleFactor(80);
	_ColWidths.push_back(temp);
	temp = VtGlobal::GetHorWidthByScaleFactor(60);
	_ColWidths.push_back(temp);
	temp = VtGlobal::GetHorWidthByScaleFactor(40);
	_ColWidths.push_back(temp);
	temp = VtGlobal::GetHorWidthByScaleFactor(80);
	_ColWidths.push_back(temp);
	temp = VtGlobal::GetHorWidthByScaleFactor(60);
	_ColWidths.push_back(temp);
	temp = VtGlobal::GetHorWidthByScaleFactor(40);
	_ColWidths.push_back(temp);

	for (int i = 0; i < _ColCount; ++i) {
		SetColWidth(i, _ColWidths[i]);
	}
	QuickSetText(0, 0, _T("ÁÖ°¡Áö¼ö"));
	QuickSetBackColor(0, 0, VtGlobal::MainBackColor);
	QuickSetTextColor(0, 0, VtGlobal::MainTextColor);
	QuickSetText(3, 0, _T("ÄÚ½º´Ú150"));
	QuickSetBackColor(3, 0, VtGlobal::MainBackColor);
	QuickSetTextColor(3, 0, VtGlobal::MainTextColor);
	QuickSetText(0, 1, _T("¿ø´Þ·¯"));
	QuickSetBackColor(0, 1, VtGlobal::MainBackColor);
	QuickSetTextColor(0, 1, VtGlobal::MainTextColor);
	QuickSetText(3, 1, _T("±¹Ã¤"));
	QuickSetBackColor(3, 1, VtGlobal::MainBackColor);
	QuickSetTextColor(3, 1, VtGlobal::MainTextColor);


	_ItemMap[_T("101")] = std::make_pair(1, 0);
	_ItemMap[_T("106")] = std::make_pair(4, 0);
	_ItemMap[_T("175")] = std::make_pair(1, 1);
	_ItemMap[_T("167")] = std::make_pair(4, 1);

	CUGCell cell;
	GetCell(2, 0, &cell);
	cell.SetCellType(m_nArrowCellIndx);
	SetCell(2, 0, &cell);

	GetCell(5, 0, &cell);
	cell.SetCellType(m_nArrowCellIndx);
	SetCell(5, 0, &cell);

	GetCell(2, 1, &cell);
	cell.SetCellType(m_nArrowCellIndx);
	SetCell(2, 1, &cell);

	GetCell(5, 1, &cell);
	cell.SetCellType(m_nArrowCellIndx);
	SetCell(5, 1, &cell);

	CRect rcWnd;
	GetWindowRect(rcWnd);
	int width = VtGlobal::GetHorWidthByScaleFactor(std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0));
	int height = VtGlobal::GetVerHeightByScaleFactor(36);
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE);
}

void VtMainSiseGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

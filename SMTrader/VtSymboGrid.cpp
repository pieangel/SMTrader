#include "stdafx.h"
#include "VtSymboGrid.h"
#include "VtKrClient.h"
#include "VtChartDataRequest.h"
#include "VtChartDataManager.h"
#include "VtChartData.h"
#include "MainFrm.h"
#include "VtRealtimeRegisterManager.h"
#include "VtSymbolManager.h"
#include "SmOrderPanelOut.h"
#include "VtSymbolFavoriteDlg.h"

#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtSymbol.h"
#include "VtGlobal.h"
#include "Market/SmMarketManager.h"
#include "Market/SmMarket.h"
#include "Market/SmProduct.h"

#include <vector>
VtSymboGrid::VtSymboGrid()
{
	_Source = 0;
	_FavoriteDlg = nullptr;
}


VtSymboGrid::~VtSymboGrid()
{

}

void VtSymboGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	_RowCount = 100;

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			//m_data[yIndex].m_colData[xIndex] = rand() % 100;
			CString msg;
			msg.Format(_T("%d, %d value"), yIndex, xIndex);
			//QuickSetText(xIndex, yIndex, msg);
			QuickSetFont(xIndex, yIndex, &_defFont);
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
}

void VtSymboGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (_Source == 0)
	{
		if (col < 3)
			return;

		CUGCell cell;
		GetCell(col, row, &cell);
		//std::string value = QuickGetText(col, row);
		VtSymbol* sym = (VtSymbol*)cell.Tag();

		VtKrClient* client = VtKrClient::GetInstance();
		VtChartDataRequest req;
		req.chartType = VtChartType::MIN;
		req.msgid = "0000000000";
		req.productCode = sym->ShortCode;
		req.cycle = 10;
		req.count = 1500;

		VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
		realtimeManager->RegisterProduct(sym->FullCode);

		CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
		//pMainWnd->OnSymbolClicked(sym->ShortCode);

		VtChartDataManager* dataMgr = VtChartDataManager::GetInstance();
		std::string dataID;
		VtChartData* chartData = dataMgr->Find(dataID);
		if (chartData)
			chartData->Reset();
		client->GetChartData(std::move(req));

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		symMgr->ActiveSymbol(sym);
		symMgr->RequestSymbolBundleInfo(sym->FullCode);
	}
	else if (_Source == 1)
	{
		if (_FavoriteDlg)
		{
			if (col < 3)
				return;

			CUGCell cell;
			GetCell(col, row, &cell);
			VtSymbol* sym = (VtSymbol*)cell.Tag();

			_FavoriteDlg->AddFavorite(sym);
		}
	}
}

void VtSymboGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (col < 3)
		return;

	CUGCell cell;
	GetCell(col, row, &cell);
	//std::string value = QuickGetText(col, row);
	VtSymbol* sym = (VtSymbol*)cell.Tag();

	if (_Source == 1)
	{
		for (int yIndex = 0; yIndex < _DataCount; yIndex++)
		{
			for (int xIndex = 3; xIndex < _ColCount; xIndex++)
			{
				if (xIndex == col && yIndex == row)
				{
					QuickSetBackColor(xIndex, yIndex, RGB(0, 255, 255));
				}
				else
				{
					if (xIndex != 3)
						QuickSetBackColor(xIndex, yIndex, RGB(255, 255, 255));
					else
						QuickSetBackColor(xIndex, yIndex, RGB(255, 255, 0));
					GetCell(xIndex, yIndex, &cell);
					if (_FavoriteDlg && cell.Tag())
					{
						_FavoriteDlg->SrcSymbol((VtSymbol*)cell.Tag());
					}
				}
				QuickRedrawCell(xIndex, yIndex);
			}
		}
	}
}

void VtSymboGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSymboGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[4] = { "거래소", "종목명", "코드", "거래월물" };
	int colWidth[4] = { 50, 140, 65, 80};

	for (int i = 0; i < 3; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
	for (int i = 3; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[3]);
		QuickSetText(i, -1, title[3]);
		QuickSetFont(i, -1, &_titleFont);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		RedrawCell(i, -1);
	}
}
BEGIN_MESSAGE_MAP(VtSymboGrid, VtGrid)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


void VtSymboGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	VtGrid::OnVScroll(nSBCode, nPos, pScrollBar);
}

void VtSymboGrid::SetProduct()
{
	CUGCell cell;
	std::vector<SmMarket*> market_list = SmMarketManager::GetInstance()->GetAbroadMarketList();
	if (market_list.size() == 0)
		return;
	SmMarket* market = market_list[_Section];
	std::vector<SmProduct*> product_list = market->GetProductList();
	int i = 0;
	for (auto it = product_list.begin(); it != product_list.end(); ++it) {
		SmProduct* sec = *it;
		std::vector<VtSymbol*>& symbol_list = sec->GetSymbolList();
		if (symbol_list.size() > 0) {
			QuickSetText(0, i, sec->Exchange().c_str());
			QuickSetBackColor(0, i, RGB(240, 240, 240));
			QuickSetText(1, i, sec->NameKr().c_str());
			QuickSetBackColor(1, i, RGB(240, 240, 240));
			QuickSetText(2, i, sec->Code().c_str());
			QuickSetBackColor(2, i, RGB(240, 240, 240));
			QuickSetBorderColor(2, i, RGB(0, 0, 0));
			int j = 3;
			for (auto itcol = symbol_list.begin(); itcol != symbol_list.end(); ++itcol) {
				VtSymbol* sym = *itcol;
				std::string short_name = sym->Name.substr(sym->Name.length() - 7, 7);
				QuickSetText(j, i, short_name.c_str());
				GetCell(j, i, &cell);
				cell.Tag((void*)sym);
				//cell.UserMessage(sym->ShortCode.c_str());
				if (j == 3)
					cell.SetBackColor(RGB(255, 255, 0));
				SetCell(j, i, &cell);
				j++;
			}
			i++;
		}
	}
	
	_DataCount = i;
}

void VtSymboGrid::SetSource(int src)
{
	_Source = src;
}

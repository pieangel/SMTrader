#include "stdafx.h"
#include "VtTotalRemainGrid.h"
#include "VtOrderConfigManager.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "VtFund.h"
#include "VtGlobal.h"
#include <algorithm> 
#include <functional>
#include "SmCallbackManager.h"

using namespace std;
using namespace std::placeholders;

using Poco::NumberFormatter;

VtTotalRemainGrid::VtTotalRemainGrid()
{
	RegisterQuoteCallback();
}


VtTotalRemainGrid::~VtTotalRemainGrid()
{
	UnregisterAllCallback();
}


void VtTotalRemainGrid::UnregisterAllCallback()
{
	UnregisterQuoteCallback();
}

void VtTotalRemainGrid::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&VtTotalRemainGrid::OnQuoteEvent, this, _1));
}

void VtTotalRemainGrid::UnregisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void VtTotalRemainGrid::OnQuoteEvent(VtSymbol* symbol)
{
	if (!symbol)
		return;
	InitGrid();
}


void VtTotalRemainGrid::Init()
{
	_CellHeight = 20;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 0; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < _ColCount; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				if (j == 0)
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				else // �ؽ�Ʈ ����
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// �� ��� �� ����
				pCell->SetBackClr(RGB(255, 255, 255));
				// �� ���ڻ� ����
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);

	SetRowTitle();
}

void VtTotalRemainGrid::SetRowTitle()
{
	LPCTSTR title[3] = { "�򰡼���", "��������", "�Ѽ���" };
	SetColumnWidth(0, 60);
	SetColumnWidth(1, 97);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetText(i, 0, title[i]);
		QuickSetBackColor(i, 0, VtGlobal::GridTitleBackColor);
		QuickSetTextColor(i, 0, VtGlobal::GridTitleTextColor);
		InvalidateCellRect(i, 0);
	}
}


void VtTotalRemainGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void VtTotalRemainGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void VtTotalRemainGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void VtTotalRemainGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}


void VtTotalRemainGrid::InitGrid()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
		ShowAccountProfitLoss();
	else
		ShowFundProfitLoss();
}

void VtTotalRemainGrid::OnOutstanding()
{
	InitGrid();
}

void VtTotalRemainGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	InitGrid();
}

void VtTotalRemainGrid::OnReceiveAccountInfo()
{
	InitGrid();
}

void VtTotalRemainGrid::ShowAccountProfitLoss()
{
	VtAccount* acnt = _OrderConfigMgr->Account();

	if (!acnt)
		return;

	// ���� ������Ʈ�� Ȱ��ȭ ��Ų��.
	// ������ �򰡼����� ������Ʈ �ȴ�.
	acnt->Updating(true);
	std::string temp = NumberFormatter::format(acnt->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), 2);

	if (acnt->OpenPL > 0) {
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, profitLoss);
	}
	else if (acnt->OpenPL < 0) {
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, profitLoss);
	}
	else {
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	temp = NumberFormatter::format(acnt->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), 2);

	if (acnt->TradePL > 0) {
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, profitLoss);
	}
	else if (acnt->TradePL < 0) {
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, profitLoss);
	}
	else {
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}

	double total_pl = acnt->OpenPL + acnt->TradePL + acnt->Fee;
	temp = NumberFormatter::format(total_pl, 0);
	profitLoss = XFormatNumber(temp.c_str(), 2);

	if (total_pl > 0) {
		QuickSetTextColor(2, 1, RGB(255, 0, 0));
		QuickSetText(2, 1, profitLoss);
	}
	else if (total_pl < 0) {
		QuickSetTextColor(2, 1, RGB(0, 0, 255));
		QuickSetText(2, 1, profitLoss);
	}
	else {
		QuickSetTextColor(2, 1, RGB(0, 0, 0));
		QuickSetNumber(2, 1, 0);
	}
	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
	InvalidateCellRect(2, 1);
}

void VtTotalRemainGrid::ShowFundProfitLoss()
{
	VtFund* fund = _OrderConfigMgr->Fund();
	if (!fund)
		return;

	std::string temp = NumberFormatter::format(fund->OpenPL, 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->OpenPL > 0)
	{
		QuickSetTextColor(0, 1, RGB(255, 0, 0));
		QuickSetText(0, 1, profitLoss);
	}
	else if (fund->OpenPL < 0)
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 255));
		QuickSetText(0, 1, profitLoss);
	}
	else
	{
		QuickSetTextColor(0, 1, RGB(0, 0, 0));
		QuickSetNumber(0, 1, 0);
	}

	temp = NumberFormatter::format(fund->TradePL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->TradePL > 0)
	{
		QuickSetTextColor(1, 1, RGB(255, 0, 0));
		QuickSetText(1, 1, profitLoss);
	}
	else if (fund->TradePL < 0)
	{
		QuickSetTextColor(1, 1, RGB(0, 0, 255));
		QuickSetText(1, 1, profitLoss);
	}
	else
	{
		QuickSetTextColor(1, 1, RGB(0, 0, 0));
		QuickSetNumber(1, 1, 0);
	}

	temp = NumberFormatter::format(fund->TotalPL, 0);
	profitLoss = XFormatNumber(temp.c_str(), -1);

	if (fund->TotalPL > 0)
	{
		QuickSetTextColor(2, 1, RGB(255, 0, 0));
		QuickSetText(2, 1, profitLoss);
	}
	else if (fund->TotalPL < 0)
	{
		QuickSetTextColor(2, 1, RGB(0, 0, 255));
		QuickSetText(2, 1, profitLoss);
	}
	else
	{
		QuickSetTextColor(2, 1, RGB(0, 0, 0));
		QuickSetNumber(2, 1, 0);
	}
	InvalidateCellRect(0, 1);
	InvalidateCellRect(1, 1);
	InvalidateCellRect(2, 1);
}

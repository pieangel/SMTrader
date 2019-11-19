#pragma once
#include "Grid/GridCtrl.h"
class VtOrderConfigManager;
class SmAssetGrid : public CGridCtrl
{
public:
	SmAssetGrid();
	~SmAssetGrid();

	void Init();

	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 2;
	CFont _defFont;
	CFont _titleFont;
	int _CellHeight;
	void InitGrid();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void OnReceiveAccountInfo();
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
};


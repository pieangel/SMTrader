#pragma once
#include "Skel/VtGrid.h"
class VtOrderConfigManager;
class VtSymbol;
class HdProfitLossGrid : public VtGrid
{
public:
	HdProfitLossGrid();
	~HdProfitLossGrid(); 

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 1;
	int _RowCount = 3;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }

	void InitGrid();
	void OnOutstanding();
	void OnReceiveQuote(VtSymbol* sym);
	void OnReceiveAccountInfo();
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void ShowAccountProfitLoss();
	void ShowFundProfitLoss();
};


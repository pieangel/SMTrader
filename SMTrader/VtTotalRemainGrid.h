#pragma once
#include "Skel/VtGrid.h"
class VtOrderConfigManager;
class VtAccount;
class VtTotalRemainGrid : public VtGrid
{
public:
	VtTotalRemainGrid();
	~VtTotalRemainGrid();

	virtual void OnSetup();
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 3;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);

	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnReceiveAccountDeposit(VtAccount* acnt);
	void ShowAccountInfo(VtAccount* acnt);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};


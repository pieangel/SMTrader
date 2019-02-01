#pragma once
#include "Skel/VtGrid.h"
class VtOrderConfigManager;
class VtAccount;
class VtAccountRemainGrid : public VtGrid
{
public:
	VtAccountRemainGrid();
	~VtAccountRemainGrid();

	virtual void OnSetup();
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 6;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);

	void ShowAccountInfo(VtAccount* acnt);

	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnReceiveAccountDeposit(VtAccount* acnt);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};


#pragma once
#include "Skel/VtGrid.h"
class VtOrderConfigManager;
class HdAssetGrid : public VtGrid
{
public:
	HdAssetGrid();
	~HdAssetGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 1;
	int _RowCount = 2;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void OnReceiveAccountInfo();
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

};


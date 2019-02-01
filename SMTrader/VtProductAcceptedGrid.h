
#pragma once
#include "Skel/VtGrid.h"
#include <list>
class VtOrderConfigManager;
class CVtOrderLeftWnd;
class VtProductAcceptedGrid : public VtGrid
{
public:
	VtProductAcceptedGrid();
	~VtProductAcceptedGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	//cell type notifications
	virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;

	CBitmap				m_CancelBitmap;

	void SetAcptOrderList();
	void QuickRedrawCell(int col, long row);
	void ResetCellText();
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	CVtOrderLeftWnd* OrderLeftWnd() const { return _OrderLeftWnd; }
	void OrderLeftWnd(CVtOrderLeftWnd* val) { _OrderLeftWnd = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	CVtOrderLeftWnd* _OrderLeftWnd = nullptr;
	void CancelOrder(VtOrder* order);
	std::list<VtOrder*> _SelectedOrderList;
	void CancelOrderList();
public:
	void CancelOrderAll();
	void CancelOrderSelected();
};




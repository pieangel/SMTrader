#pragma once
#include "Skel/VtGrid.h"
class HdAccountPLDlg;
class VtAccount;
class VtSymbol;
struct VtOrder;
class HdAccountPLGrid : public VtGrid
{
public:
	HdAccountPLGrid();
	virtual ~HdAccountPLGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 1;
	int _RowCount = 4;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();

	HdAccountPLDlg* AcntPLDlg() const { return _AcntPLDlg; }
	void AcntPLDlg(HdAccountPLDlg* val) { _AcntPLDlg = val; }
	void OnReceiveQuote(VtSymbol* sym);
	void OnOrderFilledHd(VtOrder* order);
	void ClearValues();
private:
	HdAccountPLDlg* _AcntPLDlg = nullptr;
};


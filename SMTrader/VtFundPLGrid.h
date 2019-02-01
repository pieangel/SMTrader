
#pragma once
#include "Skel/VtGrid.h"
class VtFundMiniJango;
class VtAccount;
class VtSymbol;
struct VtOrder;
class HdAccountPLDlg;
class VtFundPLGrid : public VtGrid
{
public:
	VtFundPLGrid();
	virtual ~VtFundPLGrid();

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
	VtFundMiniJango* FundPLDlg() const { return _FundPLDlg; }
	void FundPLDlg(VtFundMiniJango* val) { _FundPLDlg = val; }
private:
	HdAccountPLDlg* _AcntPLDlg = nullptr;
	VtFundMiniJango* _FundPLDlg = nullptr;
};


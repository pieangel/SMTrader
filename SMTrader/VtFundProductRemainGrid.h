#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include <map>
#include <vector>
class HdAccountPLDlg;
struct VtPosition;
class VtAccount;
class VtSymbol;
struct VtOrder;
class VtFundMiniJango;
class VtFundProductRemainGrid : public VtGrid
{
public:
	VtFundProductRemainGrid();
	virtual ~VtFundProductRemainGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle(int mode);
	int _ColCount = 4;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	HdAccountPLDlg* AcntPLDlg() const { return _AcntPLDlg; }
	void AcntPLDlg(HdAccountPLDlg* val) { _AcntPLDlg = val; }
	void OnReceiveQuote(VtSymbol* sym);
	void OnOrderFilledHd(VtOrder* order);
	void ClearValues();
	VtFundMiniJango* FundPLDlg() const { return _FundPLDlg; }
	void FundPLDlg(VtFundMiniJango* val) { _FundPLDlg = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void ClearCells();
private:
	HdAccountPLDlg* _AcntPLDlg = nullptr;
	VtFundMiniJango* _FundPLDlg = nullptr;
	void ShowPosition(VtPosition* posi, int index);
	int _Mode = 0;
	std::map<VtCellPos, VtCellPos> _OldPosMap;
	void ClearOldValuse(std::map<VtCellPos, VtCellPos>& posMap);
	void RefreshCells(std::map<VtCellPos, VtCellPos>& posMap);
	std::vector<int> _ColWidthVector;
};



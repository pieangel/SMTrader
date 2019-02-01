#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <string>
#include <vector>
#include <set>
class VtAccount;
class VtFundAccount;
class VtFundEditor;
class VtFDMultiAcntPage;
class VtFundGrid : public VtGrid
{
public:
	VtFundGrid();
	virtual ~VtFundGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);
	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 50;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);
	void InitGrid(std::map<std::string, VtAccount*>& fundAcntMap);
	void InitGrid(std::vector<VtAccount*>& fundAcntVector);
	VtFundEditor* FundEditor() const { return _FundEditor; }
	void FundEditor(VtFundEditor* val) { _FundEditor = val; }
	void ClearCells();
	void ChangeSelectedRow(int oldRow, int newRow);
	VtFDMultiAcntPage* FundPage() const { return _FundPage; }
	void FundPage(VtFDMultiAcntPage* val) { _FundPage = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	std::set<std::pair<int, int>> _RefreshArea;
	int _SelRow;
	VtFundEditor* _FundEditor;
	VtFDMultiAcntPage* _FundPage;
	int _Mode; // Big or small
};


#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include "UGrid/UGEdit.h"
#include <string>
#include <map>
class VtAccount;
class VtAccountPasswordGrid : public VtGrid
{
public:
	VtAccountPasswordGrid();
	~VtAccountPasswordGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);


	//SetCell notification
	virtual void OnSetCell(int col, long row, CUGCell *cell);

	//cell type notifications
	virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);

	//editing
	virtual int OnEditStart(int col, long row, CWnd **edit);
	virtual int OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey);
	virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag);
	virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow);

	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	// additional edit controls
	CUGEdit m_myCUGEdit;
	void InitGrid();
	void InitGrid(std::map<std::string, VtAccount*>& acntMap);
	void ShowPassword(bool show);
	
	bool FromServer() const { return _FromServer; }
	void FromServer(bool val) { _FromServer = val; }
	void SavePassword();
private:
	std::map<std::pair<int, int>, std::pair<VtAccount*, std::string>> _CellAcntMap;
	bool _FromServer = false;
	bool _ShowPassword;
	int _EdittingRow;
};


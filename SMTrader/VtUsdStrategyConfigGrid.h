#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <vector>
#include "UGrid/CellTypes/UGCTbutn.h"
#include "UGrid/UGEdit.h"
#include "VtSystemArgGroup.h"

const int CONFIG_EDIT = 0x0001;
class VtUsdStrategyConfigGrid : public VtGrid
{
public:
	VtUsdStrategyConfigGrid();
	~VtUsdStrategyConfigGrid();

	virtual void OnSetup();
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnPushButton(long ID, int col, long row, long msg, long param);

	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);


	//SetCell notification
	virtual void OnSetCell(int col, long row, CUGCell *cell);

	//editing
	virtual int OnEditStart(int col, long row, CWnd **edit);
	virtual int OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey);
	virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag);
	virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow);

	void SetRowTitle();
	int _ColCount = 10;
	int _RowCount = 4;
	int _HeadHeight = 18;
	CFont _defFont;
	CPen m_Pen;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;
	CBitmap				_SetBitmap;
	void QuickRedrawCell(int col, long row);
	// additional edit controls
	CUGEdit m_myCUGEdit;

	int Type() const { return _Type; }
	void Type(int val) { _Type = val; }

	void SetArg(VtSystemArgGroup& argGrp);
	void RefreshRealTimeValue(std::string argName, CString value);
private:
	std::map<std::string, std::pair<int, int>> _ArgMap;
	std::vector<int> _ColWidthVec;
	// 0 : 진입1, 1 : 청산1, 2 : 진입2, 3 : 청산2
	int _Type = 0;
	void ResizeWindow();
};


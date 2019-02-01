#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <vector>
#include "UGrid/CellTypes/UGCTbutn.h"
#include "UGrid/UGEdit.h"
#include "VtSystemArgGroup.h"
const int ARG_EDIT = 0x0002;
class VtSysArgGrid : public VtGrid
{
public:
	VtSysArgGrid();
	~VtSysArgGrid();

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
	int _ColCount = 6;
	int _RowCount = 10;
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

	void SetArg(VtSystemArgGroup& argGrp);
private:
	std::vector<int> _ColWidthVec;
	void ResizeWindow();
};


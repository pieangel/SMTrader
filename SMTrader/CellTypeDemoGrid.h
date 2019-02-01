/***********************************************
	Ultimate Grid 97
// This software along with its related components, documentation and files ("The Libraries")
// is ?1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.


	class 
		CUGCCellTypeDemoGrid
	Purpose
		General purpose derived grid class.
		This class can be used as a starting 
		point for any grid project.
************************************************/
#pragma once

#include "UGrid/ugctrl.h"

//celltypes
#include "UGrid/CellTypes/ugctprog.h"
#include "UGrid/CellTypes/ugctspin.h"
#include "UGrid/CellTypes/ugctbutn.h"
#include "UGrid/CellTypes/ugctsarw.h"
#include "UGrid/CellTypes/ugctelps.h"
#include "UGrid/CellTypes/ugctrado.h"
#include "UGrid/CellTypes/ugctmfnt.h"
#include "UGrid/CellTypes/ugctpie.h"
#include "UGrid/CellTypes/ugctafnt.h"
#include "UGrid/CellTypes/ugctexpand.h"
#include "UGrid/CellTypes/ugctpro1.h"
#include "UGrid/CellTypes/ugctsldr.h"
#include "UGrid/CellTypes/ugctdtp.h"
#include "UGrid/CellTypes/ugctmarquee.h"
#include "UGrid/CellTypes/UGCTNote.h"
#include "UGrid/CellTypes/UGCTLabeled.h"

#define STANDARD_ROW	1
#define DROPLIST_ROW	STANDARD_ROW + 8
#define CHECKBOX_ROW	DROPLIST_ROW + 7
#define RADIOBTN_ROW	CHECKBOX_ROW + 10
#define DATETIME_ROW	RADIOBTN_ROW + 5
#define PUSHBTN_ROW		DATETIME_ROW + 5
#define ELLIPSIS_ROW	PUSHBTN_ROW + 6
#define SPINBTN_ROW		ELLIPSIS_ROW + 7
#define SLIDERS_ROW		SPINBTN_ROW + 5
#define ARROWS_ROW		SLIDERS_ROW + 5
#define SORTARROW_ROW	ARROWS_ROW + 4
#define PROGRESS_ROW	SORTARROW_ROW + 4
#define MULTIFONT_ROW	PROGRESS_ROW + 8
#define AUTOFONT_ROW	MULTIFONT_ROW + 7
#define NOTE_ROW		AUTOFONT_ROW + 6
#define LABELED_ROW		NOTE_ROW + 6
#define MARQUEE_ROW		LABELED_ROW + 5
#define PIECHART_ROW	MARQUEE_ROW + 5

#define NUM_ROWS		PIECHART_ROW + 7
#define NUM_COLS		10


//cell 'param' values, used as a style field
#define DROPLIST_DYNAMIC_LOAD		110
#define DROPLIST_DYNAMIC_LOAD2		111
#define DROPLIST_DYNAMIC_LOAD3		112
#define DROPLIST_CHANGE_SELECTION	113
#define DROPLIST_CHANGE_SELECTION2	114

#define CELLTYPE_IS_EDITABLE		120

#define CHECKBOX_CHANGEFONT			130

#define BUTTON_CLICK1				140
#define BUTTON_CLICK2				141
#define BUTTON_CLICK3				142

#define ELLIPSISBUTTON_CLICK1		150
#define ELLIPSISBUTTON_CLICK2		151

#define SPIN_TYPE1					160
#define SPIN_TYPE2					161
#define SPIN_TYPE3					162
#define SPIN_TYPE4					163
#define SPIN_TYPE5					164

#define SLIDER_TYPE1				170
#define SLIDER_TYPE2				171
#define SLIDER_TYPE3				172
#define SLIDER_TYPE4				173

//Class decaration
class CCellTypeDemoGrid:public CUGCtrl
{

	//Standard celltype, built-in
	//UGCT_NORMAL

	//Droplist celltype, built-in
	//UGCT_DROPLIST

	//Checkbox celltype, built-in
	//UGCT_CHECKBOX

	//Radio button cell type
	CUGRadioType		m_radio;
	int					m_nRadioIndex;

	//Date Time Picker cell type
	CUGCTDateTimePicker	m_dateTime;
	int					m_nDateTimeIndex;

	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	//Ellipsis Button cell type
	CUGEllipsisType		m_ellipsis;
	int					m_nEllipsisIndex;

	//Spin Button cell type
	CUGSpinButtonType	m_spin;
	int					m_nSpinIndex;

	//Slider cell type
	CUGSliderType		m_slider;
	int					m_nSliderIndex;

	//Arrow cell type built-in
	//UGCT_ARROW

	//Sort Arrow cell type
	CUGSortArrowType	m_sortArrow;
	int					m_nSortArrowIndex;

	//Progress bar cell types
	CUGProgressType		m_progress;
	int					m_nProgressIndex;
	CUGAdvProgressType	m_advProress1;
	int					m_nAdvProressIndex1;
	CUGAdvProgressType	m_advProress2;
	int					m_nAdvProressIndex2;
	CUGAdvProgressType	m_advProress3;
	int					m_nAdvProressIndex3;

	//Multi-font cell type
	CUGMultiFontType	m_multiFont;
	int					m_nMultiFontIndex;

	//Auto-font cell type
	CUGAutoFontType		m_autoFont;
	int					m_nAutoFontIndex;

	//Note cell type
	CUGNoteType			m_note;
	int					m_nNoteIndex;
	
	//Labeled cell type
	CUGLabeledType		m_labeled;
	int					m_nLabeledIndex;
	
	//Marquee cell type
	CUGMarqueeType		m_marquee;
	int					m_nMarqueeIndex;
	
	//Pie Chart cell type
	CUGPieChartType		m_pieChart;
	int					m_nPieChartIndex;


	//fonts
	CFont				m_defFont;
	CFont				m_boldFont;
	CFont				m_medFont;
	CFont				m_medBoldFont;
	CFont				m_lrgFont;
	CFont				m_smFont;

	//bitmaps
	CBitmap				m_noteBitmap;
	CBitmap				m_folderBitmap;


	void SetupSection(long row, LPCSTR title);


	//celltype notification handlers
	int OnDropList(long ID,int col,long row,long msg,long param);
	int OnCheckbox(long ID,int col,long row,long msg,long param);
	int OnDateTime(long ID,int col,long row,long msg,long param);
	int OnPushButton(long ID,int col,long row,long msg,long param);
	int OnEllipsisButton(long ID,int col,long row,long msg,long param);
	int OnSpinButton(long ID,int col,long row,long msg,long param);
	int OnSlider(long ID,int col,long row,long msg,long param);
	
public:

	CCellTypeDemoGrid();
	~CCellTypeDemoGrid();

	void Reset();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCellTypeDemoGrid)
	//}}AFX_VIRTUAL

	
	//{{AFX_MSG(CCellTypeDemoGrid)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	virtual void OnReset();
	virtual void OnGetCell(int col,long row,CUGCell *cell);

	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);
	virtual int OnEditFinish(int col, long row,CWnd* edit,LPCTSTR string,BOOL cancelFlag);

	//cell type notifications
	int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

	//mouse and key strokes
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);

	virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
	virtual void OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed=0);
	virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnTH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnTH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnSH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnSH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnSH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnCB_LClicked(int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnCB_RClicked(int updn,RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnCB_DClicked(RECT *rect,POINT *point,BOOL processed=0);
	virtual void OnMenuCommand(int col,long row,int section,int item);
};

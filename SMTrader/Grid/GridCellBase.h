/////////////////////////////////////////////////////////////////////////////
// GridCellBase.h : header file
//
// MFC Grid Control - Grid cell base class header file
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <map>
class CGridCtrl;

// Cell states
#define GVIS_FOCUSED            0x0001
#define GVIS_SELECTED           0x0002
#define GVIS_DROPHILITED        0x0004
#define GVIS_READONLY           0x0008
#define GVIS_FIXED              0x0010
#define GVIS_FIXEDROW           0x0020
#define GVIS_FIXEDCOL           0x0040
#define GVIS_MODIFIED           0x0080

// Cell data mask
#define GVIF_TEXT               LVIF_TEXT
#define GVIF_IMAGE              LVIF_IMAGE
#define GVIF_PARAM              LVIF_PARAM
#define GVIF_STATE              LVIF_STATE
#define GVIF_BKCLR              (GVIF_STATE<<1)
#define GVIF_FGCLR              (GVIF_STATE<<2)
#define GVIF_FORMAT             (GVIF_STATE<<3)
#define GVIF_FONT               (GVIF_STATE<<4)
#define GVIF_MARGIN             (GVIF_STATE<<5)
#define GVIF_ALL                (GVIF_TEXT|GVIF_IMAGE|GVIF_PARAM|GVIF_STATE|GVIF_BKCLR|GVIF_FGCLR| \
                                 GVIF_FORMAT|GVIF_FONT|GVIF_MARGIN)


// Used for Get/SetItem calls.
typedef struct _GV_ITEM {
    int      row,col;     // Row and Column of item
    UINT     mask;        // Mask for use in getting/setting cell data
    UINT     nState;      // cell state (focus/hilighted etc)
    DWORD    nFormat;     // Format of cell
    int      iImage;      // index of the list view item�s icon
    COLORREF crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM   lParam;      // 32-bit value to associate with item
    LOGFONT  lfFont;      // Cell font
    UINT     nMargin;     // Internal cell margin
    CString  strText;     // Text in cell
} GV_ITEM;


// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.
struct VtOrder;
struct HdOrderRequest;
class CGridCellBase : public CObject
{
    friend class CGridCtrl;
    DECLARE_DYNAMIC(CGridCellBase)

// Construction/Destruction
public:
    CGridCellBase();
    virtual ~CGridCellBase();

	int Style() const { return _Style; }
	void Style(int val) { _Style = val; }
	int MergeStartRow() const { return _MergeStartRow; }
	void MergeStartRow(int val) { _MergeStartRow = val; }
	int MergeEndRow() const { return _MergeEndRow; }
	void MergeEndRow(int val) { _MergeEndRow = val; }
	int MergeStartCol() const { return _MergeStartCol; }
	void MergeStartCol(int val) { _MergeStartCol = val; }
	int MergeEndCol() const { return _MergeEndCol; }
	void MergeEndCol(int val) { _MergeEndCol = val; }
	// Attributes
public:
    virtual void SetText(LPCTSTR /* szText */)              = 0 ;
    virtual void SetImage(int /* nImage */)                 = 0 ;
    virtual void SetData(LPARAM /* lParam */)               = 0 ;
    virtual void SetState(DWORD nState)                     { m_nState = nState; }
    virtual void SetFormat(DWORD /* nFormat */)             = 0 ;
    virtual void SetTextClr(COLORREF /* clr */)             = 0 ;
    virtual void SetBackClr(COLORREF /* clr */)             = 0 ;
    virtual void SetFont(const LOGFONT* /* plf */)          = 0 ;
    virtual void SetMargin( UINT /* nMargin */)             = 0 ;
    virtual void SetGrid(CGridCtrl* /* pGrid */)            = 0 ;
    virtual void SetCoords( int /* nRow */, int /* nCol */) = 0 ;
	virtual void SetLongValue(long ) = 0;
	virtual long GetLongValue() const = 0;

	virtual void SetMoving(bool) = 0;
	virtual bool GetMoving() const = 0;
	virtual void SetClicked(bool) = 0;
	virtual bool GetClicked() const = 0;

	virtual void SetPosition(int position) = 0;
	virtual int GetPosition() const = 0;

	virtual void SetLabel(LPCTSTR /* szText */) = 0;
	virtual LPCTSTR    GetLabel()       const = 0;

	virtual int GetMerged() const = 0;
	virtual void SetMerged(int flag) = 0;

    virtual LPCTSTR    GetText()       const                = 0 ;
    virtual LPCTSTR    GetTipText()    const                { return GetText(); } // may override TitleTip return
    virtual int        GetImage()      const                = 0 ;
    virtual LPARAM     GetData()       const                = 0 ;
    virtual DWORD      GetState()      const                { return m_nState;  }
    virtual DWORD      GetFormat()     const                = 0 ;
    virtual COLORREF   GetTextClr()    const                = 0 ;
    virtual COLORREF   GetBackClr()    const                = 0 ;
    virtual LOGFONT  * GetFont()       const                = 0 ;
    virtual CFont    * GetFontObject() const                = 0 ;
    virtual CGridCtrl* GetGrid()       const                = 0 ;
    virtual CWnd     * GetEditWnd()    const                = 0 ;
    virtual UINT       GetMargin()     const                = 0 ;

    virtual CGridCellBase* GetDefaultCell() const;

    virtual BOOL IsDefaultFont()       const                = 0 ;
    virtual BOOL IsEditing()           const                = 0 ;
    virtual BOOL IsFocused()           const                { return (m_nState & GVIS_FOCUSED);  }
    virtual BOOL IsFixed()             const                { return (m_nState & GVIS_FIXED);    }
    virtual BOOL IsFixedCol()          const                { return (m_nState & GVIS_FIXEDCOL); }
    virtual BOOL IsFixedRow()          const                { return (m_nState & GVIS_FIXEDROW); }
    virtual BOOL IsSelected()          const                { return (m_nState & GVIS_SELECTED); }
    virtual BOOL IsReadOnly()          const                { return (m_nState & GVIS_READONLY); }
    virtual BOOL IsModified()          const                { return (m_nState & GVIS_MODIFIED); }
    virtual BOOL IsDropHighlighted()   const                { return (m_nState & GVIS_DROPHILITED); }

// Operators
public:
    virtual void operator=(const CGridCellBase& cell);

// Operations
public:
    virtual void Reset();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL GetTextRect( LPRECT pRect);    // i/o:  i=dims of cell rect; o=dims of text rect
    virtual BOOL GetTipTextRect( LPRECT pRect) { return GetTextRect( pRect); }  // may override for btns, etc.
    virtual CSize GetTextExtent(LPCTSTR str, CDC* pDC = NULL);
    virtual CSize GetCellExtent(CDC* pDC);

    // Editing
    virtual BOOL Edit( int /* nRow */, int /* nCol */, CRect /* rect */, CPoint /* point */, 
                       UINT /* nID */, UINT /* nChar */) { ASSERT( FALSE); return FALSE;}
	virtual BOOL ValidateEdit(LPCTSTR str);
    virtual void EndEdit() {}

    // EFW - Added to print cells properly
    virtual BOOL PrintCell(CDC* pDC, int nRow, int nCol, CRect rect);

    // add additional protected grid members required of cells
    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

protected:
    virtual void OnEndEdit();
    virtual void OnMouseEnter();
    virtual void OnMouseOver();
    virtual void OnMouseLeave();
    virtual void OnClick( CPoint PointCellRelative);
    virtual void OnClickDown( CPoint PointCellRelative);
    virtual void OnRClick( CPoint PointCellRelative);
    virtual void OnDblClick( CPoint PointCellRelative);
    virtual BOOL OnSetCursor();

public:
	void AddOrder(VtOrder* order);
	void RemoveOrder(int orderNo);
	void ClearOrders();
	int GetOrderCount();

	void AddStopOrder(HdOrderRequest* order);
	void RemoveStopOrder(int reqNo);
	void ClearStopOrders();
	int GetStopOrderCount();
	std::map<int, VtOrder*>& GetOrderMap() {
		return OrderMap;
	}
	std::map<int, HdOrderRequest*>& GetStopOrderMap() {
		return StopOrderMap;
	}
	
protected:
    DWORD    m_nState;      // Cell state (selected/focus etc)
	std::map<int, VtOrder*> OrderMap;
	std::map<int, HdOrderRequest*> StopOrderMap;
	void DrawOrder(CDC* pDC, CRect& rect);
	void DrawMovingRect(CDC* pDC, CRect& rect);
	void DrawSelectedgRect(CDC* pDC, CRect& rect);
	// 0 : normal, 1 : close line, 2 : button, 3 : moving rect, 4 : selected rect.
	int _Style = 0;
	int _MergeStartRow;
	int _MergeEndRow;
	int _MergeStartCol;
	int _MergeEndCol;
	bool _MovingRect = false;
	bool _Clicked = false;
	// ������ ���� 0 : None, 1 : buy, 2 : sell.
	int _Position = 0;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)

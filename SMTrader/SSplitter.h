//------------------------------------------------------------------------------
// File    : SSplitter.h 
// Version : 1.1
// Date    : 20. January 2004
// Author  : Alexander Atamas
// Email   : atamas@mail.univ.kiev.ua
// Web     : 
// Systems : VC6.0 (Run under Window 98, Windows Nt)
// Remarks : based on Paul DiLascia's WinMgr code
//

// 
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of your 
// applications (Freeware, Shareware, Commercial). 
// All I ask is that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also wouldn't 
// mind if you sent me a copy of your application since I like to play with
// new stuff.
//------------------------------------------------------------------------------

#if !defined(AFX_SSPLITTER_H__BBFAEC99_7CAD_11D8_B153_CEBADFA6E04F__INCLUDED_)
#define AFX_SSPLITTER_H__BBFAEC99_7CAD_11D8_B153_CEBADFA6E04F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SSplitter.h : header file
//

#include "Global/VtDefine.h"

// Splitter Styles
#define SS_VERT			0x00000001	// vertical splitter
#define SS_HORIZ  		0x00000002	// horizontal splitter


/////////////////////////////////////////////////////////////////////////////
// CSSplitter window
enum VtSplitType
{
	SplitNone,
	SplitVertical,
	SplitHorizontal
};



class CSSplitter : public CStatic
{
// Construction
public:
	CSSplitter();

	CSSplitter* ParentNode() const { return _ParentNode; }
	void ParentNode(CSSplitter* val) { _ParentNode = val; }
	//int Direction() const { return _Direction; }
	//void Direction(int val) { _Direction = val; }
	int WndType() const { return _WndType; }
	void WndType(int val) { _WndType = val; }
	CWnd* WndParent() const { return m_pWndParent; }
	void WndParent(CWnd* val) { m_pWndParent = val; }
	CWnd* WndLeft() const { return m_pWndLeft; }
	void WndLeft(CWnd* val) { m_pWndLeft = val; }
	CWnd* WndRight() const { return m_pWndRight; }
	void WndRight(CWnd* val) { m_pWndRight = val; }
	CWnd* WndTop() const { return m_pWndTop; }
	void WndTop(CWnd* val) { m_pWndTop = val; }
	CWnd* WndBottom() const { return m_pWndBottom; }
	void WndBottom(CWnd* val) { m_pWndBottom = val; }
	// Attributes
	VtSplitType SplitType;
	CSSplitter* Sibling = nullptr;
	CSSplitter* ParentSplit = nullptr;
	VtAttachDirection AttachDirection() const { return _AttachDirection; }
	void AttachDirection(VtAttachDirection val) { _AttachDirection = val; }
	void HideSibling();
private:

	CWnd*	m_pWndParent;
	CWnd*	m_pWndLeft;
	CWnd*	m_pWndRight;
	CWnd*	m_pWndTop;
	CWnd*	m_pWndBottom;

	int		m_nMaxLeft, m_nMaxRight;
	int		m_nMaxTop, m_nMaxBottom;
	UINT	m_nSplitterWidth;
	UINT	m_nID;
	int		m_nLeftPaneWidth;
	int		m_nBottomPaneHeight;

	CRect	m_rcWndLeft, m_rcWndRight;
	CRect	m_rcWndTop, m_rcWndBottom;
	CRect	m_rcVertSplitter;
	CRect	m_rcVertBar;
	CRect	m_rcHorizSplitter;
	CRect	m_rcHorizBar;

	BOOL	m_bHorizSplitter;
	BOOL	m_bVertSplitter;
	BOOL	m_bMovingVertSplitter;
	BOOL	m_bDraggingVert; 
	BOOL	m_bHiddenRightPane;
	BOOL	m_bMovingHorizSplitter;
	BOOL	m_bDraggingHoriz; 
	BOOL	m_bHiddenBottomPane;
	BOOL	m_bHiddenLeftPane;
	BOOL	m_bMouseMoveSplittingMode;
	BOOL    m_bHiddenTopPane;

	CPoint	m_ptVertOriginal;		
	CPoint	m_ptVertPrevious;		
	CPoint	m_ptHorizOriginal;		
	CPoint	m_ptHorizPrevious;	

	HWND	m_hwndPrevFocusVert;	
	HWND	m_hwndPrevFocusHoriz;

// Operations
private:

	CPoint MakePtVert(CPoint pt){ return CPoint(pt.x, 0);}
	void SetVertConstraint(CPoint &pt);
	void CancelVertDrag();
	void OnDrawVertBar(CDC &dc, CRect &rc);
	void DrawVertBar();
	void MoveVertPanes(CPoint SplitPoint);

	CPoint MakePtHoriz(CPoint pt) { return CPoint(0, pt.y);}
	void SetHorizConstraint(CPoint &pt);
	void CancelHorizDrag();
	void OnDrawHorizBar(CDC &dc, CRect &rc);
	void DrawHorizBar();
	void MoveHorizPanes(CPoint SplitPoint);

	void InitVertSplitter(
		CWnd*	pParentWnd,				
		CWnd*	pWndLeft,				
		CWnd*	pWndRight,
		UINT	nMaxLeft,
		UINT	nMaxRight);

	void InitHorizSplitter(
		CWnd*	pParentWnd,				
		CWnd*	pWndTop,				
		CWnd*	pWndBottom,
		UINT	nMaxTop,
		UINT	nMaxBottom);

// Attributes
public:

// Operations
public:

	BOOL Create(DWORD dwStyle, 
			CWnd*	pParentWnd, 
			CWnd* pFPane, 
			CWnd* pSPane,
			UINT nID,
			const RECT& rc,
			UINT nFConstr = 30,
			UINT nSConstr = 30
		);

	void MakeVertSplitter();
	void MakeHorizSplitter();

	void HideRightPane();
	void ShowRightPane();

	void HideLeftPane();
	void ShowLeftPane();

	void HideBottomPane();
	void ShowBottomPane();

	void HideTopPane();
	void ShowTopPane();

	UINT GetSplitterStyle();

	int  GetSplitterPos();
	void SetSplitterPos(int nPos);

	BOOL IsMouseMoveSplittingMode();

	void SetMouseMoveSplittingMode(BOOL bMouseMove);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSplitter)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSSplitter)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP() 

private:
	CSSplitter* _ParentNode = nullptr;
	/// <summary>
	/// 0 : 루트, 1 : 왼쪽, 2 : 오른쪽, 3 : 위쪽, 4 : 아래쪽
	/// </summary>
	int _Direction = 0;
	int _WndType = 0;
	VtAttachDirection _AttachDirection;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SSPLITTER_H__BBFAEC99_7CAD_11D8_B153_CEBADFA6E04F__INCLUDED_)

#if !defined(AFX_GRADIENTSTATIC_H__0709E3A1_C8B6_11D6_B74E_004033A0FB96__INCLUDED_)
#define AFX_GRADIENTSTATIC_H__0709E3A1_C8B6_11D6_B74E_004033A0FB96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GradientStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGradientStatic window
typedef UINT (CALLBACK* LPFNDLLFUNC1)(HDC,CONST PTRIVERTEX,DWORD,CONST PVOID,DWORD,DWORD);

class CGradientStatic : public CStatic
{
// Construction
public:
	CGradientStatic();
	virtual ~CGradientStatic();
	void SetWindowText(LPCSTR a_lpstr);
	void SetColor(long cl) {clLeft=cl;};
	void SetGradientColor(long cl) {clRight=cl;};
	void SetTextColor(long cl) {clText=cl;};
	void SetReverseGradient();
	void SetLeftSpacing(int iNoOfPixels) { m_iLeftSpacing = iNoOfPixels; };
	void SetTextAlign(int iAlign ) { m_iAlign = iAlign; }; //0 - left, 1 - center, 2 -right
	void SetVerticalGradient(BOOL a_bVertical = TRUE) { m_bVertical = a_bVertical; };

	static void DrawGradRect(CDC *pDC, CRect r, COLORREF cLeft, COLORREF cRight, BOOL a_bVertical);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGradientStatic)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	CString m_sTEXT;
	int m_iLeftSpacing;
	long clLeft;
	long clRight;
	long clText;
	int m_iAlign;
	HINSTANCE hinst_msimg32;
	BOOL m_bCanDoGradientFill;
	BOOL m_bVertical;
	LPFNDLLFUNC1 dllfunc_GradientFill;
	
	//{{AFX_MSG(CGradientStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRADIENTSTATIC_H__0709E3A1_C8B6_11D6_B74E_004033A0FB96__INCLUDED_)

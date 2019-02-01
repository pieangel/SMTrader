#pragma once


// CGridButton

class CGridButton : public CMFCButton
{
	DECLARE_DYNAMIC(CGridButton)

public:
	CGridButton();
	virtual ~CGridButton();

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



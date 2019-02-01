#pragma once


// VtEditCtrl

class VtEditCtrl : public CEdit
{
	DECLARE_DYNAMIC(VtEditCtrl)

public:
	VtEditCtrl();
	virtual ~VtEditCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};



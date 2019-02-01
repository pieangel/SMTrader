#pragma once
#include "afxpropertygridctrl.h"


// VtSystemProperty dialog
class VtSystem;
class VtSystemProperty : public CDialogEx
{
	DECLARE_DYNAMIC(VtSystemProperty)

public:
	VtSystemProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSystemProperty();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_PROPERTY };
#endif

	VtSystem* System() const { return _System; }
	void System(VtSystem* val) { _System = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMFCPropertyGridCtrl _PropCtrl;

private:
	CFont l_font;
	VtSystem* _System;
	void InitSystemProperty(VtSystem* sys);
public:
	virtual BOOL OnInitDialog();
};

#pragma once
#include "afxwin.h"


// VtAddOutSigDefDlg dialog
class VtSignalDefinitionGrid;
class VtAddOutSigDefDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtAddOutSigDefDlg)

public:
	VtAddOutSigDefDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAddOutSigDefDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_SIG_DEF };
#endif

	VtSignalDefinitionGrid* SigDefGrid() const { return _SigDefGrid; }
	void SigDefGrid(VtSignalDefinitionGrid* val) { _SigDefGrid = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit _EditSignal;
	CEdit _EditStrategy;
private:
	VtSignalDefinitionGrid* _SigDefGrid = nullptr;
};

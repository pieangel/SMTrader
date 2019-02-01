// VtFundNameEditor.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFundNameEditor.h"
#include "afxdialogex.h"
#include "VtFund.h"


// VtFundNameEditor dialog

IMPLEMENT_DYNAMIC(VtFundNameEditor, CDialogEx)

VtFundNameEditor::VtFundNameEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUND_NAME, pParent)
{
	_fund = nullptr;
}

VtFundNameEditor::~VtFundNameEditor()
{
}

void VtFundNameEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FUND_NAME, _EditFundName);
}


BEGIN_MESSAGE_MAP(VtFundNameEditor, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_FUND_NAME, &VtFundNameEditor::OnEnChangeEditFundName)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtFundNameEditor message handlers


BOOL VtFundNameEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	_EditFundName.SetWindowText(fundName);
	_EditFundName.SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtFundNameEditor::OnEnChangeEditFundName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	_EditFundName.GetWindowText(fundName);
}


void VtFundNameEditor::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnClose();
}

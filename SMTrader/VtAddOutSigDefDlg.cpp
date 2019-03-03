// VtAddOutSigDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtAddOutSigDefDlg.h"
#include "afxdialogex.h"
#include "VtOutSignalDef.h"
#include "VtOutSignalDefManager.h"
#include <memory>
#include <algorithm>
#include "VtSignalDefinitionGrid.h"

// VtAddOutSigDefDlg dialog

IMPLEMENT_DYNAMIC(VtAddOutSigDefDlg, CDialogEx)

VtAddOutSigDefDlg::VtAddOutSigDefDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_SIG_DEF, pParent)
{

}

VtAddOutSigDefDlg::~VtAddOutSigDefDlg()
{
}

void VtAddOutSigDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DESC, _EditDesc);
	DDX_Control(pDX, IDC_EDIT_SIGNAL, _EditSignal);
	DDX_Control(pDX, IDC_EDIT_STRATEGY, _EditStrategy);
	DDX_Control(pDX, IDC_EDIT_SYMBOL, _EditSymbol);
}


BEGIN_MESSAGE_MAP(VtAddOutSigDefDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &VtAddOutSigDefDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// VtAddOutSigDefDlg message handlers


void VtAddOutSigDefDlg::OnBnClickedOk()
{
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	SharedOutSigDef sig = std::make_shared<VtOutSignalDef>();
	CString strDesc, strSymbol, strSignal, strStrategy;
	_EditSignal.GetWindowText(strSignal);
	_EditSymbol.GetWindowText(strSymbol);
	_EditStrategy.GetWindowText(strStrategy);
	_EditDesc.GetWindowText(strDesc);
	sig->Name = (LPCTSTR)strSignal;
	sig->SymbolCode = (LPCTSTR)strSymbol;
	sig->StrategyName = (LPCTSTR)strStrategy;
	sig->Desc = (LPCTSTR)strDesc;
	outSigDefMgr->AddOutSignalDef(sig);
	if (_SigDefGrid) _SigDefGrid->AddOutSigDef(sig);
	CDialogEx::OnOK();
}

// SmPortfolioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "SmPortfolioDlg.h"
#include "afxdialogex.h"


// SmPortfolioDlg dialog

IMPLEMENT_DYNAMIC(SmPortfolioDlg, CDialogEx)

SmPortfolioDlg::SmPortfolioDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PORT_COR, pParent)
{

}

SmPortfolioDlg::~SmPortfolioDlg()
{
}

void SmPortfolioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SmPortfolioDlg, CDialogEx)
END_MESSAGE_MAP()


// SmPortfolioDlg message handlers


BOOL SmPortfolioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (_mode == 0) {
		SetWindowText("상품포트폴리오 상관계수 비교표");
	}
	else {
		SetWindowText("신호포트폴리오 상관계수 비교표");
	}
	_Grid.Mode(_mode);
	_Grid.AttachGrid(this, IDC_STATIC_COR);
	
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmPortfolioDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

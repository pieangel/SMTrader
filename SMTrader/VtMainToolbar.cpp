// VtMainToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "VtMainToolbar.h"
#include "afxdialogex.h"
#include "resource.h"
#include <vector>
#include "VtGlobal.h"
#include "MainFrm.h"
// VtMainToolbar dialog

IMPLEMENT_DYNAMIC(VtMainToolbar, CDialogEx)

VtMainToolbar::VtMainToolbar(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAIN_TOOLBAR, pParent)
{

}

VtMainToolbar::~VtMainToolbar()
{
}

void VtMainToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_ACNT_MJ, _BtnAcntMj);
	DDX_Control(pDX, IDC_BTN_ACNT_ORDER, _BtnAcntOrder);
	DDX_Control(pDX, IDC_BTN_FUND_MJ, _BtnFundMj);
	DDX_Control(pDX, IDC_FUND_ORDER, _BtnFundOrder);
}


BEGIN_MESSAGE_MAP(VtMainToolbar, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ACNT_ORDER, &VtMainToolbar::OnBnClickedBtnAcntOrder)
	ON_BN_CLICKED(IDC_FUND_ORDER, &VtMainToolbar::OnBnClickedFundOrder)
	ON_BN_CLICKED(IDC_BTN_ACNT_MJ, &VtMainToolbar::OnBnClickedBtnAcntMj)
	ON_BN_CLICKED(IDC_BTN_FUND_MJ, &VtMainToolbar::OnBnClickedBtnFundMj)
END_MESSAGE_MAP()


// VtMainToolbar message handlers


BOOL VtMainToolbar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	std::vector<CButtonST*> btnVec;
	btnVec.push_back(&_BtnAcntMj);
	btnVec.push_back(&_BtnAcntOrder);
	btnVec.push_back(&_BtnFundMj);
	btnVec.push_back(&_BtnFundOrder);
	
	for (auto it = btnVec.begin(); it != btnVec.end(); ++it) {
		(*it)->SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
		(*it)->SetColor(BTNST_COLOR_FG_OUT, VtGlobal::MainTextColor, true);
		(*it)->SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
		(*it)->SetColor(BTNST_COLOR_BK_IN, RGB(220, 0, 0), true);
		(*it)->SetColor(BTNST_COLOR_BK_OUT, VtGlobal::MainBackColor, true);
		(*it)->SetColor(BTNST_COLOR_BK_FOCUS, RGB(220, 0, 0), true);
	}

	_SiseGrid.AttachGrid(this, IDC_STATIC_SISE);
	int width = VtGlobal::GetHorWidthByScaleFactor(360);
	int height = VtGlobal::GetVerHeightByScaleFactor(64);
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOZORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtMainToolbar::OnReceiveComplete()
{
	_SiseGrid.OnReceiveComplete();
}

void VtMainToolbar::OnReceiveQuoteHd(VtSymbol* sym)
{
	_SiseGrid.OnReceiveQuoteHd(sym);
}


void VtMainToolbar::OnBnClickedBtnAcntOrder()
{
	// TODO: Add your control notification handler code here
	if (_MainFrm)
		_MainFrm->OpenAcntOrder();
}


void VtMainToolbar::OnBnClickedFundOrder()
{
	if (_MainFrm)
		_MainFrm->OpenFundOrder();
}


void VtMainToolbar::OnBnClickedBtnAcntMj()
{
	if (_MainFrm)
		_MainFrm->OpenAcntJango();
}


void VtMainToolbar::OnBnClickedBtnFundMj()
{
	if (_MainFrm)
		_MainFrm->OpenFundJango();
}

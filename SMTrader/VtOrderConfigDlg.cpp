// VtOrderConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderConfigDlg.h"
#include "afxdialogex.h"
#include "VtOrderCenterWndHd.h"
#include <Poco/NumberParser.h>
#include <array>
#include "VtLayoutManager.h"
#include "VtGlobal.h"
#include "VtCutManager.h"
using Poco::NumberParser;

// VtOrderConfigDlg dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtOrderConfigDlg, CDialog)

VtOrderConfigDlg::VtOrderConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CENTER_CONFIG, pParent)
{
	_CenterWnd = nullptr;
	_LayoutMgr = new VtLayoutManager(this);
	// create the background brush
	VERIFY(_BrushBackSel.CreateSolidBrush(VtGlobal::SelDialogBackColor));
	VERIFY(_BrushBackNor.CreateSolidBrush(GetSysColor(COLOR_BTNFACE)));
}

VtOrderConfigDlg::~VtOrderConfigDlg()
{
	if (_LayoutMgr) {
		delete _LayoutMgr;
		_LayoutMgr = nullptr;
	}
}

void VtOrderConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_CUT_LOSS, _CheckCutLoss);
	DDX_Control(pDX, IDC_CHECK_CUT_PROFIT, _CheckCutProfit);
	DDX_Control(pDX, IDC_EDIT_CUT_LOSS, _EditCutLoss);
	DDX_Control(pDX, IDC_EDIT_CUT_PROFIT, _EditCutProfit);
	DDX_Control(pDX, IDC_EDIT_ORDER_TYPE_SLIP, _EditOrderTypeSlip);
	DDX_Control(pDX, IDC_SPIN_CUT_LOSS, _SpinCutLoss);
	DDX_Control(pDX, IDC_SPIN_CUT_PROFIT, _SpinCutProfit);
	DDX_Control(pDX, IDC_SPIN_ORDER_TYPE_SLIP, _SpinOrderTypeSlip);
	DDX_Control(pDX, IDC_BUTTON_PROFIT_LOSS, _BtnApply);
}


BEGIN_MESSAGE_MAP(VtOrderConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_CUT_PROFIT, &VtOrderConfigDlg::OnBnClickedCheckCutProfit)
	ON_BN_CLICKED(IDC_CHECK_CUT_LOSS, &VtOrderConfigDlg::OnBnClickedCheckCutLoss)
	ON_EN_CHANGE(IDC_EDIT_CUT_PROFIT, &VtOrderConfigDlg::OnEnChangeEditCutProfit)
	ON_EN_CHANGE(IDC_EDIT_CUT_LOSS, &VtOrderConfigDlg::OnEnChangeEditCutLoss)
	ON_BN_CLICKED(IDC_BUTTON_PROFIT_LOSS, &VtOrderConfigDlg::OnBnClickedButtonProfitLoss)
	ON_BN_CLICKED(IDC_RADIO_ATMARKET, &VtOrderConfigDlg::OnBnClickedRadioAtmarket)
	ON_BN_CLICKED(IDC_RADIO_ATPRICE, &VtOrderConfigDlg::OnBnClickedRadioAtprice)
	ON_EN_CHANGE(IDC_EDIT_ORDER_TYPE_SLIP, &VtOrderConfigDlg::OnEnChangeEditOrderTypeSlip)
	ON_BN_CLICKED(IDC_STATIC_FILL_PL, &VtOrderConfigDlg::OnBnClickedStaticFillPl)
	ON_BN_CLICKED(IDC_STATIC_ORDER_TYPE, &VtOrderConfigDlg::OnBnClickedStaticOrderType)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// VtOrderConfigDlg message handlers


void VtOrderConfigDlg::OnBnClickedCheckCutProfit()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	//_CenterWnd->OnBnClickedCheckCutProfit();
	if (_CheckCutProfit.GetCheck() == BST_CHECKED)
		_CenterWnd->EnableCutProfit(true);
	else
		_CenterWnd->EnableCutProfit(false);
}


void VtOrderConfigDlg::OnBnClickedCheckCutLoss()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	//_CenterWnd->OnBnClickedCheckCutLoss();
	if (_CheckCutLoss.GetCheck() == BST_CHECKED)
	{
		_CenterWnd->EnableCutLoss(true);
	}
	else
		_CenterWnd->EnableCutLoss(false);
}


void VtOrderConfigDlg::OnEnChangeEditCutProfit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_CenterWnd)
		return;
	//_CenterWnd->OnEnChangeEditCutProfit();
	if (!_EditCutProfit.GetSafeHwnd())
		return;

	CString strVal;
	_EditCutProfit.GetWindowText(strVal);
	std::string amt = strVal;
	int val = 0;
	if (!NumberParser::tryParse(amt, val))
	{
		return;
	}
	else
	{
		_CenterWnd->CutProfit(val);
	}
}


void VtOrderConfigDlg::OnEnChangeEditCutLoss()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_CenterWnd)
		return;
	//_CenterWnd->OnEnChangeEditCutLoss();
	if (!_EditCutLoss.GetSafeHwnd())
		return;

	CString strVal;
	_EditCutLoss.GetWindowText(strVal);
	std::string amt = strVal;
	int val = 0;
	if (!NumberParser::tryParse(amt, val))
	{
		return;
	}
	else
	{
		_CenterWnd->CutLoss(val);
	}
}


void VtOrderConfigDlg::OnBnClickedButtonProfitLoss()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	if (!_EditCutProfit.GetSafeHwnd())
		return;

	VtCutManager* cutMgr = _CenterWnd->GetCutManager();
	if (cutMgr && (!cutMgr->EnableCutLoss() && !cutMgr->EnableCutProfit()))
		return;

	CString strVal;
	_EditCutProfit.GetWindowText(strVal);
	std::string amt = strVal;
	int cutProfit = 0, cutLoss = 0;
	if (!NumberParser::tryParse(amt, cutProfit))
	{
		return;
	}

	//_CenterWnd->OnEnChangeEditCutLoss();
	if (!_EditCutLoss.GetSafeHwnd())
		return;

	_EditCutLoss.GetWindowText(strVal);
	amt = strVal;
	if (!NumberParser::tryParse(amt, cutLoss))
	{
		return;
	}

	_CenterWnd->CutProfit(cutProfit);
	_CenterWnd->CutLoss(cutLoss);

	_CenterWnd->ApplyProfitLossForPosition();
}


void VtOrderConfigDlg::OnBnClickedRadioAtmarket()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	_CenterWnd->OrderType(0);
}


void VtOrderConfigDlg::OnBnClickedRadioAtprice()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	_CenterWnd->OrderType(1);
}


void VtOrderConfigDlg::OnEnChangeEditOrderTypeSlip()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_CenterWnd)
		return;
	//_CenterWnd->OnEnChangeEditOrderTypeSlip();

	if (!_EditOrderTypeSlip.GetSafeHwnd())
		return;

	CString strVal;
	_EditOrderTypeSlip.GetWindowText(strVal);
	std::string amt = strVal;
	int orderTypeSlip = 0;
	if (!NumberParser::tryParse(amt, orderTypeSlip))
	{
		return;
	}

	_CenterWnd->OrderTypeSlip(orderTypeSlip);
}


void VtOrderConfigDlg::SaveControls()
{
	if (!_LayoutMgr)
		return;
	CRect rcWnd;
	_LayoutMgr->AddWindow(_T("이익실현"), IDC_CHECK_CUT_PROFIT, GetClientArea(IDC_CHECK_CUT_PROFIT));
	_LayoutMgr->AddWindow(_T("손실제한"), IDC_CHECK_CUT_LOSS, GetClientArea(IDC_CHECK_CUT_LOSS));
	_LayoutMgr->AddWindow(_T("이익실현편집"), IDC_EDIT_CUT_PROFIT, GetClientArea(IDC_EDIT_CUT_PROFIT));
	_LayoutMgr->AddWindow(_T("손실제한편집"), IDC_EDIT_CUT_LOSS, GetClientArea(IDC_EDIT_CUT_LOSS));
	_LayoutMgr->AddWindow(_T("이익손실적용"), IDC_BUTTON_PROFIT_LOSS, GetClientArea(IDC_BUTTON_PROFIT_LOSS));
	_LayoutMgr->AddWindow(_T("이익실현스핀"), IDC_SPIN_CUT_PROFIT, GetClientArea(IDC_SPIN_CUT_PROFIT));
	_LayoutMgr->AddWindow(_T("손실제한스핀"), IDC_SPIN_CUT_LOSS, GetClientArea(IDC_SPIN_CUT_LOSS));
	_LayoutMgr->AddWindow(_T("청산타입"), IDC_STATIC_ORDER_TYPE, GetClientArea(IDC_STATIC_ORDER_TYPE));
	_LayoutMgr->AddWindow(_T("시장가"), IDC_RADIO_ATMARKET, GetClientArea(IDC_RADIO_ATMARKET));
	_LayoutMgr->AddWindow(_T("지정가"), IDC_RADIO_ATPRICE, GetClientArea(IDC_RADIO_ATPRICE));
	_LayoutMgr->AddWindow(_T("지정가스탑"), IDC_EDIT_ORDER_TYPE_SLIP, GetClientArea(IDC_EDIT_ORDER_TYPE_SLIP));
	_LayoutMgr->AddWindow(_T("지정가스탑스핀"), IDC_SPIN_ORDER_TYPE_SLIP, GetClientArea(IDC_SPIN_ORDER_TYPE_SLIP));
	_LayoutMgr->AddWindow(_T("손절익절그룹박스"), IDC_STATIC_FILL_PL, GetClientArea(IDC_STATIC_FILL_PL));
}

CRect VtOrderConfigDlg::GetClientArea(int resourceID)
{
	CWnd* wnd = (CWnd*)GetDlgItem(resourceID);
	CRect rcWnd;
	wnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	return rcWnd;
}

BOOL VtOrderConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	_BtnApply.SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
	_BtnApply.DrawFlatFocus(TRUE);

	_SpinCutLoss.SetRange(0, 100);
	_SpinCutProfit.SetRange(0, 100);
	_SpinOrderTypeSlip.SetRange(0, 100);

	if (_CenterWnd) {
		CString strValue;
		strValue.Format(_T("%d"), _CenterWnd->CutLoss());
		_EditCutLoss.SetWindowText(strValue);

		strValue.Format(_T("%d"), _CenterWnd->CutProfit());
		_EditCutProfit.SetWindowText(strValue);

		if (_CenterWnd->OrderType() == 1)
			((CButton*)GetDlgItem(IDC_RADIO_ATPRICE))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_RADIO_ATMARKET))->SetCheck(BST_CHECKED);

		strValue.Format(_T("%d"), _CenterWnd->OrderTypeSlip());
		_EditOrderTypeSlip.SetWindowText(strValue);

		//RecalateLayout();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderConfigDlg::RecalateLayout(CRect rcClient)
{
	if (!_CenterWnd)
		return;
	int availWidth = rcClient.Width() - 160;
	// 둘다 표현할 수 있는 최소 너비
	int minWidth = 252;
	//  균등하게 표현할 수 있는 최소 너비
	int bothSameMinWidth = 327;
	int top = 20;
	int _CheckX = 10;
	int _EditX = 80;
	int _Button_X = 121;

	CRect rcTemp(rcClient);
	rcTemp.DeflateRect(5, 5);
	CRect& rcGroup = _LayoutMgr->GetRect(IDC_STATIC_FILL_PL);
	rcGroup.left = rcTemp.left;
	rcGroup.top = rcTemp.top;
	rcGroup.right = rcTemp.right;
	rcGroup.bottom = rcTemp.bottom - 10;

	CStatic* grpBox = (CStatic*)GetDlgItem(IDC_STATIC_FILL_PL);
	grpBox->SetWindowPos(nullptr, rcGroup.left, rcGroup.top, rcGroup.Width(), rcGroup.Height(), SWP_NOMOVE);
	
	_CheckCutProfit.SetWindowPos(NULL, _CheckX, top + 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	CWnd * spin = _EditCutProfit.GetWindow(GW_HWNDNEXT);
	CSize offset(19, 0);
	_EditCutProfit.SetWindowPos(NULL, _EditX, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	spin->SetWindowPos(NULL, _EditX + offset.cx, top + offset.cy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	CButton* button = (CButton*)GetDlgItem(IDC_BUTTON_PROFIT_LOSS);
	button->SetWindowPos(NULL, _Button_X, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	top = 46;
	_CheckCutLoss.SetWindowPos(NULL, _CheckX, top + 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	spin = _EditCutLoss.GetWindow(GW_HWNDNEXT);
	_EditCutLoss.SetWindowPos(NULL, _EditX, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	spin->SetWindowPos(NULL, _EditX + offset.cx, top + offset.cy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	CWnd* group = (CWnd*)GetDlgItem(IDC_STATIC_ORDER_TYPE);
	CButton* radioMarket = (CButton*)GetDlgItem(IDC_RADIO_ATMARKET);
	CButton* radioPrice = (CButton*)GetDlgItem(IDC_RADIO_ATPRICE);
	if (rcClient.Width() < minWidth) {
		group->ShowWindow(SW_HIDE);
		radioMarket->ShowWindow(SW_HIDE);
		radioPrice->ShowWindow(SW_HIDE);
		_EditOrderTypeSlip.ShowWindow(SW_HIDE);
		_SpinOrderTypeSlip.ShowWindow(SW_HIDE);
		return;
	} else {
		group->ShowWindow(SW_SHOW);
		radioMarket->ShowWindow(SW_SHOW);
		radioPrice->ShowWindow(SW_SHOW);
		_EditOrderTypeSlip.ShowWindow(SW_SHOW);
		_SpinOrderTypeSlip.ShowWindow(SW_SHOW);
	}

	int remWidth = rcClient.Width() > bothSameMinWidth ? (rcClient.Width() / 2) : availWidth;
	int groupWidth = remWidth - 10;
	int groupY = 15;
	int groupX = rcClient.Width() - remWidth;
	group->SetWindowPos(NULL, groupX, groupY, groupWidth, 58, SWP_NOZORDER);
	top = groupY + 12;
	_CheckX = groupX + 5;
	_EditX = groupX + 55;
	radioMarket->SetWindowPos(NULL, _CheckX, top + 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	radioPrice->SetWindowPos(NULL, _CheckX, top + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_EditOrderTypeSlip.SetWindowPos(NULL, _EditX, top + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_SpinOrderTypeSlip.SetWindowPos(NULL, _EditX + offset.cx, top + 20 + offset.cy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

}


void VtOrderConfigDlg::OnBnClickedStaticFillPl()
{
	// TODO: Add your control notification handler code here
}


void VtOrderConfigDlg::OnBnClickedStaticOrderType()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderConfigDlg::OnEraseBkgnd(CDC* pDC)
{
	//if (_CenterWnd->GetCenterWndCount() <= 1)
	//	return FALSE;

	//CBrush brush;
	if (_CenterWnd->Activated())
		pDC->SelectObject(&_BrushBackSel);
	else
		pDC->SelectObject(&_BrushBackNor);

	//pDC->SelectObject(&brush);
	CRect rect;
	GetClientRect(&rect);
	//pDC->FillRect(&rect, &brush);

	if (_CenterWnd->Activated())
		pDC->FillRect(&rect, &_BrushBackSel);
	else
		pDC->FillRect(&rect, &_BrushBackNor);
	return TRUE;
}


void VtOrderConfigDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (_CenterWnd) {
		_CenterWnd->OnLButtonDown(1, CPoint(0, 0));
	}
	CDialog::OnLButtonDown(nFlags, point);
}


HBRUSH VtOrderConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	HBRUSH hBrush = NULL;
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
		// just return a not NULL brush handle
		if (_CenterWnd->Activated())
			hBrush = (HBRUSH)_BrushBackSel;
		else
			hBrush = (HBRUSH)_BrushBackNor;
		break;
	case CTLCOLOR_STATIC:
	{
		// set text color, transparent back node then 
		//pDC->SetTextColor(m_crStaticText);
		pDC->SetBkMode(TRANSPARENT);
		// return a not NULL brush handle
		//hBrush = (HBRUSH)m_brushBack;
		if (_CenterWnd->Activated())
			hBrush = (HBRUSH)_BrushBackSel;
		else
			hBrush = (HBRUSH)_BrushBackNor;
	}
	break;
	default:
		// do the default processing
		hBrush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
	}
	return hBrush;
}

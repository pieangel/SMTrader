// VtOrderSettlePage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderSettlePage.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"

// VtOrderSettlePage dialog

IMPLEMENT_DYNAMIC(VtOrderSettlePage, CDialogEx)

VtOrderSettlePage::VtOrderSettlePage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_SETTLE, pParent)
{

}

VtOrderSettlePage::~VtOrderSettlePage()
{
}

void VtOrderSettlePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_PROFIT_LOSS, _BtnProfitLoss);
	DDX_Control(pDX, IDC_CHECK_LOSS, _CheckLoss);
	DDX_Control(pDX, IDC_CHECK_PROFIT, _CheckProfit);
	DDX_Control(pDX, IDC_CHECK_TRAIL_STOP, _CheckTrailStop);
	DDX_Control(pDX, IDC_EDIT_LOSS, _EditLoss);
	DDX_Control(pDX, IDC_EDIT_PROFIT, _EditProfit);
	DDX_Control(pDX, IDC_EDIT_PS_STOP, _EditPsStop);
	DDX_Control(pDX, IDC_EDIT_TS_DEFAULT_LOSS, _EditTsDefaultLoss);
	DDX_Control(pDX, IDC_EDIT_TS_STOP, _EditTsStop);
	DDX_Control(pDX, IDC_SPIN_LOSS, _SpinLoss);
	DDX_Control(pDX, IDC_SPIN_PROFIT, _SpinProfit);
	DDX_Control(pDX, IDC_SPIN_PS_PROFIT, _SpinPsProfit);
	DDX_Control(pDX, IDC_SPIN_PS_STOP, _SpinPsStop);
	DDX_Control(pDX, IDC_SPIN_TS, _SpinTs);
	DDX_Control(pDX, IDC_SPIN_TS_DEFAULT_LOSS, _SpinTsDefaultLoss);
	DDX_Control(pDX, IDC_SPIN_TS_STOP, _SpinTsStop);
	DDX_Control(pDX, IDC_EDIT_TS_PROFIT, _EditTsProfit);
	DDX_Control(pDX, IDC_EDIT_TS_TRAIL, _EditTsTrail);
}


void VtOrderSettlePage::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

BEGIN_MESSAGE_MAP(VtOrderSettlePage, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_PROFIT, &VtOrderSettlePage::OnBnClickedCheckProfit)
	ON_BN_CLICKED(IDC_CHECK_LOSS, &VtOrderSettlePage::OnBnClickedCheckLoss)
	ON_EN_CHANGE(IDC_EDIT_PROFIT, &VtOrderSettlePage::OnEnChangeEditProfit)
	ON_EN_CHANGE(IDC_EDIT_LOSS, &VtOrderSettlePage::OnEnChangeEditLoss)
	ON_BN_CLICKED(IDC_BUTTON_PROFIT_LOSS, &VtOrderSettlePage::OnBnClickedButtonProfitLoss)
	ON_EN_CHANGE(IDC_EDIT_PS_STOP, &VtOrderSettlePage::OnEnChangeEditPsStop)
	ON_BN_CLICKED(IDC_CHECK_TRAIL_STOP, &VtOrderSettlePage::OnBnClickedCheckTrailStop)
	ON_EN_CHANGE(IDC_EDIT_TS_DEFAULT_LOSS, &VtOrderSettlePage::OnEnChangeEditTsDefaultLoss)
	ON_EN_CHANGE(IDC_EDIT_TS_PROFIT, &VtOrderSettlePage::OnEnChangeEditTsProfit)
	ON_EN_CHANGE(IDC_EDIT_TS_TRAIL, &VtOrderSettlePage::OnEnChangeEditTsTrail)
	ON_BN_CLICKED(IDC_RADIO_TS_ATMARKET, &VtOrderSettlePage::OnBnClickedRadioTsAtmarket)
	ON_BN_CLICKED(IDC_RADIO_TS_STOP, &VtOrderSettlePage::OnBnClickedRadioTsStop)
	ON_EN_CHANGE(IDC_EDIT_TS_STOP, &VtOrderSettlePage::OnEnChangeEditTsStop)
	ON_COMMAND(IDC_RADIO_ATMARKET, &VtOrderSettlePage::OnRadioAtmarket)
	ON_COMMAND(IDC_RADIO_STOP, &VtOrderSettlePage::OnRadioStop)
END_MESSAGE_MAP()


// VtOrderSettlePage message handlers


void VtOrderSettlePage::OnBnClickedCheckProfit()
{
	// TODO: Add your control notification handler code here
	if (_CheckProfit.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseAutoStopProfit = true;
	else
		_OrderConfigMgr->UseAutoStopProfit = false;
}


void VtOrderSettlePage::OnBnClickedCheckLoss()
{
	// TODO: Add your control notification handler code here
	if (_CheckLoss.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseAutoStopLosscut = true;
	else
		_OrderConfigMgr->UseAutoStopLosscut = false;
}


void VtOrderSettlePage::OnEnChangeEditProfit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditProfit.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->AutoStopProfitValue = VtOrderWnd::GetValue(&_EditProfit);
// 	}
}


void VtOrderSettlePage::OnEnChangeEditLoss()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditLoss.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->AutoStopLosscutValue = CVtOrderWnd::GetValue(&_EditLoss);
// 	}
}


void VtOrderSettlePage::OnBnClickedButtonProfitLoss()
{
	// TODO: Add your control notification handler code here
}

void VtOrderSettlePage::OnEnChangeEditPsStop()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditPsStop.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->AutoStopPriceSlip = CVtOrderWnd::GetValue(&_EditPsStop);
// 	}
}


void VtOrderSettlePage::OnBnClickedCheckTrailStop()
{
	// TODO: Add your control notification handler code here
	if (_CheckTrailStop.GetCheck() == BST_CHECKED)
	{
		_OrderConfigMgr->UseTrailStop = true;
	}
	else
	{
		_OrderConfigMgr->UseTrailStop = false;
	}
}


void VtOrderSettlePage::OnEnChangeEditTsDefaultLoss()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditTsDefaultLoss.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->AutoStopPriceSlip = CVtOrderWnd::GetValue(&_EditTsDefaultLoss);
// 	}
}


void VtOrderSettlePage::OnEnChangeEditTsProfit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditTsProfit.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->TrailStopProfit = CVtOrderWnd::GetValue(&_EditTsProfit);
// 	}
}


void VtOrderSettlePage::OnEnChangeEditTsTrail()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditTsTrail.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->TrailStopTrail = CVtOrderWnd::GetValue(&_EditTsTrail);
// 	}
}


void VtOrderSettlePage::OnBnClickedRadioTsAtmarket()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->TrailStopPriceType = VtPriceType::Market;
}


void VtOrderSettlePage::OnBnClickedRadioTsStop()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->TrailStopPriceType = VtPriceType::Price;
}


void VtOrderSettlePage::OnEnChangeEditTsStop()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
// 	if (_EditTsStop.GetSafeHwnd())
// 	{
// 		_OrderConfigMgr->TrailStopPriceSlip = CVtOrderWnd::GetValue(&_EditTsStop);
// 	}
}


BOOL VtOrderSettlePage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitControlValues();
	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderSettlePage::InitControlValues()
{
	if (!_OrderConfigMgr)
		return;

	_SpinLoss.SetRange32(_OrderConfigMgr->MinAutoStopVal, _OrderConfigMgr->MaxAutoStopVal);
	_SpinProfit.SetRange32(_OrderConfigMgr->MinAutoStopVal, _OrderConfigMgr->MaxAutoStopVal);
	_SpinPsProfit.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);
	_SpinPsStop.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);
	_SpinTs.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);
	_SpinTsDefaultLoss.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);
	_SpinTsStop.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);

	if (_OrderConfigMgr->UseAutoStopProfit)
		_CheckProfit.SetCheck(BST_CHECKED);
	else
		_CheckProfit.SetCheck(BST_UNCHECKED);

	if (_OrderConfigMgr->UseAutoStopLosscut)
		_CheckLoss.SetCheck(BST_CHECKED);
	else
		_CheckLoss.SetCheck(BST_UNCHECKED);

// 	CVtOrderWnd::SetValue(&_EditProfit, _OrderConfigMgr->AutoStopProfitValue);
// 	CVtOrderWnd::SetValue(&_EditLoss, _OrderConfigMgr->AutoStopLosscutValue);

	if (_OrderConfigMgr->AutoStopPriceType == VtPriceType::Market)
	{
		((CButton*)GetDlgItem(IDC_RADIO_ATMARKET))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_STOP))->SetCheck(BST_CHECKED);
	}

// 	CVtOrderWnd::SetValue(&_EditPsStop, _OrderConfigMgr->AutoStopPriceSlip);
// 
// 	if (_OrderConfigMgr->UseTrailStop)
// 		_CheckTrailStop.SetCheck(BST_CHECKED);
// 	else
// 		_CheckTrailStop.SetCheck(BST_UNCHECKED);
// 
// 	CVtOrderWnd::SetValue(&_EditTsDefaultLoss, _OrderConfigMgr->TrailStopDefaultLosscut);
// 	CVtOrderWnd::SetValue(&_EditTsProfit, _OrderConfigMgr->TrailStopProfit);
// 	CVtOrderWnd::SetValue(&_EditTsTrail, _OrderConfigMgr->TrailStopTrail);
// 
// 	if (_OrderConfigMgr->TrailStopPriceType == VtPriceType::Market)
// 	{
// 		((CButton*)GetDlgItem(IDC_RADIO_TS_ATMARKET))->SetCheck(BST_CHECKED);
// 	}
// 	else
// 	{
// 		((CButton*)GetDlgItem(IDC_RADIO_TS_STOP))->SetCheck(BST_CHECKED);
// 	}
// 	
// 	CVtOrderWnd::SetValue(&_EditTsStop, _OrderConfigMgr->TrailStopPriceSlip);
}


void VtOrderSettlePage::OnRadioAtmarket()
{
	// TODO: Add your command handler code here
	_OrderConfigMgr->AutoStopPriceType = VtPriceType::Market;
}


void VtOrderSettlePage::OnRadioStop()
{
	// TODO: Add your command handler code here
	_OrderConfigMgr->AutoStopPriceType = VtPriceType::Price;
}

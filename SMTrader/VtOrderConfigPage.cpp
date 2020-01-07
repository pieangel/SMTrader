// VtOrderConfigPage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderConfigPage.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtOrderWnd.h"

// VtOrderConfigPage dialog

IMPLEMENT_DYNAMIC(VtOrderConfigPage, CDialogEx)

VtOrderConfigPage::VtOrderConfigPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_CONFIG, pParent)
{

}

VtOrderConfigPage::~VtOrderConfigPage()
{
}

void VtOrderConfigPage::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
}

void VtOrderConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_CONFIRM_SHORTCUTORDER, _CheckConfirmShortcutOrder);
	DDX_Control(pDX, IDC_CHECK_FIX_HOGA, _CheckFixHoga);
	DDX_Control(pDX, IDC_CHECK_ONECLICK, _CheckOneClick);
	DDX_Control(pDX, IDC_CHECK_ORDER_BYSPACE, _CheckOrderBySpacebar);
	DDX_Control(pDX, IDC_CHECK_USE_SHORTCUT, _CheckUseShortcut);
	DDX_Control(pDX, IDC_EDIT_ORDER_CELL_HEIGHT, _EditOrderCellHeight);
	DDX_Control(pDX, IDC_EDIT_ORDER_CELL_WIDTH, _EditOrderCellWidth);
	DDX_Control(pDX, IDC_SPIN_ORDER_CELL_HEIGHT, _SpinOrderCellHeight);
	DDX_Control(pDX, IDC_SPIN_ORDER_CELL_WIDTH, _SpinOrderCellWidth);
}


BEGIN_MESSAGE_MAP(VtOrderConfigPage, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ONECLICK, &VtOrderConfigPage::OnBnClickedCheckOneclick)
	ON_BN_CLICKED(IDC_CHECK_FIX_HOGA, &VtOrderConfigPage::OnBnClickedCheckFixHoga)
	ON_BN_CLICKED(IDC_CHECK_ORDER_BYSPACE, &VtOrderConfigPage::OnBnClickedCheckOrderByspace)
	ON_BN_CLICKED(IDC_RADIO_BY_MOUSE_SELECT, &VtOrderConfigPage::OnBnClickedRadioByMouseSelect)
	ON_BN_CLICKED(IDC_RADIO_BY_MOUSE_POS, &VtOrderConfigPage::OnBnClickedRadioByMousePos)
	ON_BN_CLICKED(IDC_CHECK_USE_SHORTCUT, &VtOrderConfigPage::OnBnClickedCheckUseShortcut)
	ON_BN_CLICKED(IDC_CHECK_CONFIRM_SHORTCUTORDER, &VtOrderConfigPage::OnBnClickedCheckConfirmShortcutorder)
	ON_BN_CLICKED(IDC_RADIO_CANCEL_LASTORDER, &VtOrderConfigPage::OnBnClickedRadioCancelLastorder)
	ON_BN_CLICKED(IDC_RADIO_CANCEL_ORDER_BYSELPOS, &VtOrderConfigPage::OnBnClickedRadioCancelOrderByselpos)
	ON_EN_CHANGE(IDC_EDIT_ORDER_CELL_HEIGHT, &VtOrderConfigPage::OnEnChangeEditOrderCellHeight)
	ON_EN_CHANGE(IDC_EDIT_ORDER_CELL_WIDTH, &VtOrderConfigPage::OnEnChangeEditOrderCellWidth)
END_MESSAGE_MAP()


// VtOrderConfigPage message handlers


BOOL VtOrderConfigPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitControlValues();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderConfigPage::InitControlValues()
{
	if (!_OrderConfigMgr)
		return;
	/*
	if (_OrderConfigMgr->UseOneClickOrder)
		_CheckOneClick.SetCheck(BST_CHECKED);
	else
		_CheckOneClick.SetCheck(BST_UNCHECKED);

	if (_OrderConfigMgr->UseFixHoga)
		_CheckFixHoga.SetCheck(BST_CHECKED);
	else
		_CheckFixHoga.SetCheck(BST_UNCHECKED);

	if (_OrderConfigMgr->UseSpaceBarOrder)
		_CheckOrderBySpacebar.SetCheck(BST_CHECKED);
	else
		_CheckOrderBySpacebar.SetCheck(BST_UNCHECKED);

	
	CVtOrderWnd::SetValue(&_EditOrderCellWidth, _OrderConfigMgr->OrderCellWidth);
	CVtOrderWnd::SetValue(&_EditOrderCellHeight, _OrderConfigMgr->OrderCellHeight);

	if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMouseClick)
	{
		((CButton*)GetDlgItem(IDC_RADIO_BY_MOUSE_SELECT))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_BY_MOUSE_POS))->SetCheck(BST_CHECKED);
	}

	if (_OrderConfigMgr->UseShortCutOrder)
		_CheckUseShortcut.SetCheck(BST_CHECKED);
	else
		_CheckUseShortcut.SetCheck(BST_UNCHECKED);

	if (_OrderConfigMgr->UseShortCutOrderConfirm)
		_CheckConfirmShortcutOrder.SetCheck(BST_CHECKED);
	else
		_CheckConfirmShortcutOrder.SetCheck(BST_UNCHECKED);

	if (_OrderConfigMgr->RButtonCancel == VtRButtonCancelOrder::CancelLastOrder)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CANCEL_LASTORDER))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CANCEL_ORDER_BYSELPOS))->SetCheck(BST_CHECKED);
	}
	*/
}


void VtOrderConfigPage::OnBnClickedCheckOneclick()
{
	// TODO: Add your control notification handler code here
	if (_CheckOneClick.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseOneClickOrder = true;
	else
		_OrderConfigMgr->UseOneClickOrder = false;
}


void VtOrderConfigPage::OnBnClickedCheckFixHoga()
{
	// TODO: Add your control notification handler code here
	if (_CheckFixHoga.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseFixHoga = true;
	else
		_OrderConfigMgr->UseFixHoga = false;
}


void VtOrderConfigPage::OnBnClickedCheckOrderByspace()
{
	// TODO: Add your control notification handler code here
	if (_CheckOrderBySpacebar.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseSpaceBarOrder = true;
	else
		_OrderConfigMgr->UseSpaceBarOrder = false;
}


void VtOrderConfigPage::OnBnClickedRadioByMouseSelect()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->SpaceBarorderType = VtSpaceByOrderType::ByMouseClick;
	_OrderConfigMgr->centerWnd->ChangeFocus();
}


void VtOrderConfigPage::OnBnClickedRadioByMousePos()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->SpaceBarorderType = VtSpaceByOrderType::ByMousePositon;
	_OrderConfigMgr->centerWnd->ChangeFocus();
}


void VtOrderConfigPage::OnBnClickedCheckUseShortcut()
{
	// TODO: Add your control notification handler code here
	if (_CheckUseShortcut.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseShortCutOrder = true;
	else
		_OrderConfigMgr->UseShortCutOrder = false;
}


void VtOrderConfigPage::OnBnClickedCheckConfirmShortcutorder()
{
	// TODO: Add your control notification handler code here
	if (_CheckConfirmShortcutOrder.GetCheck() == BST_CHECKED)
		_OrderConfigMgr->UseShortCutOrderConfirm = true;
	else
		_OrderConfigMgr->UseShortCutOrderConfirm = false;
}


void VtOrderConfigPage::OnBnClickedRadioCancelLastorder()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->RButtonCancel = VtRButtonCancelOrder::CancelLastOrder;
}


void VtOrderConfigPage::OnBnClickedRadioCancelOrderByselpos()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->RButtonCancel = VtRButtonCancelOrder::CancelOnMousePosition;
}


void VtOrderConfigPage::OnEnChangeEditOrderCellHeight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//_OrderConfigMgr->OrderCellHeight = CVtOrderWnd::GetValue(&_EditOrderCellHeight);
}


void VtOrderConfigPage::OnEnChangeEditOrderCellWidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//_OrderConfigMgr->OrderCellWidth = CVtOrderWnd::GetValue(&_EditOrderCellWidth);
}

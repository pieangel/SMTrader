// VtOrderRightConfigHd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderRightConfigHd.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtOrderCenterWndHd.h"

// VtOrderRightConfigHd dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtOrderRightConfigHd, CDialogEx)

VtOrderRightConfigHd::VtOrderRightConfigHd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_RIGHT_CONFIG_HD, pParent)
{
	_OrderConfigMgr = nullptr;
}

VtOrderRightConfigHd::~VtOrderRightConfigHd()
{
}

void VtOrderRightConfigHd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FORMAT_THOUSAND, _FormatThousand);
	DDX_Control(pDX, IDC_ONE_CLICK, _OneClick);
	DDX_Control(pDX, IDC_SPACEBAR_ORDER, _SpaceBarOrder);
}


BEGIN_MESSAGE_MAP(VtOrderRightConfigHd, CDialogEx)
	ON_BN_CLICKED(IDC_ONE_CLICK, &VtOrderRightConfigHd::OnBnClickedOneClick)
	ON_BN_CLICKED(IDC_SPACEBAR_ORDER, &VtOrderRightConfigHd::OnBnClickedSpacebarOrder)
	ON_BN_CLICKED(IDC_RADIO_BY_MOUSE_SELECT, &VtOrderRightConfigHd::OnBnClickedRadioByMouseSelect)
	ON_BN_CLICKED(IDC_RADIO_BY_MOUSE_POS, &VtOrderRightConfigHd::OnBnClickedRadioByMousePos)
	ON_BN_CLICKED(IDC_CANCEL_LAST_ORDER, &VtOrderRightConfigHd::OnBnClickedCancelLastOrder)
	ON_BN_CLICKED(IDC_CANCEL_SELECTED, &VtOrderRightConfigHd::OnBnClickedCancelSelected)
	ON_BN_CLICKED(IDC_FORMAT_THOUSAND, &VtOrderRightConfigHd::OnBnClickedFormatThousand)
END_MESSAGE_MAP()


// VtOrderRightConfigHd message handlers


void VtOrderRightConfigHd::OnBnClickedOneClick()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		if (_OneClick.GetCheck() == BST_CHECKED)
			_OrderConfigMgr->UseOneClickOrder = true;
		else
			_OrderConfigMgr->UseOneClickOrder = false;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedSpacebarOrder()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		if (_SpaceBarOrder.GetCheck() == BST_CHECKED)
			_OrderConfigMgr->UseSpaceBarOrder = true;
		else
			_OrderConfigMgr->UseSpaceBarOrder = false;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedRadioByMouseSelect()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		_OrderConfigMgr->SpaceBarorderType = VtSpaceByOrderType::ByMouseClick;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedRadioByMousePos()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		_OrderConfigMgr->SpaceBarorderType = VtSpaceByOrderType::ByMousePositon;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedCancelLastOrder()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		_OrderConfigMgr->RButtonCancel = VtRButtonCancelOrder::CancelLastOrder;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedCancelSelected()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		_OrderConfigMgr->RButtonCancel = VtRButtonCancelOrder::CancelOnMousePosition;

		if (_OrderConfigMgr->_HdCenterWnd)
			_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
	}
}


void VtOrderRightConfigHd::OnBnClickedFormatThousand()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr)
	{
		if (_FormatThousand.GetCheck() == BST_CHECKED)
			_OrderConfigMgr->UseThousandOnProfit = true;
		else
			_OrderConfigMgr->UseThousandOnProfit = false;
	}
}


BOOL VtOrderRightConfigHd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	// TODO:  Add extra initialization here
	InitCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderRightConfigHd::InitCtrl()
{
	if (_OrderConfigMgr)
	{
		if (_OrderConfigMgr->UseOneClickOrder)
			_OneClick.SetCheck(BST_CHECKED);
		else
			_OneClick.SetCheck(BST_UNCHECKED);

		if (_OrderConfigMgr->UseSpaceBarOrder)
			_SpaceBarOrder.SetCheck(BST_CHECKED);
		else
			_SpaceBarOrder.SetCheck(BST_UNCHECKED);

		if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMouseClick)
			((CButton*)GetDlgItem(IDC_RADIO_BY_MOUSE_SELECT))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_RADIO_BY_MOUSE_POS))->SetCheck(BST_CHECKED);

		if (_OrderConfigMgr->RButtonCancel == VtRButtonCancelOrder::CancelLastOrder)
			((CButton*)GetDlgItem(IDC_CANCEL_LAST_ORDER))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_CANCEL_SELECTED))->SetCheck(BST_CHECKED);

		if (_OrderConfigMgr->UseThousandOnProfit)
			_FormatThousand.SetCheck(BST_CHECKED);
		else
			_FormatThousand.SetCheck(BST_UNCHECKED);
	}
}

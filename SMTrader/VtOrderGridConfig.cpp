// VtOrderGridConfig.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderGridConfig.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtOrderCenterWndHd.h"
#include "VtOrderPanelGrid.h"
#include "VtOrderWndHd.h"

// VtOrderGridConfig dialog

IMPLEMENT_DYNAMIC(VtOrderGridConfig, CDialogEx)

VtOrderGridConfig::VtOrderGridConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDERGRID_CONFIG, pParent)
{
	_CenterWnd = nullptr;
}

VtOrderGridConfig::VtOrderGridConfig(VtOrderCenterWndHd* centerWnd)
{
	_CenterWnd = centerWnd;
}

VtOrderGridConfig::~VtOrderGridConfig()
{
}

void VtOrderGridConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CELL_HEIGHT, _EditCellHeight);
	DDX_Control(pDX, IDC_EDIT_CELL_WIDTH, _EditCellWidth);
	DDX_Control(pDX, IDC_EDIT_TICK_COUNT, _EditTickCount);
	DDX_Control(pDX, IDC_CHECK_SHOW_COUNTCOL, _CheckShowCountCol);
	DDX_Control(pDX, IDC_CHECK_SHOW_ORDERCOL, _CheckShowOrderCol);
	DDX_Control(pDX, IDC_CHECK_SHOW_PLCONFIGWND, _CheckShowPLConfigWnd);
	DDX_Control(pDX, IDC_CHECK_SHOW_STOPCOL, _CheckShowStopCol);
	DDX_Control(pDX, IDC_CHECK_SHOW_TICKWND, _CheckShowTickWnd);
	DDX_Control(pDX, IDC_CHECK_FILTER_HOGA, _CheckFilterHoga);
}


BEGIN_MESSAGE_MAP(VtOrderGridConfig, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_SHOW_ORDERCOL, &VtOrderGridConfig::OnBnClickedCheckShowOrdercol)
	ON_BN_CLICKED(IDC_CHECK_SHOW_STOPCOL, &VtOrderGridConfig::OnBnClickedCheckShowStopcol)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TICKWND, &VtOrderGridConfig::OnBnClickedCheckShowTickwnd)
	ON_BN_CLICKED(IDC_RADIO_TICKLEFT, &VtOrderGridConfig::OnBnClickedRadioTickleft)
	ON_BN_CLICKED(IDC_RADIO_TICKRIGHT, &VtOrderGridConfig::OnBnClickedRadioTickright)
	ON_BN_CLICKED(IDC_CHECK_SHOW_PLCONFIGWND, &VtOrderGridConfig::OnBnClickedCheckShowPlconfigwnd)
	ON_BN_CLICKED(IDOK, &VtOrderGridConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &VtOrderGridConfig::OnBnClickedCancel)
	ON_BN_CLICKED(ID_APPLY, &VtOrderGridConfig::OnBnClickedApply)
	ON_BN_CLICKED(IDC_CHECK_SHOW_COUNTCOL, &VtOrderGridConfig::OnBnClickedCheckShowCountcol)
	ON_EN_CHANGE(IDC_EDIT_TICK_COUNT, &VtOrderGridConfig::OnEnChangeEditTickCount)
	ON_EN_CHANGE(IDC_EDIT_CELL_HEIGHT, &VtOrderGridConfig::OnEnChangeEditCellHeight)
	ON_BN_CLICKED(IDC_CHECK_FILTER_HOGA, &VtOrderGridConfig::OnBnClickedCheckFilterHoga)
END_MESSAGE_MAP()


// VtOrderGridConfig message handlers


void VtOrderGridConfig::OnBnClickedCheckShowOrdercol()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	bool flag = true;
	if (_CheckShowOrderCol.GetCheck() == BST_CHECKED) {
		flag = true;
	}
	else {
		flag = false;
	}
	_CenterWnd->ShowOrderAreaInGrid(flag);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		_CenterWnd->ParentDlg()->RefreshLayout(true, false, !flag);
	}
}


void VtOrderGridConfig::OnBnClickedCheckShowStopcol()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	bool flag = false;
	if (_CheckShowStopCol.GetCheck() == BST_CHECKED)
		flag = true;
	else
		flag = false;
	_CenterWnd->ShowStopAreaInGrid(flag);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		_CenterWnd->ParentDlg()->RefreshLayout(true, false, !flag);
	}
}

void VtOrderGridConfig::OnBnClickedCheckShowTickwnd()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	bool flag = false;
	if (_CheckShowTickWnd.GetCheck() == BST_CHECKED) {
		((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->EnableWindow(TRUE);
		flag = true;
		if (_CenterWnd->TickWndPos() == 1) {
			((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->SetCheck(BST_CHECKED);
			((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->SetCheck(BST_UNCHECKED);
		}
		else {
			((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->SetCheck(BST_UNCHECKED);
			((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->SetCheck(BST_CHECKED);
		}
	} else { 
		flag = false;
		((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->EnableWindow(FALSE);
	}
	_CenterWnd->ShowTickWnd(flag);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		_CenterWnd->ParentDlg()->RefreshLayout(true, false, !flag);
	}
}


void VtOrderGridConfig::OnBnClickedRadioTickleft()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	_CenterWnd->SetTickWndPos(1);
	_CenterWnd->ShowTickWnd(true);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		//_CenterWnd->ParentDlg()->RefreshLayout(true, false);
	}
}


void VtOrderGridConfig::OnBnClickedRadioTickright()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	_CenterWnd->SetTickWndPos(0);
	_CenterWnd->ShowTickWnd(true);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		//_CenterWnd->ParentDlg()->RefreshLayout(true, false);
	}
}


void VtOrderGridConfig::OnBnClickedCheckShowPlconfigwnd()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	if (_CheckShowPLConfigWnd.GetCheck() == BST_CHECKED)
		_CenterWnd->SetShowPLConfigWnd(true);
	else
		_CenterWnd->SetShowPLConfigWnd(false);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		_CenterWnd->ParentDlg()->RefreshLayout(true, true);
	}
}


void VtOrderGridConfig::OnBnClickedCheckShowCountcol()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	bool flag = false;
	if (_CheckShowCountCol.GetCheck() == BST_CHECKED)
		flag = true;
	else
		flag = false;
	_CenterWnd->ShowOrderCountInGrid(flag);
	if (_CenterWnd->ParentDlg()) {
		_CenterWnd->RefreshLayout(true);
		_CenterWnd->ParentDlg()->RefreshLayout(true, false, !flag);
	}
}



void VtOrderGridConfig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void VtOrderGridConfig::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void VtOrderGridConfig::OnBnClickedApply()
{
	// TODO: Add your control notification handler code here
	if (!_CenterWnd)
		return;
	CString strHeight;
	CString strWidth;
	_EditCellHeight.GetWindowText(strHeight);
	_EditCellWidth.GetWindowText(strWidth);
	_CenterWnd->SetOrderArea(_ttoi(strHeight), _ttoi(strWidth));
}


BOOL VtOrderGridConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (!_CenterWnd)
		return FALSE;

	VtOrderPanelGrid& grid = _CenterWnd->GetOrderPanelGrid();

	std::vector<bool>& gridColOptions = _CenterWnd->GetGridColOptions();
	
	if (gridColOptions[0])
		_CheckShowOrderCol.SetCheck(BST_CHECKED);
	else
		_CheckShowOrderCol.SetCheck(BST_UNCHECKED);
	if (gridColOptions[1])
		_CheckShowStopCol.SetCheck(BST_CHECKED);
	else
		_CheckShowStopCol.SetCheck(BST_UNCHECKED);
	if (gridColOptions[2])
		_CheckShowCountCol.SetCheck(BST_CHECKED);
	else
		_CheckShowCountCol.SetCheck(BST_UNCHECKED);
	if (grid.ShowOrderQuantityCol())
		_CheckShowPLConfigWnd.SetCheck(BST_CHECKED);
	else
		_CheckShowPLConfigWnd.SetCheck(BST_UNCHECKED);

	if (_CenterWnd->ShowTickWnd()) {
		((CButton*)GetDlgItem(IDC_CHECK_SHOW_TICKWND))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->EnableWindow(TRUE);
		if (_CenterWnd->TickWndPos() == 1)
			((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_CHECK_SHOW_TICKWND))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_TICKLEFT))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_TICKRIGHT))->EnableWindow(FALSE);
	}

	if (_CenterWnd->ShowRemainConfig()) {
		((CButton*)GetDlgItem(IDC_CHECK_SHOW_PLCONFIGWND))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_CHECK_SHOW_PLCONFIGWND))->SetCheck(BST_UNCHECKED);
	}

	if (_CenterWnd->UseHogaSiseFilter()) {
		_CheckFilterHoga.SetCheck(BST_CHECKED);
	}
	else {
		_CheckFilterHoga.SetCheck(BST_UNCHECKED);
	}

	// TODO:  Add extra initialization here
	CString text;
	text.Format(_T("%d"), _CenterWnd->GetCellHeight());
	_EditCellHeight.SetWindowText(text);
	text.Format(_T("%d"), _CenterWnd->GetOrderCellWidth());
	_EditCellWidth.SetWindowText(text);
	text.Format(_T("%d"), _CenterWnd->GetTickCount());
	_EditTickCount.SetWindowText(text);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtOrderGridConfig::OnEnChangeEditTickCount()
{
	// TODO:  Add your control notification handler code here
	if (_EditTickCount.GetSafeHwnd())
	{
		CString text;
		_EditTickCount.GetWindowText(text);
		if (_CenterWnd)
		{
			_CenterWnd->SetTickCount(_ttoi(text));
		}
	}
}


void VtOrderGridConfig::OnEnChangeEditCellHeight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void VtOrderGridConfig::OnBnClickedCheckFilterHoga()
{
	if (!_CenterWnd)
		return;
	bool flag = false;
	if (_CheckFilterHoga.GetCheck() == BST_CHECKED)
		flag = true;
	else
		flag = false;
	_CenterWnd->UseHogaSiseFilter(flag);
}

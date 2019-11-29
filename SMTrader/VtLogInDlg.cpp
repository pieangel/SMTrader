// VtLogInDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtLogInDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtHdClient.h"
#include "VtSaveManager.h"
#include <string>

extern TApplicationFont g_Font;
// VtLogInDlg dialog

IMPLEMENT_DYNAMIC(VtLogInDlg, CDialogEx)

VtLogInDlg::VtLogInDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_MAIN, pParent)
{

}

VtLogInDlg::~VtLogInDlg()
{
}

void VtLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_CERT, _EditId);
	DDX_Control(pDX, IDC_EDIT_ID, _EditID);
	DDX_Control(pDX, IDC_EDIT_CERT, _EditCert);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
	DDX_Control(pDX, IDC_STATIC_CERT, _StaticCert);
	DDX_Control(pDX, IDC_STATIC_ID, _StaticID);
	DDX_Control(pDX, IDC_STATIC_PWD, _StaticPwd);
	DDX_Control(pDX, IDC_BTN_LOGIN, _BtnSignIn);
	DDX_Control(pDX, IDC_BTN_CAN, _BtnCan);
}


BEGIN_MESSAGE_MAP(VtLogInDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_SAVE, &VtLogInDlg::OnBnClickedCheckSave)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &VtLogInDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CAN, &VtLogInDlg::OnBnClickedBtnCan)
END_MESSAGE_MAP()


// VtLogInDlg message handlers


void VtLogInDlg::OnBnClickedOk()
{
	_EditID.GetWindowText(id);
	_EditPwd.GetWindowText(pwd);
	_EditCert.GetWindowText(cert);
	VtHdClient* client = VtHdClient::GetInstance();
	int Ret = client->LogIn(id, pwd, cert);
	if (Ret <= 0) {
		/*
		-1	OnGetMsgWithRqId에서 반환되어지는 값 및 메시지 참조
			- 2	공인인증서 관련 오류
			- 3	정식로그인시도중 오류
			- 4	메모리 초기화 오류
			- 5	API버전오류
			- API사용 신청이 되었는지를 확인(모의ID인지 실장ID인지 확인요망)
			- 6	시스템 점검 오류
		*/
		CString msg;
		switch (Ret)
		{
		case -1:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("로그인오류!"));
			break;
		case -2:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("공인인증서오류!"));
			break;
		case -3:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("정식로그인시도중오류!"));
			break;
		case -4:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("메모리초기화오류!"));
			break;
		case -5:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("API버전오류!"));
			break;
		case -6:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("시스템점검오류!"));
			break;
		default:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("약식로그인오류!"));
			break;
		}
		AfxMessageBox(msg);
		return;
	}
	CDialogEx::OnOK();
}


void VtLogInDlg::OnBnClickedCheckSave()
{
	Save ? Save = false : Save = true;
}


BOOL VtLogInDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	VERIFY(_BrushBackNor.CreateSolidBrush(RGB(3, 30, 82)));

	_BtnCan.BorderShadow(RGB(34, 133, 212));
	_BtnCan.BorderLight(RGB(34, 133, 212));
	_BtnCan.DrawBorder(TRUE, TRUE);
	_BtnCan.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnCan.SetColor(BTNST_COLOR_BK_IN, RGB(3, 30, 82), true);
	_BtnCan.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
	_BtnCan.SetColor(BTNST_COLOR_BK_OUT, RGB(3, 30, 82), true);
	_BtnCan.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);

	_BtnSignIn.BorderShadow(RGB(34, 133, 212));
	_BtnSignIn.BorderLight(RGB(34, 133, 212));
	_BtnSignIn.DrawBorder(TRUE, TRUE);
	_BtnSignIn.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnSignIn.SetColor(BTNST_COLOR_BK_IN, RGB(3, 30, 82), true);
	_BtnSignIn.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
	_BtnSignIn.SetColor(BTNST_COLOR_BK_OUT, RGB(3, 30, 82), true);
	_BtnSignIn.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);

	_StaticID.SetTextColor(RGB(255, 255, 255));
	_StaticID.SetColor(RGB(3, 30, 82));
	_StaticID.SetGradientColor(RGB(3, 30, 82));

	_StaticPwd.SetTextColor(RGB(255, 255, 255));
	_StaticPwd.SetColor(RGB(3, 30, 82));
	_StaticPwd.SetGradientColor(RGB(3, 30, 82));

	_StaticCert.SetTextColor(RGB(255, 255, 255));
	_StaticCert.SetColor(RGB(3, 30, 82));
	_StaticCert.SetGradientColor(RGB(3, 30, 82));

	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	std::string sid, spwd, scert;
	//saveMgr->LoadLoginInfo(_T("SmTrader.cfg"), sid, spwd, scert, Save);
	int result = saveMgr->LoadLoginInfoFromXml(sid, spwd, scert, Save);
	if (result < 0) {
		_EditID.SetWindowText(_T(""));
		_EditPwd.SetWindowText(_T(""));
		_EditCert.SetWindowText(_T(""));
		((CButton*)GetDlgItem(IDC_CHECK_SAVE))->SetCheck(BST_UNCHECKED);
	}
	else {
		if (Save) {
			_EditID.SetWindowText(sid.c_str());
			_EditPwd.SetWindowText(spwd.c_str());
			_EditCert.SetWindowText(scert.c_str());
			((CButton*)GetDlgItem(IDC_CHECK_SAVE))->SetCheck(BST_CHECKED);
		}
		else {
			_EditID.SetWindowText(_T(""));
			_EditPwd.SetWindowText(_T(""));
			_EditCert.SetWindowText(_T(""));
			((CButton*)GetDlgItem(IDC_CHECK_SAVE))->SetCheck(BST_UNCHECKED);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL VtLogInDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	pDC->FillRect(&rcClient, &_BrushBackNor);
	return TRUE;
}


HBRUSH VtLogInDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_CHECK_SAVE)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		hbr = (HBRUSH)_BrushBackNor;
	}
	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)_BrushBackNor;
	}
	return hbr;
}


void VtLogInDlg::OnBnClickedBtnLogin()
{
	_EditID.GetWindowText(id);
	_EditPwd.GetWindowText(pwd);
	_EditCert.GetWindowText(cert);
	VtHdClient* client = VtHdClient::GetInstance();
	int Ret = client->LogIn(id, pwd, cert);
	if (Ret <= 0) {
		/*
		-1	OnGetMsgWithRqId에서 반환되어지는 값 및 메시지 참조
		- 2	공인인증서 관련 오류
		- 3	정식로그인시도중 오류
		- 4	메모리 초기화 오류
		- 5	API버전오류
		- API사용 신청이 되었는지를 확인(모의ID인지 실장ID인지 확인요망)
		- 6	시스템 점검 오류
		*/
		CString msg;
		switch (Ret)
		{
		case -1:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("로그인오류!"));
			break;
		case -2:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("공인인증서오류!"));
			break;
		case -3:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("정식로그인시도중오류!"));
			break;
		case -4:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("메모리초기화오류!"));
			break;
		case -5:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("API버전오류!"));
			break;
		case -6:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("시스템점검오류!"));
			break;
		default:
			msg.Format(_T("코드번호 : %d, 오류메시지 : %s"), Ret, _T("약식로그인오류!"));
			break;
		}
		AfxMessageBox(msg);
		CDialogEx::OnCancel();
		return;
	}
	CDialogEx::OnOK();
}


void VtLogInDlg::OnBnClickedBtnCancel()
{
	CDialogEx::OnOK();
}


void VtLogInDlg::OnBnClickedBtnCan()
{
	CDialogEx::OnCancel();
}

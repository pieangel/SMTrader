// VtLogInDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VtLogInDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtHdClient.h"
#include "VtSaveManager.h"
#include <string>

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
}


BEGIN_MESSAGE_MAP(VtLogInDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &VtLogInDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SAVE, &VtLogInDlg::OnBnClickedCheckSave)
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

	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	std::string sid, spwd, scert;
	saveMgr->LoadLoginInfo(_T("SmTrader.cfg"), sid, spwd, scert, Save);
	//sid = _T("yjs1974");
	//spwd = _T("3753yang");
	//scert = _T("#*sm1026jw");
#ifdef _DEBUG
	sid = _T("angelpie");
	spwd = _T("orion1");
	scert = _T("11orion@@@");
#endif
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

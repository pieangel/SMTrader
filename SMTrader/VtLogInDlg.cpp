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
		-1	OnGetMsgWithRqId���� ��ȯ�Ǿ����� �� �� �޽��� ����
			- 2	���������� ���� ����
			- 3	���ķα��νõ��� ����
			- 4	�޸� �ʱ�ȭ ����
			- 5	API��������
			- API��� ��û�� �Ǿ������� Ȯ��(����ID���� ����ID���� Ȯ�ο��)
			- 6	�ý��� ���� ����
		*/
		CString msg;
		switch (Ret)
		{
		case -1:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("�α��ο���!"));
			break;
		case -2:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("��������������!"));
			break;
		case -3:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("���ķα��νõ��߿���!"));
			break;
		case -4:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("�޸��ʱ�ȭ����!"));
			break;
		case -5:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("API��������!"));
			break;
		case -6:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("�ý������˿���!"));
			break;
		default:
			msg.Format(_T("�ڵ��ȣ : %d, �����޽��� : %s"), Ret, _T("��ķα��ο���!"));
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

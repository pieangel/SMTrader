// VtProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VtProgressDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtProductCategoryManager.h"
#include "HdScheduler.h"
#include "MainFrm.h"
// VtProgressDlg dialog



UINT threadfunc(LPVOID pp)
{
	VtProgressDlg* dlg = (VtProgressDlg*)pp;
	HdScheduler* taskMgr = HdScheduler::GetInstance();
	taskMgr->ProgressDlg(dlg);
	//Sleep(2000);
	//dlg->PostMessage(WM_CLOSE, 0, 0);
	return 0;
}

IMPLEMENT_DYNAMIC(VtProgressDlg, CDialogEx)

VtProgressDlg::VtProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROGRESS, pParent)
{

}

VtProgressDlg::~VtProgressDlg()
{
}

void VtProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATE, _StaticState);
	DDX_Control(pDX, IDC_PROGRESS_CTRL, _PrgCtrl);
	DDX_Control(pDX, IDC_STATIC_TASK, _StaticTask);
}


BEGIN_MESSAGE_MAP(VtProgressDlg, CDialogEx)
END_MESSAGE_MAP()


// VtProgressDlg message handlers


BOOL VtProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here
	//::AfxBeginThread((AFX_THREADPROC)threadfunc, (LPVOID)this);
	HdScheduler* taskMgr = HdScheduler::GetInstance();
	taskMgr->ProgressDlg(this);
	taskMgr->GetSymbolCode();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtProgressDlg::CloseDialog()
{
	if (MainFrm) {
		MainFrm->OnReceiveComplete();
	}
	EndDialog(1);
}

void VtProgressDlg::SetPrgressPos(int pos)
{
	_PrgCtrl.SetPos(pos);
}

void VtProgressDlg::SetTaskInfo(HdTaskInfo& taskInfo)
{
	if (taskInfo.argVec.size() == 0)
		return;

	_StaticTask.SetWindowText(taskInfo.TaskName.c_str());
	int index = taskInfo.TotalTaskCount - taskInfo.RemainTaskCount;
	if (index < taskInfo.argVec.size()) {
		std::pair<std::string, HdTaskArg> arg = taskInfo.argVec[index];
		_StaticState.SetWindowText(std::get<0>(arg).c_str());
	}
	double top = (double)taskInfo.TotalTaskCount - (double)taskInfo.RemainTaskCount;
	double entire = top / taskInfo.TotalTaskCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}


void VtProgressDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialogEx::PostNcDestroy();
}


void VtProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyWindow();
	CDialogEx::OnCancel();
}

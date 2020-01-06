// VtProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VtProgressDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtProductCategoryManager.h"
#include "HdScheduler.h"
#include "MainFrm.h"
#include "Market/SmSymbolReader.h"
#include "SmTaskManager.h"
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
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// VtProgressDlg message handlers


BOOL VtProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	VERIFY(_BrushBackNor.CreateSolidBrush(RGB(3, 30, 82)));
	// TODO:  Add extra initialization here
	//::AfxBeginThread((AFX_THREADPROC)threadfunc, (LPVOID)this);
	_StaticState.SetTextColor(RGB(255, 255, 255));
	_StaticState.SetColor(RGB(3, 30, 82));
	_StaticState.SetGradientColor(RGB(3, 30, 82));
	_StaticState.SetTextAlign(1);

	_StaticTask.SetTextColor(RGB(255, 255, 255));
	_StaticTask.SetColor(RGB(3, 30, 82));
	_StaticTask.SetGradientColor(RGB(3, 30, 82));
	_StaticTask.SetTextAlign(1);

	HdScheduler* taskMgr = HdScheduler::GetInstance();
	taskMgr->ProgressDlg(this);
	//taskMgr->GetSymbolCode();
	taskMgr->GetSymbolFile();

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


void VtProgressDlg::SetTaskInfo(std::string msg)
{
	_StaticState.SetWindowText(msg.c_str());
}

void VtProgressDlg::SetTaskInfo2(HdTaskInfo& taskInfo)
{
	_StaticTask.SetWindowText(taskInfo.TaskName.c_str());
	if (taskInfo.argMap.size() > 0) {
		std::shared_ptr<HdTaskArg> arg = taskInfo.argMap.begin()->second;
		_StaticState.SetWindowText(arg->info_text.c_str());
	}
	double top = (double)taskInfo.TotalTaskCount - (double)taskInfo.RemainTaskCount;
	double entire = top / taskInfo.TotalTaskCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}

void VtProgressDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	//delete this;
	CDialogEx::PostNcDestroy();
}


void VtProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyWindow();
	CDialogEx::OnCancel();
}


HBRUSH VtProgressDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hBrush = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATE || 
		pWnd->GetDlgCtrlID() == IDC_STATIC_TASK)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(3, 30, 82));
	}
	_StaticState.SetTextAlign(1);
	_StaticTask.SetTextAlign(1);
	hBrush = (HBRUSH)_BrushBackNor;
	return hBrush;
}

void VtProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	//SmTaskManager::GetInstance()->ExecuteTask();
	CDialogEx::OnTimer(nIDEvent);
}

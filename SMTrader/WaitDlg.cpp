// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cWaitDlg dialog


cWaitDlg::cWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(cWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(cWaitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   m_Text = "";
   m_bShowCancelButton = false;
}


void cWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cWaitDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cWaitDlg, CDialog)
	//{{AFX_MSG_MAP(cWaitDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cWaitDlg message handlers
void cWaitDlg::OnCancel() 
{
   //Override this so the user cannot cancel by hiting ESC
   return;
}

void cWaitDlg::OnOK() 
{
   //Ignore this unless the button is visible
   if (m_bShowCancelButton)
      {
	   m_Thread->m_Event->SetEvent();
	   CDialog::OnOK();
      }
}

BOOL cWaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   if (m_Text.GetLength() > 0)
      GetDlgItem(IDC_BUSY_TEXT)->SetWindowText(m_Text);

   GetDlgItem(IDOK)->ShowWindow(m_bShowCancelButton ? SW_SHOW : SW_HIDE);

   CRect rect;
   GetDlgItem(IDC_BUSY_PIC)->GetWindowRect(rect);
   ScreenToClient(&rect);

   m_Anim.Create(WS_VISIBLE|WS_CHILD, rect, this, IDC_BUSY_PIC);
   m_Anim.SetImageList(IDB_CALC_ANIM2, 70, 56, RGB(255,0,255));
   m_Anim.StartAnim(250);

	SetTimer(1,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void cWaitDlg::OnTimer(UINT nIDEvent) 
{
	CSingleLock lock(m_Thread->m_Event, FALSE);

   if (lock.Lock(0))
      {
      CDialog::OnOK();
      }

	//CDialog::OnTimer(nIDEvent);
}

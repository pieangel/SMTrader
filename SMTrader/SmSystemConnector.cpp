// SmSystemConnector.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "SmSystemConnector.h"
#include "afxdialogex.h"


// SmSystemConnector dialog

IMPLEMENT_DYNAMIC(SmSystemConnector, CDialogEx)

SmSystemConnector::SmSystemConnector(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYS_CONNECTOR, pParent)
{

}

SmSystemConnector::~SmSystemConnector()
{
}

void SmSystemConnector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SmSystemConnector, CDialogEx)
	ON_NOTIFY(SPN_MAXMINPOS, IDC_SPLITTER_HOR, OnMaxMinInfo)
	ON_NOTIFY(SPN_MAXMINPOS, IDC_SPLITTER_VER, OnMaxMinInfo)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// SmSystemConnector message handlers


BOOL SmSystemConnector::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_SPLITTER_HOR);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	BOOL bRet = m_wndSplitterHor.Create(WS_CHILD | WS_VISIBLE, rc,
		this, IDC_SPLITTER_HOR, SPS_VERTICAL | SPS_DELTA_NOTIFY);//, RGB(255, 0, 0));
	if (FALSE == bRet)
	{
		AfxMessageBox("m_wndSplitterHor create failed");
	}

	pWnd = GetDlgItem(IDC_SPLITTER_VER);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	bRet = m_wndSplitterVer.Create(WS_CHILD | WS_VISIBLE, rc,
		this, IDC_SPLITTER_VER, SPS_HORIZONTAL, RGB(0, 0, 255));
	if (FALSE == bRet)
	{
		AfxMessageBox("m_wndSplitterVer create failed");
	}

	//  register windows for splitter
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_STATIC_RUN_LIST));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_CHECK_AUTO_ORDER));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_CHECK_DETAIL));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_BUTTON_SUM_SIGNAL));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_BUTTON_LIQ_POSITION));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_BTN_ORDER_CONFIG));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_UP, GetDlgItem(IDC_STATIC_TOTAL_SIGNAL));

	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, &m_wndSplitterVer);
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_STATIC_CONNECT_SIGNAL_CHART));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_CHECK_ALL));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_BTN_ADD_CONNECT));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_BTN_DEL_CONNECT));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_STATIC_SIGNAL_CONNECTION));

	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_STATIC_SIGNAL_CHART));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_BTN_ADD_SIGNAL));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_BTN_DEL_SIGNAL));
	this->m_wndSplitterHor.RegisterLinkedWindow(SPLS_LINKED_DOWN, GetDlgItem(IDC_STATIC_SIGNAL_DEFINITION));
	
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_LEFT, GetDlgItem(IDC_STATIC_CONNECT_SIGNAL_CHART));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_LEFT, GetDlgItem(IDC_CHECK_ALL));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_LEFT, GetDlgItem(IDC_BTN_ADD_CONNECT));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_LEFT, GetDlgItem(IDC_BTN_DEL_CONNECT));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_LEFT, GetDlgItem(IDC_STATIC_SIGNAL_CONNECTION));

	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_RIGHT, GetDlgItem(IDC_STATIC_SIGNAL_CHART));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_RIGHT, GetDlgItem(IDC_BTN_ADD_SIGNAL));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_RIGHT, GetDlgItem(IDC_BTN_DEL_SIGNAL));
	this->m_wndSplitterVer.RegisterLinkedWindow(SPLS_LINKED_RIGHT, GetDlgItem(IDC_STATIC_SIGNAL_DEFINITION));

	//  relayout the splotter to make them good look
	this->m_wndSplitterHor.Relayout();
	this->m_wndSplitterVer.Relayout();

	this->Resize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmSystemConnector::OnMaxMinInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	SPC_NM_MAXMINPOS* pNewMaxMinPos = (SPC_NM_MAXMINPOS*)pNMHDR;
	if (IDC_SPLITTER_HOR == pNMHDR->idFrom)
	{
		pNewMaxMinPos->lMin = 50;
		pNewMaxMinPos->lMax = 50 + STD_GAP;
	}
	else
	{
		pNewMaxMinPos->lMin = 50;
		pNewMaxMinPos->lMax = 100;
	}
}

void SmSystemConnector::Resize()
{
	// 여기서 수평 스플리터의 위치를 잡는다.
	if (FALSE == IsWindow(m_wndSplitterHor.GetSafeHwnd()))
	{
		return;
	}

	CRect rcDlg;
	this->GetClientRect(rcDlg);

	int horizontal_position = (rcDlg.Height() * 2) / 3;
	int vertical_position = rcDlg.Width() / 2;

	// 수직 스플리터의 위치를 잡는다.
	CRect rcSplitVer;
	m_wndSplitterVer.GetWindowRect(rcSplitVer);
	this->ScreenToClient(rcSplitVer);
	rcSplitVer.bottom = rcDlg.bottom - STD_GAP;
	this->m_wndSplitterVer.MoveWindow(rcSplitVer, TRUE);

	

	// 수평 스플리터의 위치를 잡는다.
	CRect rcSplitHor;
	m_wndSplitterHor.GetWindowRect(rcSplitHor);
	this->ScreenToClient(rcSplitHor);

	int controlHeight = 32;
	
	// 위쪽 타이틀
	CRect rcCtrl;
	CWnd* pWnd = this->GetDlgItem(IDC_STATIC_RUN_LIST);
	
	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 주문설정 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ORDER_CONFIG);
	
	int left = rcDlg.right - STD_GAP - 100;
	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 포지션 청산 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_LIQ_POSITION);
	
	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 신호 합계 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_SUM_SIGNAL);
	

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 상세표시 체크 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_DETAIL);
	

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 자동주문 체크 표시 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_AUTO_ORDER);
	
	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 실행 목록 그리드
	pWnd = this->GetDlgItem(IDC_STATIC_TOTAL_SIGNAL);

	
	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = rcDlg.top + STD_GAP + controlHeight;
	rcCtrl.bottom = horizontal_position - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	
	rcSplitHor.right = rcDlg.right - STD_GAP;
	this->m_wndSplitterHor.MoveWindow(rcSplitHor, TRUE);

	// 분할 왼쪽 타이틀
	rcCtrl;
	pWnd = this->GetDlgItem(IDC_STATIC_CONNECT_SIGNAL_CHART);

	int top = horizontal_position + STD_GAP;
	rcCtrl.top = top + 7;
	rcCtrl.bottom =rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.right;
	
	// 모두 선택 버튼 
	pWnd = this->GetDlgItem(IDC_CHECK_ALL);
	rcCtrl.top = top + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 삭제 버튼 
	pWnd = this->GetDlgItem(IDC_BTN_DEL_CONNECT);
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;

	left = vertical_position - STD_GAP - 100;

	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	
	// 왼쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_CONNECT);
	
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_CONNECTION);
	

	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = vertical_position - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);

	


	// 오른쪽 타이틀
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_CHART);
	
	rcCtrl.top = top + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 삭제 버튼
	pWnd = this->GetDlgItem(IDC_BTN_DEL_SIGNAL);
	
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.right - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	left = rcDlg.right - STD_GAP - 100;

	// 오른쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_SIGNAL);
	
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_DEFINITION);
	

	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);

	Invalidate(TRUE);
}

void SmSystemConnector::MoveDlgItem(int nID, const CRect& rcPos, BOOL bRepaint)
{
	CWnd* pWnd = this->GetDlgItem(nID);
	if (NULL == pWnd)
	{
		return;
	}

	pWnd->MoveWindow(rcPos, bRepaint);

	CRect rcsp;
	m_wndSplitterHor.GetWindowRect(rcsp);
	this->ScreenToClient(rcsp);
}

void SmSystemConnector::OnSplitter1Delta(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	SPC_NM_DELTA* pDelta = (SPC_NM_DELTA*)pNMHDR;
	if (NULL == pDelta)
	{
		return;
	}

	//m_wndSplitterHor.ChangePos(&m_edHelp, SPLS_LINKED_LEFT, pDelta->lDelta);
}


void SmSystemConnector::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	this->Resize();
}

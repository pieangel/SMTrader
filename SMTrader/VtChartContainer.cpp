// VtChartContainer.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtChartContainer.h"
#include "afxdialogex.h"
#include "VtChartCtrlManager.h"
#include "resource.h"
#include "VtSymbol.h"
#include "VtChartWindowManager.h"
#include "VtSymbolManager.h"
#include "VtGlobal.h"
#include "VtChartDataManager.h"
#include "VtChartFrm.h"
#include "VtChartWindow.h"
#include "MainFrm.h"
#include "VtWindowManager.h"
#include "VtTaskManager.h"
#include "VtTaskArg.h"
#include "ZmConfigManager.h"
#include "VtSplitterManager.h"
#include <mmsystem.h>
#include "MainFrm.h"
#include "VtChartContainerManager.h"
#include "HdWindowManager.h"
#include "System/VtSystemManager.h"
#include "Poco/Delegate.h"
#include "SmCallbackManager.h"
using Poco::Delegate;


// VtChartContainer dialog

using namespace std;
using namespace std::placeholders;

void VtChartContainer::RegisterChartCallback()
{
	SmCallbackManager::GetInstance()->SubscribeChartCallback((long)this, std::bind(&VtChartContainer::OnChartEvent, this, _1));
}

void VtChartContainer::UnregisterChartCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeChartCallback((int)this);
}

void VtChartContainer::OnChartEvent(const SmChartData* chart_data)
{
	if (!chart_data)
		return;
}


void VtChartContainer::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&VtChartContainer::OnQuoteEvent, this, _1));
}

void VtChartContainer::UnregisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void VtChartContainer::OnQuoteEvent(const VtSymbol* symbol)
{
	if (!symbol)
		return;
}


IMPLEMENT_DYNAMIC(VtChartContainer, CDialogEx)

VtChartContainer::VtChartContainer(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHART_CONTAINER, pParent)
{
	_ActiveChartWnd = nullptr;
	_MainWnd = nullptr;
	_CurMode = -1;
	_GridWidth = 200;
	_ToolBarHeight = 25;

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	sysMgr->CreateBundleSystem();
}

VtChartContainer::~VtChartContainer()
{
	// 반드시 리소스 해제할것
	for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it) {
		(*it)->UnregisterAllCallback();
		delete *it;
	}
	ClearSplitters();

	VtChartContainerManager* wndMgr = VtChartContainerManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &VtChartContainerManager::OnWindowEvent);
}

void VtChartContainer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtChartContainer, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_MOUSEHWHEEL()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// VtChartContainer message handlers


void VtChartContainer::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ClearEvent();
	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::ChartWindow;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
}


void VtChartContainer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_MainWnd) {
		CRect rect;
		GetClientRect(&rect);
		CRect rcGrid;
		rcGrid.left = rect.Width() - _GridWidth;
		rcGrid.right = rect.right;
		rcGrid.top = rect.top;
		rcGrid.bottom = rect.bottom;

		if (_TimeToolBar.GetSafeHwnd()) {
			CRect rcToolBar;
			_TimeToolBar.GetWindowRect(rcToolBar);
			_ToolBarHeight = rcToolBar.Height();
		}
		rect.top += _ToolBarHeight;
		rect.right = rect.right - _GridWidth;
		_MainWnd->MoveWindow(&rect);

		CWnd* gridWnd = (CWnd*)GetDlgItem(IDC_STATIC_GRID);
		if (gridWnd)
			gridWnd->MoveWindow(rcGrid);
		_FavGrid.ResizeColWidth(rcGrid.Width());
	}

	for (int i = 0; i < _ChartList.size(); i++) {
		_ChartList[i]->Invalidate(TRUE);
	}
}


BOOL VtChartContainer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);
	
	_TimeToolBar.Create(IDD_TIME_TOOLBAR, this);
	_TimeToolBar.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOSIZE);
	_TimeToolBar.ShowWindow(SW_SHOW);
	_TimeToolBar.Container(this);
	// TODO:  Add extra initialization here
	_FavGrid.AttachGrid(this, IDC_STATIC_GRID);
	_FavGrid.ChartContainer(this);
	AddChartOneByOne();

	if (_ShowGrid) {
		_GridWidth = 200;
		_FavGrid.ShowWindow(SW_SHOW);
	}
	else {
		_GridWidth = 0;
		_FavGrid.ShowWindow(SW_HIDE);
	}

	SendMessage(WM_SIZE);

	if (_ActiveChartWnd) {
		_FavGrid.SetChartData(_ActiveChartWnd);
	}

	InitSystem();
	
	VtChartContainerManager* wndMgr = VtChartContainerManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &VtChartContainerManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::ChartWindow;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));

	RegisterChartCallback();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtChartContainer::ChangeChartStyle(SmChartStyle style)
{
	if (_ActiveChartWnd) {
		_ActiveChartWnd->ChangeChartStyle(style);
	}
}

void VtChartContainer::ChangeChartData(VtSymbol* symbol, SmChartType chart_type, int cycle)
{
	if (!symbol)
		return;
	if (_ActiveChartWnd) {
		_ActiveChartWnd->ChangeChartData(symbol, chart_type, cycle);
		_FavGrid.SetChartData(_ActiveChartWnd);
	}
}

void VtChartContainer::Save(simple::file_ostream<same_endian_type>& ss)
{

}

void VtChartContainer::Load(simple::file_istream<same_endian_type>& ss)
{

}

void VtChartContainer::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (_ActiveChartWnd)
	{
		_ActiveChartWnd->OnMouseHWheel(nFlags, zDelta, pt);
	}

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}

void VtChartContainer::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;
	_FavGrid.OnReceiveChartData(sym->ShortCode);
}

void VtChartContainer::OnReceiveChartData(VtChartData* data)
{
	if (!data)
		return;
	SetDateTime(data);
	_FavGrid.OnReceiveChartData(data->SymbolCode());
}

void VtChartContainer::SetSelectedChartData(std::string selectedCode)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->SetSelectedChartData(selectedCode);
}

void VtChartContainer::ChangeChartTime(int newTime)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->ChangeChartTime(newTime);
}

void VtChartContainer::OnChartPattern(UINT id)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->OnChartPattern(id);
}

void VtChartContainer::OnChartIndex(UINT id)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->OnChartIndex(id);
}

void VtChartContainer::OnChartCompare(UINT id)
{
	;
}


void VtChartContainer::InsertIndex(CMFCPropertyGridProperty* prop)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->InsertIndex(prop);
}

void VtChartContainer::AddChart(int mode)
{
	if (_CurMode == mode)
		return;

	if (_CurMode == 5)
	{
		if (mode < _CurMode)
		{
			AfxMessageBox(_T("레이아웃을 변경할 수 없습니다."));
			return;
		}
	}

	if (_CurMode == 3 || _CurMode == 4)
	{
		if (mode == 1 || mode == 2)
		{
			AfxMessageBox(_T("레이아웃을 변경할 수 없습니다."));
			return;
		}
	}
	if (mode == 0)
		AddChartOneByOne();
	else if (mode == 1)
		AddChartOneByTwo();
	else if (mode == 2)
		AddChartTwoByOne();
	else if (mode == 3)
		AddChartTwoByTwo();
	else if (mode == 4)
		AddChartFourByOne();
	else if (mode == 5)
		AddChartFourByTwo();

	_CurMode = mode;

	if (_MainWnd)
		_MainWnd->SendMessage(WM_SIZE);
}

void VtChartContainer::AddChart(std::vector<std::pair<std::string, VtMainChartType>>& keyList, int mode)
{
	if (mode == 1)
		AddChartOneByTwo(keyList);
	else if (mode == 2)
		AddChartTwoByOne(keyList);
	else if (mode == 3)
		AddChartTwoByTwo(keyList);
	else if (mode == 4)
		AddChartFourByOne(keyList);
	else if (mode == 5)
		AddChartFourByTwo(keyList);

	_CurMode = mode;
}

VtChartWindow* VtChartContainer::AddChart()
{
// 	VtChartCtrlManager* chartCtrlMgr = VtChartCtrlManager::GetInstance();
// 	VtChartWindow* chart = chartCtrlMgr->GetChartCtrl(this);
// 	chart->ParentView(this);
// 	_ChartList.push_back(chart);
// 	_ChartSet.insert(chart);
// 
// 	return chart;
	return nullptr;
}

void VtChartContainer::ClearSplitters()
{
	for (auto it = _SplitVector.begin(); it != _SplitVector.end();)
	{
		CSSplitter* splitter = *it;
		it = _SplitVector.erase(it);
		if (splitter->GetSafeHwnd())
		{
			if (splitter->WndLeft())
				splitter->WndLeft()->SetParent(this);
			if (splitter->WndRight())
				splitter->WndRight()->SetParent(this);

			if (splitter->WndTop())
				splitter->WndTop()->SetParent(this);
			if (splitter->WndBottom())
				splitter->WndBottom()->SetParent(this);

		}
		delete splitter;
	}
}


void VtChartContainer::StartByOne()
{
	VtGlobal* global = VtGlobal::GetInstance();
	VtChartCtrlManager* chartCtrlMgr = VtChartCtrlManager::GetInstance();
	std::string key = VtChartDataManager::MakeChartDataKey(global->SelExchange(), global->SelProduct(), global->SelType(), global->SelCycle());
	VtChartWindow* chart1 = chartCtrlMgr->GetChartCtrl(this);
	VtChartWindowManager* chartWndMgr = VtChartWindowManager::GetInstance();
	chartWndMgr->AddChartWindow(key, chart1);
	_ActiveChartWnd = chart1;
	_MainWnd = (CWnd*)chart1;
	chart1->ParentView(this);
	chart1->MainChartType(VtMainChartType::CandleStick);

	_ChartList.push_back(chart1);
	SendMessage(WM_SIZE);


// 	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
// 	VtTaskChartDataArg* arg = new VtTaskChartDataArg(VtTaskType::VtTaskChartData);
// 	VtChartDataRequest req = global->ChartReqArray[0];
// 	arg->req = req;
// 	taskMgr->AddTask(arg);
// 	taskMgr->ReqCount(1);
// 	taskMgr->StartTask();
}

void VtChartContainer::RefreshSplitters()
{
	for (auto it = _SplitVector.begin(); it != _SplitVector.end();)
	{
		CSSplitter* splitter = *it;
		splitter->SendMessage(WM_SIZE);
	}
}

void VtChartContainer::StartByOneByTwo()
{
// 	VtChartCtrlManager* chartCtrlMgr = VtChartCtrlManager::GetInstance();
// 	for (int i = 0; i < 2; i++)
// 	{
// 		VtChartWindow* chart = chartCtrlMgr->GetChartCtrl(this);
// 		chart->MainChartType(VtMainChartType::ClosePrice);
// 		chart->ParentView(this);
// 		_ChartList.push_back(chart);
// 		_ChartSet.insert(chart);
// 	}
// 
// 	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
// 
// 	CMainFrame* mainFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRect rect;
// 	CRect rcMain;
// 	mainFrm->GetClientRect(rcMain);
// 
// 	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
// 	CWnd* wnd = wndMgr->FindWindow(_T("BulletinWnd"));
// 	CRect rcPane;
// 	wnd->GetClientRect(rcPane);
// 
// 	rect.left = 0;
// 	rect.top = 0;
// 	rect.right = rcMain.Width() - rcPane.Width();
// 	rect.bottom = rcMain.Height();
// 
// 	CSSplitter* splitMain = new CSSplitter();
// 	_MainWnd = (CWnd*)splitMain;
// 	_SplitVector.push_back(splitMain);
// 	splitMain->SplitType = VtSplitType::SplitVertical;
// 	splitMain->Create(
// 		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
// 		this,				// the parent of the splitter pane
// 		_ChartList[0],		// top pane
// 		_ChartList[1],		// bottom pane
// 		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		rect,				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);
// 
// 
// 	_ChartList[0]->ParentSplit = splitMain;
// 	_ChartList[1]->ParentSplit = splitMain;
// 	_ChartList[0]->AttachDirection(VtAttachDirection::AttachLeft);
// 	_ChartList[1]->AttachDirection(VtAttachDirection::AttachRight);
// 
// 	_ActiveChartWnd = _ChartList[0];
// 
// 	for (int i = 0; i < _ChartList.size(); i++)
// 	{
// 		_ChartList[i]->Invalidate(TRUE);
// 	}
// 
// 	VtGlobal* global = VtGlobal::GetInstance();
// 	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
// 	for (int i = 0; i < DefaultReqCount; i++)
// 	{
// 		VtTaskChartDataArg* arg = new VtTaskChartDataArg(VtTaskType::VtTaskChartData);
// 		VtChartDataRequest req = global->ChartReqArray[i];
// 		std::string newKey = VtChartDataManager::MakeChartDataKey(req.exchange, req.productCode, req.chartType, req.cycle);
// 		VtChartWindowManager* chartWndMgr = VtChartWindowManager::GetInstance();
// 		if (i == 0)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[0]);
// 		else if (i == DefaultReqCount / 2)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[1]);
// 		else if (i < DefaultReqCount / 2)
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[0]);
// 		else
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[1]);
// 
// 		arg->req = req;
// 		taskMgr->AddTask(arg);
// 	}
// 	taskMgr->ReqCount(DefaultReqCount);
// 	taskMgr->StartTask();
// 
// 	mainFrm->Invalidate(TRUE);
}

void VtChartContainer::StartByTwoByTwo()
{
// 	VtChartCtrlManager* chartCtrlMgr = VtChartCtrlManager::GetInstance();
// 	for (int i = 0; i < 4; i++)
// 	{
// 		VtChartWindow* chart = chartCtrlMgr->GetChartCtrl(this);
// 		chart->ParentView(this);
// 		_ChartList.push_back(chart);
// 		_ChartSet.insert(chart);
// 	}
// 
// 	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
// 	//CRect rect;
// 	//GetClientRect(rect);
// 	CMainFrame* mainFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRect rect;
// 	CRect rcMain;
// 	mainFrm->GetClientRect(rcMain);
// 	CMDIChildWnd *pChild = (CMDIChildWnd *)((CMainFrame*)mainFrm)->GetActiveFrame();
// 	pChild->GetClientRect(rcMain);
// 	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
// 	CWnd* wnd = wndMgr->FindWindow(_T("BulletinWnd"));
// 	CRect rcPane;
// 	wnd->GetClientRect(rcPane);
// 
// 	rect.left = rcMain.left;
// 	rect.top = rcMain.top;
// 	rect.right = rcMain.Width() - rcPane.Width();
// 	rect.bottom = rcPane.Height();
// 
// 	CSSplitter* splitMain = new CSSplitter();
// 	_MainWnd = (CWnd*)splitMain;
// 	splitMain->SplitType = VtSplitType::SplitVertical;
// 	_SplitVector.push_back(splitMain);
// 
// 	CSSplitter* splitLeft = new CSSplitter();
// 	splitLeft->ParentSplit = splitMain;
// 	splitLeft->SplitType = VtSplitType::SplitHorizontal;
// 	splitLeft->AttachDirection(VtAttachDirection::AttachLeft);
// 	_SplitVector.push_back(splitLeft);
// 
// 	CSSplitter* splitRight = new CSSplitter();
// 	splitRight->ParentSplit = splitMain;
// 	splitRight->SplitType = VtSplitType::SplitHorizontal;
// 	splitRight->AttachDirection(VtAttachDirection::AttachRight);
// 	_SplitVector.push_back(splitRight);
// 
// 	CRect rcLeft, rcRight;
// 
// 	rcLeft.left = rect.left;
// 	rcLeft.top = rect.top;
// 	rcLeft.right = rect.Width() / 2;
// 	rcLeft.bottom = rect.bottom;
// 
// 	rcRight.left = rect.left + rect.Width() / 2;
// 	rcRight.top = rect.top;
// 	rcRight.right = rect.right;
// 	rcRight.bottom = rect.bottom;
// 
// 	splitLeft->Create(
// 		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
// 		this,				// the parent of the splitter pane
// 		_ChartList[0],		// top pane
// 		_ChartList[1],		// bottom pane
// 		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		rcLeft,				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);
// 
// 	_ChartList[0]->ParentSplit = splitLeft;
// 	_ChartList[1]->ParentSplit = splitLeft;
// 	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
// 	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);
// 
// 	splitRight->Create(
// 		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
// 		this,				// the parent of the splitter pane
// 		_ChartList[2],		// top pane
// 		_ChartList[3],		// bottom pane
// 		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		rcRight,				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);
// 
// 	_ChartList[2]->ParentSplit = splitRight;
// 	_ChartList[3]->ParentSplit = splitRight;
// 	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
// 	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);
// 
// 	splitMain->Create(
// 		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
// 		this,				// the parent of the splitter pane
// 		splitLeft,		// top pane
// 		splitRight,		// bottom pane
// 		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
// 							// position and therefore it must be unique 
// 							// within your application
// 		rect,				// dimensions of the splitter pane
// 		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
// 		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
// 	);
// 	_ActiveChartWnd = _ChartList[0];
// 
// 	VtGlobal* global = VtGlobal::GetInstance();
// 	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
// 	for (int i = 0; i < DefaultReqCount; i++)
// 	{
// 		VtTaskChartDataArg* arg = new VtTaskChartDataArg(VtTaskType::VtTaskChartData);
// 		VtChartDataRequest req = global->ChartReqArray[i];
// 		std::string newKey = VtChartDataManager::MakeChartDataKey(req.exchange, req.productCode, req.chartType, req.cycle);
// 		VtChartWindowManager* chartWndMgr = VtChartWindowManager::GetInstance();
// 		if (i == 0)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[0]);
// 		else if (i == 1)
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[0]);
// 		else if (i == 2)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[1]);
// 		else if (i < DefaultReqCount / 2)
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[1]);
// 		else if (i == DefaultReqCount / 2)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[2]);
// 		else if (i == (DefaultReqCount / 2) + 1)
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[2]);
// 		else if (i == (DefaultReqCount / 2) + 2)
// 			chartWndMgr->AddChartWindow(newKey, _ChartList[3]);
// 		else
// 			chartWndMgr->AddRefChartWindow(newKey, _ChartList[3]);
// 
// 		arg->req = req;
// 		taskMgr->AddTask(arg);
// 	}
// 	taskMgr->StartTask();
// 
// 	for (int i = 0; i < _ChartList.size(); i++)
// 	{
// 		_ChartList[i]->MainChartType(VtMainChartType::ClosePrice);
// 		_ChartList[i]->UseMainAxis(true);
// 	}
// 
// 	splitMain->Invalidate(TRUE);
}

void VtChartContainer::AddChartOneByOne()
{
	VtChartWindow* chart1 = CreateChartWindow();
	_ActiveChartWnd = chart1;
	_MainWnd = (CWnd*)chart1;
	chart1->ParentView(this);
	chart1->MainChartType(VtMainChartType::CandleStick);

	_ChartList.push_back(chart1);
}

void VtChartContainer::AddChartOneByTwo()
{
	for (int i = 0; i < 2; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	_SplitVector.push_back(splitMain);
	splitMain->SplitType = VtSplitType::SplitVertical;
	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);


	_ChartList[0]->ParentSplit = splitMain;
	_ChartList[1]->ParentSplit = splitMain;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachLeft);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachRight);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartOneByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList)
{
	for (int i = 0; i < 2; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	_SplitVector.push_back(splitMain);
	splitMain->SplitType = VtSplitType::SplitVertical;
	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);


	_ChartList[0]->ParentSplit = splitMain;
	_ChartList[1]->ParentSplit = splitMain;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachLeft);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachRight);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartTwoByOne()
{
	
	for (int i = 0; i < 2; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	_SplitVector.push_back(splitMain);
	splitMain->SplitType = VtSplitType::SplitHorizontal;
	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);
	_ChartList[0]->ParentSplit = splitMain;
	_ChartList[1]->ParentSplit = splitMain;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);
	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartTwoByOne(std::vector<std::pair<std::string, VtMainChartType>>& keyList)
{
	for (int i = 0; i < 2; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	_SplitVector.push_back(splitMain);
	splitMain->SplitType = VtSplitType::SplitHorizontal;
	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);
	_ChartList[0]->ParentSplit = splitMain;
	_ChartList[1]->ParentSplit = splitMain;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);
	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartTwoByTwo()
{
	for (int i = 0; i < 4; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitVertical;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitLeft = new CSSplitter();
	splitLeft->ParentSplit = splitMain;
	splitLeft->SplitType = VtSplitType::SplitHorizontal;
	splitLeft->AttachDirection(VtAttachDirection::AttachLeft);
	_SplitVector.push_back(splitLeft);

	CSSplitter* splitRight = new CSSplitter();
	splitRight->ParentSplit = splitMain;
	splitRight->SplitType = VtSplitType::SplitHorizontal;
	splitRight->AttachDirection(VtAttachDirection::AttachRight);
	_SplitVector.push_back(splitRight);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.Width() / 2;
	rcLeft.bottom = rect.bottom;

	rcRight.left = rect.left + rect.Width() / 2;
	rcRight.top = rect.top;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	splitLeft->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitLeft;
	_ChartList[1]->ParentSplit = splitLeft;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRight->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[2]->ParentSplit = splitRight;
	_ChartList[3]->ParentSplit = splitRight;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		splitLeft,		// top pane
		splitRight,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);
	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartTwoByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList)
{
	for (int i = 0; i < 4; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitVertical;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitLeft = new CSSplitter();
	splitLeft->ParentSplit = splitMain;
	splitLeft->SplitType = VtSplitType::SplitHorizontal;
	splitLeft->AttachDirection(VtAttachDirection::AttachLeft);
	_SplitVector.push_back(splitLeft);

	CSSplitter* splitRight = new CSSplitter();
	splitRight->ParentSplit = splitMain;
	splitRight->SplitType = VtSplitType::SplitHorizontal;
	splitRight->AttachDirection(VtAttachDirection::AttachRight);
	_SplitVector.push_back(splitRight);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.Width() / 2;
	rcLeft.bottom = rect.bottom;

	rcRight.left = rect.left + rect.Width() / 2;
	rcRight.top = rect.top;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	splitLeft->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitLeft;
	_ChartList[1]->ParentSplit = splitLeft;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRight->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[2]->ParentSplit = splitRight;
	_ChartList[3]->ParentSplit = splitRight;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		splitLeft,		// top pane
		splitRight,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);
	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartFourByOne()
{
	for (int i = 0; i < 4; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitHorizontal;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitTop = new CSSplitter();
	splitTop->ParentSplit = splitMain;
	splitTop->SplitType = VtSplitType::SplitHorizontal;
	splitTop->AttachDirection(VtAttachDirection::AttachTop);
	_SplitVector.push_back(splitTop);
	CSSplitter* splitBottom = new CSSplitter();
	splitBottom->ParentSplit = splitMain;
	splitBottom->SplitType = VtSplitType::SplitHorizontal;
	splitBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitBottom);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.Width();
	rcLeft.bottom = rect.top + rect.Height() / 2;

	rcRight.left = rect.left;
	rcRight.top = rect.top + rect.Height() / 2;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	splitTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitTop;
	_ChartList[1]->ParentSplit = splitTop;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);
	splitBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[2]->ParentSplit = splitBottom;
	_ChartList[3]->ParentSplit = splitBottom;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitTop,		// top pane
		splitBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartFourByOne(std::vector<std::pair<std::string, VtMainChartType>>& keyList)
{
	for (int i = 0; i < 4; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->ParentView(this);
		chart->MainChartType(VtMainChartType::CandleStick);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitHorizontal;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitTop = new CSSplitter();
	splitTop->ParentSplit = splitMain;
	splitTop->SplitType = VtSplitType::SplitHorizontal;
	splitTop->AttachDirection(VtAttachDirection::AttachTop);
	_SplitVector.push_back(splitTop);
	CSSplitter* splitBottom = new CSSplitter();
	splitBottom->ParentSplit = splitMain;
	splitBottom->SplitType = VtSplitType::SplitHorizontal;
	splitBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitBottom);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.Width();
	rcLeft.bottom = rect.top + rect.Height() / 2;

	rcRight.left = rect.left;
	rcRight.top = rect.top + rect.Height() / 2;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	splitTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitTop;
	_ChartList[1]->ParentSplit = splitTop;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);
	splitBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[2]->ParentSplit = splitBottom;
	_ChartList[3]->ParentSplit = splitBottom;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitTop,		// top pane
		splitBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartFourByTwo()
{
	for (int i = 0; i < 8; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->MainChartType(VtMainChartType::CandleStick);
		chart->ParentView(this);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitVertical;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitLeft = new CSSplitter();
	splitLeft->ParentSplit = splitMain;
	splitLeft->SplitType = VtSplitType::SplitHorizontal;
	splitLeft->AttachDirection(VtAttachDirection::AttachLeft);
	_SplitVector.push_back(splitLeft);
	CSSplitter* splitRight = new CSSplitter();
	splitRight->ParentSplit = splitMain;
	splitRight->SplitType = VtSplitType::SplitHorizontal;
	splitRight->AttachDirection(VtAttachDirection::AttachRight);
	_SplitVector.push_back(splitRight);

	CSSplitter* splitLeftTop = new CSSplitter();
	_SplitVector.push_back(splitLeftTop);
	splitLeftTop->ParentSplit = splitLeft;
	splitLeftTop->SplitType = VtSplitType::SplitHorizontal;
	splitLeftTop->AttachDirection(VtAttachDirection::AttachTop);
	CSSplitter* splitLeftBottom = new CSSplitter();
	splitLeftBottom->ParentSplit = splitLeft;
	splitLeftBottom->SplitType = VtSplitType::SplitHorizontal;
	splitLeftBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitLeftBottom);

	CSSplitter* splitRightTop = new CSSplitter();
	splitRightTop->ParentSplit = splitRight;
	splitRightTop->SplitType = VtSplitType::SplitHorizontal;
	splitRightTop->AttachDirection(VtAttachDirection::AttachTop);
	_SplitVector.push_back(splitRightTop);
	CSSplitter* splitRightBottom = new CSSplitter();
	splitRightBottom->ParentSplit = splitRight;
	splitRightBottom->SplitType = VtSplitType::SplitHorizontal;
	splitRightBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitRightBottom);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.left + rect.Width() / 2;
	rcLeft.bottom = rect.bottom;

	rcRight.left = rect.Width() / 2;
	rcRight.top = rect.top;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	CRect rcLeftTop, rcLeftBottom;

	rcLeftTop.left = rcLeft.left;
	rcLeftTop.top = rcLeft.top;
	rcLeftTop.right = rcLeft.right;
	rcLeftTop.bottom = rcLeft.Height() / 2;

	rcLeftBottom.left = rcLeft.left;
	rcLeftBottom.top = rcLeft.Height() / 2;
	rcLeftBottom.right = rcLeft.right;
	rcLeftBottom.bottom = rcLeft.bottom;


	CRect rcRightTop, rcRightBottom;

	rcRightTop.left = rcRight.left;
	rcRightTop.top = rcRight.top;
	rcRightTop.right = rcRight.right;
	rcRightTop.bottom = rcRight.Height() / 2;

	rcRightBottom.left = rcRight.left;
	rcRightBottom.top = rcRight.Height() / 2;
	rcRightBottom.right = rcRight.right;
	rcRightBottom.bottom = rcRight.bottom;


	splitLeftTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeftTop,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitLeftTop;
	_ChartList[1]->ParentSplit = splitLeftTop;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);

	splitLeftBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeftBottom,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);


	_ChartList[2]->ParentSplit = splitLeftBottom;
	_ChartList[3]->ParentSplit = splitLeftBottom;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRightTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[4],		// top pane
		_ChartList[5],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRightTop,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[4]->ParentSplit = splitRightTop;
	_ChartList[5]->ParentSplit = splitRightTop;
	_ChartList[4]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[5]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRightBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[6],		// top pane
		_ChartList[7],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRightBottom,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[6]->ParentSplit = splitRightBottom;
	_ChartList[7]->ParentSplit = splitRightBottom;
	_ChartList[6]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[7]->AttachDirection(VtAttachDirection::AttachBottom);

	splitLeft->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitLeftTop,		// top pane
		splitLeftBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	splitRight->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitRightTop,		// top pane
		splitRightBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);




	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		splitLeft,		// top pane
		splitRight,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartFourByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList)
{
	for (int i = 0; i < 8; i++)
	{
		VtChartWindow* chart = CreateChartWindow();
		chart->MainChartType(VtMainChartType::CandleStick);
		chart->ParentView(this);
		_ChartList.push_back(chart);
	}

	VtSplitterManager* splitMgr = VtSplitterManager::GetInstance();
	CRect rect;
	GetClientRect(rect);
	CSSplitter* splitMain = new CSSplitter();
	_MainWnd = (CWnd*)splitMain;
	splitMain->SplitType = VtSplitType::SplitVertical;
	_SplitVector.push_back(splitMain);

	CSSplitter* splitLeft = new CSSplitter();
	splitLeft->ParentSplit = splitMain;
	splitLeft->SplitType = VtSplitType::SplitHorizontal;
	splitLeft->AttachDirection(VtAttachDirection::AttachLeft);
	_SplitVector.push_back(splitLeft);
	CSSplitter* splitRight = new CSSplitter();
	splitRight->ParentSplit = splitMain;
	splitRight->SplitType = VtSplitType::SplitHorizontal;
	splitRight->AttachDirection(VtAttachDirection::AttachRight);
	_SplitVector.push_back(splitRight);

	CSSplitter* splitLeftTop = new CSSplitter();
	_SplitVector.push_back(splitLeftTop);
	splitLeftTop->ParentSplit = splitLeft;
	splitLeftTop->SplitType = VtSplitType::SplitHorizontal;
	splitLeftTop->AttachDirection(VtAttachDirection::AttachTop);
	CSSplitter* splitLeftBottom = new CSSplitter();
	splitLeftBottom->ParentSplit = splitLeft;
	splitLeftBottom->SplitType = VtSplitType::SplitHorizontal;
	splitLeftBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitLeftBottom);

	CSSplitter* splitRightTop = new CSSplitter();
	splitRightTop->ParentSplit = splitRight;
	splitRightTop->SplitType = VtSplitType::SplitHorizontal;
	splitRightTop->AttachDirection(VtAttachDirection::AttachTop);
	_SplitVector.push_back(splitRightTop);
	CSSplitter* splitRightBottom = new CSSplitter();
	splitRightBottom->ParentSplit = splitRight;
	splitRightBottom->SplitType = VtSplitType::SplitHorizontal;
	splitRightBottom->AttachDirection(VtAttachDirection::AttachBottom);
	_SplitVector.push_back(splitRightBottom);

	CRect rcLeft, rcRight;

	rcLeft.left = rect.left;
	rcLeft.top = rect.top;
	rcLeft.right = rect.left + rect.Width() / 2;
	rcLeft.bottom = rect.bottom;

	rcRight.left = rect.Width() / 2;
	rcRight.top = rect.top;
	rcRight.right = rect.right;
	rcRight.bottom = rect.bottom;

	CRect rcLeftTop, rcLeftBottom;

	rcLeftTop.left = rcLeft.left;
	rcLeftTop.top = rcLeft.top;
	rcLeftTop.right = rcLeft.right;
	rcLeftTop.bottom = rcLeft.Height() / 2;

	rcLeftBottom.left = rcLeft.left;
	rcLeftBottom.top = rcLeft.Height() / 2;
	rcLeftBottom.right = rcLeft.right;
	rcLeftBottom.bottom = rcLeft.bottom;


	CRect rcRightTop, rcRightBottom;

	rcRightTop.left = rcRight.left;
	rcRightTop.top = rcRight.top;
	rcRightTop.right = rcRight.right;
	rcRightTop.bottom = rcRight.Height() / 2;

	rcRightBottom.left = rcRight.left;
	rcRightBottom.top = rcRight.Height() / 2;
	rcRightBottom.right = rcRight.right;
	rcRightBottom.bottom = rcRight.bottom;


	splitLeftTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[0],		// top pane
		_ChartList[1],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeftTop,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[0]->ParentSplit = splitLeftTop;
	_ChartList[1]->ParentSplit = splitLeftTop;
	_ChartList[0]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[1]->AttachDirection(VtAttachDirection::AttachBottom);

	splitLeftBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[2],		// top pane
		_ChartList[3],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeftBottom,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);


	_ChartList[2]->ParentSplit = splitLeftBottom;
	_ChartList[3]->ParentSplit = splitLeftBottom;
	_ChartList[2]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[3]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRightTop->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[4],		// top pane
		_ChartList[5],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRightTop,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[4]->ParentSplit = splitRightTop;
	_ChartList[5]->ParentSplit = splitRightTop;
	_ChartList[4]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[5]->AttachDirection(VtAttachDirection::AttachBottom);

	splitRightBottom->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		_ChartList[6],		// top pane
		_ChartList[7],		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRightBottom,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ChartList[6]->ParentSplit = splitRightBottom;
	_ChartList[7]->ParentSplit = splitRightBottom;
	_ChartList[6]->AttachDirection(VtAttachDirection::AttachTop);
	_ChartList[7]->AttachDirection(VtAttachDirection::AttachBottom);

	splitLeft->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitLeftTop,		// top pane
		splitLeftBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcLeft,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	splitRight->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_HORIZ,
		this,				// the parent of the splitter pane
		splitRightTop,		// top pane
		splitRightBottom,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rcRight,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	splitMain->Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | SS_VERT,
		this,				// the parent of the splitter pane
		splitLeft,		// top pane
		splitRight,		// bottom pane
		splitMgr->GetID(),	// this ID is used for saving/restoring splitter
							// position and therefore it must be unique 
							// within your application
		rect,				// dimensions of the splitter pane
		90,	// m_nMaxTop value, i.e. top constraint for splitter position  
		110	// m_nMaxBottom value, i.e. bottom constraint for splitter position
	);

	_ActiveChartWnd = _ChartList[0];
}

void VtChartContainer::AddChartOneByFour()
{

}

void VtChartContainer::AddChartEightByOne()
{

}

void VtChartContainer::AddChartOneByEight()
{

}

void VtChartContainer::AddChartFourByFour()
{

}

void VtChartContainer::CopyLegacyChart(std::vector<VtChartWindow*>& chartList)
{
	
}

VtChartWindow* VtChartContainer::CreateChartWindow()
{
	VtChartWindow* chartCtrl = new VtChartWindow();
	chartCtrl->Create(IDD_DIALOG_CHART, this);
	return chartCtrl;
}

void VtChartContainer::CreateChartByMode(int mode)
{
	AddChart(mode);
}

/*
void VtChartContainer::ClearChart()
{
VtChartCtrlManager* chartCtrlMgr = VtChartCtrlManager::GetInstance();
for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it)
{
VtChartWindow* chart = *it;
chartCtrlMgr->DeleteChartCtrl(chart);
}
}
*/

void VtChartContainer::SetActiveChartWindow(VtChartWindow* chart)
{
	if (!chart)
		return;

	_ActiveChartWnd = chart;

	for (int i = 0; i < _ChartList.size(); i++)
	{
		VtChartWindow* wnd = _ChartList[i];
		if (wnd != _ActiveChartWnd)
		{
			wnd->ShowChartCtrl(false);
		}
		else
		{
			wnd->ShowChartCtrl(true);
		}
	}
}


void VtChartContainer::ClearEvent()
{
	for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it)
	{
		VtChartWindow* chart = *it;
		chart->StopChartDataEvent();
	}
}

void VtChartContainer::ShowChartCtrl()
{
	if (_ActiveChartWnd)
	{
		_ActiveChartWnd->ShowChartCtrl();
	}
}

void VtChartContainer::SetEditOption(EditOption a_EditOption)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->SetEditOption(a_EditOption);
}


void VtChartContainer::DeleteSelectedObject()
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->DeleteSelectedObject();
}

void VtChartContainer::DeleteAllObjects()
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->DeleteAllObjects();
}


void VtChartContainer::SaveToXml(pugi::xml_node& node)
{
	CString msg;

	msg.Format(_T("%d"), CurMode());
	std::string value = (LPCTSTR)msg;
	pugi::xml_node chart_layout = node.append_child("chart_layout");
	chart_layout.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), ChartList().size());
	value = (LPCTSTR)msg;

	pugi::xml_node chart_count = node.append_child("chart_count");
	chart_count.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node chart_list = node.append_child("chart_list");
	int k = 0;
	for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it)
	{
		VtChartWindow* chartWnd = *it;

		pugi::xml_node chart = chart_list.append_child("chart");

		msg.Format(_T("%d"), k);
		value = (LPCTSTR)msg;

		pugi::xml_node index = chart.append_child("index");
		index.append_child(pugi::node_pcdata).set_value(value.c_str());

		chartWnd->SaveToXml(chart);

		k++;
	}
}

void VtChartContainer::LoadFromXml(pugi::xml_node& node)
{
	std::string tab_no = node.child_value("tab_no");
	std::string chart_layout = node.child_value("chart_layout");
	std::string chart_count = node.child_value("chart_count");

	int mode = std::stoi(chart_layout);
	AddChart(mode);

	pugi::xml_node chart_list = node.child("chart_list");
	int i = 0;
	for (pugi::xml_node chart = chart_list.child("chart"); chart; chart = chart.next_sibling("chart"))
	{
		VtChartWindow* chartWnd = _ChartList[i++];
		chartWnd->LoadFromXml(chart);
	}

	SendMessage(WM_SIZE);
}

void VtChartContainer::SetChartData(std::vector<VtChartDataRequest> reqList)
{
	if (_ChartList.size() == 0 || _ChartList.size() > reqList.size())
		return;

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	int i = 0;
	for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it)
	{
		VtChartWindow* chartWnd = *it;
		VtChartData* chartData = chartDataMgr->Add(reqList[i++]);
		chartWnd->SetChartData(chartData);
	}
}

void VtChartContainer::ShowRefChart(std::string symCode, bool show)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->ShowChart(symCode, show);
}



void VtChartContainer::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::PostNcDestroy();
}


void VtChartContainer::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();
	//VtChartContainerManager* chartMgr = VtChartContainerManager::GetInstance();
	//chartMgr->RemoveChartContainer(this);
}

void VtChartContainer::InitSystem()
{
	VtSystemManager* symMgr = VtSystemManager::GetInstance();
	symMgr->CreateBundleSystem();
	_TimeToolBar.InitSystem(symMgr->GetSystemVector());

	for (auto it = _ChartList.begin(); it != _ChartList.end(); ++it)
	{
		VtChartWindow* wnd = *it;
		VtSystem* system = symMgr->GetSystem(_T("MaSystem"));
		if (system)
			wnd->System(system);
	}
}

void VtChartContainer::ChangeSystem(VtSystem* newSystem)
{
	if (_ActiveChartWnd)
		_ActiveChartWnd->ChangeSystem(newSystem);
}

void VtChartContainer::SetDateTime(VtChartData* chartData)
{
	if (!chartData)
		return;

	for (int i = 0; i < chartData->DataCount(); ++i) {
		auto key = chartData->DateTime.data()[i];
		_DateTimeSet.insert(key);
	}

	int k = 0; 
	k = k + 1;
}

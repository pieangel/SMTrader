
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "SMTrader.h"

#include "MainFrm.h"
#include "VtHdClient.h"
#include "VtProductCategoryManager.h"
#include "HdScheduler.h"
#include "HdTaskEventArg.h"
#include "VtSymbolManager.h"
#include "VtWindowManager.h"
#include "VtOrderWndHd.h"
#include "VtOrderWnd.h"
#include "VtRealtimeRegisterManager.h"
#include "VtOrderDialogManager.h"
#include "VtOrderManagerSelector.h"
#include "VtAccountManager.h"
#include "VtGlobal.h"
#include "ZmConfigManager.h"
#include "HdAccountPLDlg.h"
#include "HdWindowManager.h"
#include "VtAccountAssetDlg.h"
#include "ETSLayout.h"
#include "VtFundDesigner.h"
#include "VtFundManager.h"
#include "VtSaveManager.h"
#include "VtSubAccountManager.h"
#include "VtFundOrderManagerSelecter.h"
#include "VtFundMiniJango.h"
#include "VtAccountPasswordDlg.h"
#include "VtOrderQueueManager.h"
#include "VtChartContainer.h"
#include "VtColorManager.h"
#include "resource.h"
#include "VtChartDataManager.h"
#include "VtChartContainerManager.h"
#include "VtLogManager.h"
#include "VtLoginManager.h"
#include "System/VtSystemManager.h"
#include "VtProgressDlg.h"
#include "VtTotalOrderManager.h"
#include "HdOrderDialog.h"
#include "VtStrategyToolWnd.h"
#include "VtSystemGroupManager.h"
#include "VtDataCollectWnd.h"
#include "VtChartDataCollector.h"
#include "VtLogInDlg.h"
#include "VtStrategyWndManager.h"
#include "FileWatch/VtFileEventMonitor.h"
#include "VtAutoSignalManagerDialog.h"
#include "VtOutSystemManager.h"
#include "VtOutSignalDefManager.h"
#include "VtOutSystemOrderManager.h"
#include "VtSystemLogDlg.h"
#include "System/VtSystem.h"
#include "VtOutSystemOrderManager.h"
#include "VtSystemSetMonthsDialog.h"
#include "VtScheduler.h"
#include "VtSystemOrderConfig.h"
#include "SmOrderPanel.h"
#include "SmCallbackManager.h"
#include "Market/SmSymbolReader.h"
#include "Market/SmMarketManager.h"
#include "Chart/SmChartDataManager.h"
#include "SmPortfolioDlg.h"

extern TApplicationFont g_Font;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_LOGIN_TEST, &CMainFrame::OnLoginTest)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()

	ON_COMMAND(ID_FILE_SERVER_MSG, &CMainFrame::OnServerMsg)
	ON_COMMAND(ID_FILE_OPEN_SCREEN, &CMainFrame::OnOpenSavedScreen)
	ON_COMMAND(ID_SETTIME_TO_SERVER, &CMainFrame::OnSettimeToServer)

	ON_COMMAND(ID_ACCOUNT_MANAGE_SUBACCOUNT, &CMainFrame::OnManageSubAccount)
	ON_COMMAND(ID_ACCOUNT_SHOW_DEPOSIT, &CMainFrame::OnShowAccountDeposit)
	ON_COMMAND(ID_ACCOUNT_MANAGE_PASSWORD, &CMainFrame::OnManagePassword)

	ON_COMMAND(ID_ORDER_WINDOW, &CMainFrame::OnOrderWindow)
	ON_COMMAND(ID_ORDER_BALANCE, &CMainFrame::OnShowBalance)
	ON_COMMAND(ID_ORDER_LIST, &CMainFrame::OnShowOrderList)
	ON_COMMAND(ID_ORDER_STOPLIST, &CMainFrame::OnShowStopList)

	ON_COMMAND(ID_CHART_ORDER, &CMainFrame::OnChartOrder)
	ON_COMMAND(ID_CHART_SPREAD, &CMainFrame::OnChartSpread)

	ON_COMMAND(ID_MULTIACCOUNT_MANAGE, &CMainFrame::OnManageMultiAccount)
	ON_COMMAND(ID_MULTIACCOUNT_WINDOW, &CMainFrame::OnMultiAccountOrderWindow)
	ON_COMMAND(ID_MULTIACCOUNT_BRIEF_BALANCE, &CMainFrame::OnShowMultiAccountBalance)

	ON_COMMAND(ID_STRATEGY_TOOLBAR, &CMainFrame::OnStrategyToolbar)
	ON_WM_TIMER()
	ON_COMMAND(ID_AUTO_SIGNAL_CONNECT, &CMainFrame::OnAutoSignalConnect)
	ON_COMMAND(ID_SAVE_CURRENT_SCREEN, &CMainFrame::OnSaveCurrentScreen)
	ON_COMMAND(ID_32797, &CMainFrame::OnSetSystemMonth)
	ON_COMMAND(ID_SET_SYSMONTH, &CMainFrame::OnSetSysmonth)
	ON_COMMAND(ID_SET_ORDER, &CMainFrame::OnSetOrder)
	ON_COMMAND(ID_ORDER_TEST, &CMainFrame::OnOrderTest)
	ON_COMMAND(ID_AI, &CMainFrame::OnAi)
	ON_COMMAND(ID_COR_BTWN_PORT, &CMainFrame::OnCorBtwnPort)
	ON_COMMAND(ID_COR_BTWN_SIG, &CMainFrame::OnCorBtwnSig)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	_Init = false;
	bCreating = false;
	_OrderWndCreated = false;

	HdScheduler* taskMgr = HdScheduler::GetInstance();
	auto d = dlgt::make_delegate(&CMainFrame::OnReceiveComplete, *this);

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->LoadAbroadSymbols();

	VtLogManager* logMgr = VtLogManager::GetInstance();
	logMgr->InitLog();

	//LoadSettings();
}

CMainFrame::~CMainFrame()
{
	if (_FildMonitor) {
		delete _FildMonitor;
		_FildMonitor = nullptr;
	}

	ResetSysLogDlg();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // ����� �� �ִ� �ٸ� ��Ÿ��...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE�� �����Ͽ� �� ���� �����ʿ� �ݱ� ���߸� ��ġ�մϴ�.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE�� �����Ͽ� MDI ���� ���� �������� Ȱ��ȭ�մϴ�.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE�� �����Ͽ� MDI ���� �ڵ� �� ������ ��Ȱ��ȭ�մϴ�.
	mdiTabParams.m_bDocumentMenu = TRUE; // �� ������ ������ �����ڸ��� ���� �޴��� Ȱ��ȭ�մϴ�.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("�޴� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// �޴� ������ Ȱ��ȭ�ص� ��Ŀ���� �̵����� �ʰ� �մϴ�.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// ����� ���� ���� ���� �۾��� ����մϴ�.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	*/

	CRect rcToolbar;
	m_wndMenuBar.GetWindowRect(rcToolbar);
	_MainToolbar.Create(IDD_MAIN_TOOLBAR, this);
	_MainToolbar.SetWindowPos(nullptr, 0, rcToolbar.Height(), 0, 0, SWP_NOSIZE);
	_MainToolbar.ShowWindow(SW_HIDE);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ���� ���� �� �޴� ������ ��ŷ�� �� ���� �Ϸ��� �� �ټ� ���� �����Ͻʽÿ�.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	//DockPane(&m_wndToolBar);


	// Visual Studio 2005 ��Ÿ�� ��ŷ â ������ Ȱ��ȭ�մϴ�.
	//CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 ��Ÿ�� ��ŷ â �ڵ� ���� ������ Ȱ��ȭ�մϴ�.
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// �޴� �׸� �̹����� �ε��մϴ�(ǥ�� ���� ������ ����).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// ��ŷ â�� ����ϴ�.
// 	if (!CreateDockingWindows())
// 	{
// 		TRACE0("��ŷ â�� ������ ���߽��ϴ�.\n");
// 		return -1;
// 	}

// 	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
// 	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
// 	DockPane(&m_wndFileView);
// 	CDockablePane* pTabbedBar = NULL;
// 	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
// 	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
// 	DockPane(&m_wndOutput);
// 	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
// 	DockPane(&m_wndProperties);

	// ������ ���� ���� ���־� ������ �� ��Ÿ���� �����մϴ�.
	OnApplicationLook(theApp.m_nAppLook);

	// ���� â ���� ��ȭ ���ڸ� Ȱ��ȭ�մϴ�.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// ���� ���� �� ��ŷ â �޴� �ٲٱ⸦ Ȱ��ȭ�մϴ�.
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ����(<Alt> Ű�� ���� ä ����) ���� ���� ����� ������ Ȱ��ȭ�մϴ�.
// 	CMFCToolBar::EnableQuickCustomization();
// 
// 	if (CMFCToolBar::GetUserImages() == NULL)
// 	{
// 		// ����� ���� ���� ���� �̹����� �ε��մϴ�.
// 		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
// 		{
// 			CMFCToolBar::SetUserImages(&m_UserImages);
// 		}
// 	}

	// �޴� ���� ������ Ȱ��ȭ�մϴ�(���� �ֱٿ� ����� ���).
	// TODO: ������� �⺻ ����� �����Ͽ� �� Ǯ�ٿ� �޴��� �ϳ� �̻��� �⺻ ����� ���Խ�ŵ�ϴ�.
// 	CList<UINT, UINT> lstBasicCommands;
// 
// 	lstBasicCommands.AddTail(ID_FILE_NEW);
// 	lstBasicCommands.AddTail(ID_FILE_OPEN);
// 	lstBasicCommands.AddTail(ID_FILE_SAVE);
// 	lstBasicCommands.AddTail(ID_FILE_PRINT);
// 	lstBasicCommands.AddTail(ID_APP_EXIT);
// 	lstBasicCommands.AddTail(ID_EDIT_CUT);
// 	lstBasicCommands.AddTail(ID_EDIT_PASTE);
// 	lstBasicCommands.AddTail(ID_EDIT_UNDO);
// 	lstBasicCommands.AddTail(ID_APP_ABOUT);
// 	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
// 	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
// 	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
// 	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
// 	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
// 	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);
// 
// 	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// â ���� ǥ���ٿ��� ���� �̸� �� ���� ���α׷� �̸��� ������ ��ȯ�մϴ�.
	// ���� �̸��� ����� �׸��� �Բ� ǥ�õǹǷ� �۾� ǥ������ ��ɼ��� �����˴ϴ�.
	ModifyStyle(0, FWS_PREFIXTITLE);

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	_MainToolbar.MainFrm(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Ŭ���� �並 ����ϴ�.
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Ŭ���� �� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	// ���� �並 ����ϴ�.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("���� �� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	// ��� â�� ����ϴ�.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("��� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	// �Ӽ� â�� ����ϴ�.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�Ӽ� â�� ������ ���߽��ϴ�.\n");
		return FALSE; // ������ ���߽��ϴ�.
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* �޴��� �˻��մϴ�. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	//m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// �⺻ Ŭ������ ���� �۾��� �����մϴ�.

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// ��� ����� ���� ������ ����� ���� ���߸� Ȱ��ȭ�մϴ�.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//m_wndOutput.UpdateFonts();
}


void CMainFrame::OnLoginTest()
{
	//VtLogInDlg dlg;
	//dlg.DoModal();
	SiseStressTest();
}


void CMainFrame::OnClose()
{
	if (_OrderPanel) {
		delete _OrderPanel;
		_OrderPanel = nullptr;
	}
	KillTimer(SysLiqTimer);
	SaveSettings();
	if (!ClearAllResources())
		return;

	VtDate date = VtGlobal::GetLocalDate();
	VtTime time = VtGlobal::GetLocalTime();
	CString msg;
	msg.Format(_T("%d�� %d�� %d�� %d�� %d�� %d�ʿ� �����մϴ�."), date.year, date.month, date.day, time.hour, time.min, time.sec);
	AfxMessageBox(msg, MB_ICONEXCLAMATION);
	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMDIFrameWndEx::OnShowWindow(bShow, nStatus);
	
	ReadConfig();

	if (!_Init) {
		_Init = true;
		// ���ǻ� Ŭ���̾�Ʈ �ʱ�ȸ
		VtHdClient* client = VtHdClient::GetInstance();
		client->Init();
		// �α��� ��ȭ���ڸ� ����.
		VtLogInDlg loginDlg;
		VtSaveManager* saveMgr = VtSaveManager::GetInstance();
		int Res = loginDlg.DoModal();
		if (Res == IDOK) {

			// �α��� ����ڸ� ���� ���丮�� �����.
			VtGlobal::LoginUserID = loginDlg.id;
			saveMgr->CreateUserDirectory();
			
			// �α��� ������ �����Ѵ�.
			saveMgr->SaveLoginInfoToXml((LPCTSTR)loginDlg.id, (LPCTSTR)loginDlg.pwd, (LPCTSTR)loginDlg.cert, loginDlg.Save);

			VtSaveManager* saveMgr = VtSaveManager::GetInstance();
			// ���¿� �ݵ带 �о� �´�.
			saveMgr->ReadSettings();

			// �α��� �����ڿ� �α��� ���� ����
			VtLoginManager::GetInstance()->ID = (LPCTSTR)loginDlg.id;
			VtLoginManager::GetInstance()->Password = (LPCTSTR)loginDlg.pwd;
			VtLoginManager::GetInstance()->Cert = (LPCTSTR)loginDlg.cert;

			// ���� ������ ini���Ͽ��� �о� �´�.
			// ���¹�ȣ ��� ���� ���� �ҷ��´�.
			ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
			std::string section = _T("ACCOUNT_INFO");
			std::string name, value;
			name = _T("savepwd");
			value = configMgr->getString(section, name);

			// ���� �����ڷ� ���¸� ���͸��Ѵ�.
			VtAccountManager* acntMgr = VtAccountManager::GetInstance();
			acntMgr->FileterAccount();

			// ���� ��й�ȣ ���� â�� ����.
			VtAccountPasswordDlg dlg;
			if (value.length() > 0 && (!saveMgr->IsAccountFileExist() || value.compare(_T("false")) == 0))
				dlg.FromServer(true);
			else
				dlg.FromServer(false);
			dlg.DoModal();

			// ���簡 ���
			VtRealtimeRegisterManager* regMgr = VtRealtimeRegisterManager::GetInstance();
			regMgr->RegisterCurrent();
			// 			VtHdClient* client = VtHdClient::GetInstance();
			// 			std::string fileName = _T("product.cod");
			// 			client->GetMasterFile(fileName);
			// Ÿ�̸Ӹ� �����Ͽ� �ɺ� ���� �ٿ�ε� ����
			SetTimer(2, 500, 0);
			// �ɺ��ڵ带 �������� �����Ѵ�.
			//GetSymbolCode();
		}
		else {
			ClearAllResources();
			PostQuitMessage(WM_QUIT);
		}
	}
}

void CMainFrame::OnReceiveComplete()
{
	if (!_OrderWndCreated) {
		int width = VtGlobal::GetHorWidthByScaleFactor(376);
		int height = VtGlobal::GetVerHeightByScaleFactor(145);
		SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

		_MainToolbar.OnReceiveComplete();
		_MainToolbar.ShowWindow(SW_SHOW);
		_OrderWndCreated = true;

		VtSystemManager* sysMgr = VtSystemManager::GetInstance();
		sysMgr->InitDataSources();

		VtSaveManager* saveMgr = VtSaveManager::GetInstance();
		//saveMgr->LoadOutSignal(_T("outsignal.dat"));
		//saveMgr->LoadSystems(_T("systemlist.dat"));
		//saveMgr->LoadOutSystems(_T("outsystemlist.dat"));
		//saveMgr->LoadOrderWndList(_T("orderwndlist.dat"), this);
		saveMgr->ReadWindows();

		//SetTimer(SysLiqTimer, 5000, NULL);

		_LoadComplete = true;
	}
}

void CMainFrame::OnReceiveQuoteHd(VtSymbol* sym)
{
	_MainToolbar.OnReceiveQuoteHd(sym);
}

void CMainFrame::OpenAcntOrder()
{
	VtOrderWndHd* pOrderDlg = new VtOrderWndHd(this);
	pOrderDlg->WindowHeight(1000);
	// For the order with the accuount and the product code.
	pOrderDlg->SetType(0);
	//pOrderDlg->CenterWndCount(0);
	pOrderDlg->Create(IDD_ORDER_WND_HD, this);
	pOrderDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OpenFundOrder()
{
	VtFundManager* fundMgr = VtFundManager::GetInstance();
	std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
	if (fundList.size() == 0)
	{
		AfxMessageBox(_T("�ݵ尡 ��������� �ʾҽ��ϴ�. �ּ��� �ϳ��� �ݵ�� �����ؾ� �մϴ�."));
		return;
	}
	bCreating = true;
	VtOrderWndHd* pOrderDlg = new VtOrderWndHd(this);
	pOrderDlg->WindowHeight(750);
	// For the order with the accuount and the product code.
	pOrderDlg->SetType(1);
	//pOrderDlg->CenterWndCount(0);
	pOrderDlg->Create(IDD_ORDER_WND_HD, this);
	//pOrderDlg->SetWindowText(_T("�ݵ��ֹ�â"));
	pOrderDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OpenAcntJango()
{
	HdAccountPLDlg* pWnd = new HdAccountPLDlg(this);
	pWnd->Create(IDD_MINI_JANGO, this);
	pWnd->ShowWindow(SW_SHOW);
}

void CMainFrame::OpenFundJango()
{
	VtFundMiniJango* fundPLDlg = new VtFundMiniJango();
	fundPLDlg->Create(IDD_MINI_JANGO_FUND, this);
	fundPLDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::CreateFileWatch()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string section = _T("FILE_WATCH");
	std::string name, value;
	name = _T("path");
	value = configMgr->getString(section, name);

	_FildMonitor = new VtFileEventMonitor();
	_FildMonitor->AddMonDir(value.c_str(), true);
	//_FildMonitor->AddMonDir(_T("C:\\WRFutures\\YesGlobalPro\\YesLang"), true);
	//_FildMonitor->AddMonDir(_T("C:\\WRFutures\\YesGlobalPro\\Spot\\Export"), true);
	_FildMonitor->Start();
}

bool CMainFrame::ClearAllResources()
{
	SmChartDataManager::DestroyInstance();
	SmMarketManager::DestroyInstance();
	SmSymbolReader::DestroyInstance();
	SmCallbackManager::DestroyInstance();
	VtScheduler::DestroyInstance();
	HdScheduler* sch = HdScheduler::GetInstance();
	sch->ClearTasks();
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::vector<std::string> msgList = totalOrderMgr->GetUnsettledList();
	if (totalOrderMgr->GetUnsettledCount() > 0) {
		CString msg;
		msg.Append(_T("������ ���� �ֹ��� ���� �ֽ��ϴ�.!\n\n"));
		for (auto it = msgList.begin(); it != msgList.end(); ++it) {
			msg.Append((*it).c_str());
			msg.Append(_T("\n"));
		}
		msg.Append(_T("\n�̴�� �����Ͻðڽ��ϱ�?"));
		int Ret = AfxMessageBox(msg, MB_YESNO);
		if (Ret == IDNO)
			return false;
	}
	VtHdClient* client = VtHdClient::GetInstance();
	// ���� Ŭ���̾�Ʈ�� �޽����� �޴� ���� ���´�. 
	client->SetBlock(true);

	VtChartDataCollector::DestroyInstance();
	VtSystemGroupManager::DestroyInstance();

	VtRealtimeRegisterManager* regMgr = VtRealtimeRegisterManager::GetInstance();
	regMgr->UnregisterCurrent();
	regMgr->UnRegisterAll();

	//SaveSettings();
	VtSystemManager::DestroyInstance();
	VtLogManager::DestroyInstance();
	VtLoginManager::DestroyInstance();
	VtChartDataManager::DestroyInstance();
	//VtChartContainerManager::DestroyInstance();

	//VtColorManager::DestroyInstance();
	VtFundOrderManagerSelecter::DestroyInstance();

	VtSubAccountManager::DestroyInstance();

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	wndMgr->DestroyAllWindow();

	VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
	stgWndMgr->DestroyAllWindow();
	VtStrategyWndManager::DestroyInstance();

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->ReleaseMainOrderWnd();

	VtFundManager::DestroyInstance();
	HdWindowManager::DestroyInstance();
	ZmConfigManager::DestroyInstance();
	VtGlobal::DestroyInstance();
	VtAccountManager::DestroyInstance();
	VtOrderManagerSelector* orderMgr = VtOrderManagerSelector::GetInstance();

	VtTotalOrderManager::DestroyInstance();

	VtOrderManagerSelector::DestroyInstance();
	VtRealtimeRegisterManager::DestroyInstance();
	VtOrderDialogManager::DestroyInstance();
	VtHdClient::DestroyInstance();
	VtProductCategoryManager::DestroyInstance();
	VtSymbolManager::DestroyInstance();
	HdScheduler::DestroyInstance();
	VtWindowManager::DestroyInstance();
	VtSaveManager::DestroyInstance();
	VtOrderQueueManager::DestroyInstance();
	VtOutSignalDefManager::DestroyInstance();
	VtOutSystemManager::DestroyInstance();
	VtOutSystemOrderManager::DestroyInstance();
	return true;
}

void CMainFrame::UpdateSysLog()
{
	if (!GetSafeHwnd())
		return;
	try
	{
		CRect rcWnd;
		if (!_SysLogDlg) {
			_SysLogDlg = new VtSystemLogDlg();
			_SysLogDlg->MainFrm = this;
			_SysLogDlg->Create(IDD_SYS_LOG, this);
			_SysLogDlg->GetWindowRect(rcWnd);
			int width = rcWnd.Width();
			int height = rcWnd.Height();
			rcWnd.left = 0;
			rcWnd.top = 0;
			rcWnd.right = rcWnd.left + width;
			rcWnd.bottom = rcWnd.top + height;
			_SysLogDlg->MoveWindow(rcWnd);
			_SysLogDlg->ShowWindow(SW_SHOW);
		}
		else {
			_SysLogDlg->ShowWindow(SW_SHOW);
			_SysLogDlg->GetWindowRect(rcWnd);
			int width = rcWnd.Width();
			int height = rcWnd.Height();
			rcWnd.left = 0;
			rcWnd.top = 0;
			rcWnd.right = rcWnd.left + width;
			rcWnd.bottom = rcWnd.top + height;
			_SysLogDlg->MoveWindow(rcWnd);
			_SysLogDlg->RefreshGrid();
		}
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
	}
}

void CMainFrame::ReadConfig()
{
	SmMarketManager::GetInstance()->LoadRunInfo();
	// ���Ͽ��� �ɺ� ����� �ε��Ѵ�.
	SmSymbolReader::GetInstance()->ReadSymbolFileList();

	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	// ���� ������ xml���Ͽ��� �о� �´�.
	saveMgr->LoadRunInfoFromXml();
	// ���� ������ ini���Ͽ��� �о� �´�.
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string section = _T("FILE_WATCH");
	std::string file_watch_flag;
	std::string name = _T("enable");
	file_watch_flag = configMgr->getString(section, name);
	std::string file_watch_path;
	name = _T("path");
	file_watch_path = configMgr->getString(section, name);
	if (file_watch_flag.length() > 0 && stoi(file_watch_flag) == 1) {
		_EnableFileWatch = true;
	}
	else {
		_EnableFileWatch = false;
	}

	_FleWathPath = file_watch_path;
	VtGlobal::FileWatchPath = _FleWathPath;
	VtGlobal::EnableFileWatch = _EnableFileWatch;
	if (_FleWathPath.length() > 0) {
		CreateFileWatch();
	}

	
}

void CMainFrame::StartPreProcess()
{
	SmMarketManager::GetInstance()->ReadDomesticSymbolsFromFile();
	SmMarketManager::GetInstance()->ReadAbroadSymbolsFromFile();
	GetSymbolCode();
}

void CMainFrame::ResetSysLogDlg()
{
// 	if (_SysLogDlg) {
// 		_SysLogDlg->DestroyWindow();
// 		delete _SysLogDlg;
// 		_SysLogDlg = nullptr;
// 	}
}

void CMainFrame::GetSymbolCode()
{
	LOG_F(INFO, _T("GetSymbolCode was called!"));
	ProgressDlg = new VtProgressDlg();
	ProgressDlg->Create(IDD_PROGRESS, this);
	ProgressDlg->MainFrm = this;
	ProgressDlg->ShowWindow(SW_SHOW);
	ProgressDlg->SetForegroundWindow();
	ProgressDlg->BringWindowToTop();
}

void CMainFrame::SaveSettings()
{
	// �Ϻ��ϰ� ȯ���� �ε���� �ʾҴٸ� �� ������ �������� �ʴ´�.
	// ������ ���ؼ� ���α׷��� ����Ǵ� ���� �����ϱ� �����̴�.
	if (!_LoadComplete)
		return;

	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
// 	saveMgr->SaveFundList(_T("fundlist.dat"));
// 	saveMgr->SaveAccountList(_T("accountlist.dat"));
// 	saveMgr->SaveOrderWndList(_T("orderwndlist.dat"), this);
// 	saveMgr->SaveSystems(_T("systemlist.dat"));
// 	saveMgr->SaveOutSignal(_T("outsignal.dat"));
// 	saveMgr->SaveOutSystems(_T("outsystemlist.dat"));
	saveMgr->WriteSettings();
}

void CMainFrame::LoadSettings()
{
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	//saveMgr->LoadAccountList(_T("accountlist.dat"));

	//saveMgr->LoadFundList(_T("fundlist.dat"));

	saveMgr->ReadSettings();
}



void CMainFrame::OnServerMsg()
{
	UpdateSysLog();
}

void CMainFrame::OnOpenSavedScreen()
{

}

void CMainFrame::OnManageSubAccount()
{
	VtFundDesigner* fundDesigner = new VtFundDesigner();
	fundDesigner->SelTab = 0;
	fundDesigner->Create(IDD_FUND_DESIGNER, this);
	fundDesigner->ShowWindow(SW_SHOW);
}

void CMainFrame::OnShowAccountDeposit()
{
	VtAccountAssetDlg* dlg = new VtAccountAssetDlg(this);
	dlg->Create(IDD_ACCOUNT_ASSET, this);
	dlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnManagePassword()
{
	VtAccountPasswordDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnOrderWindow()
{
	if (bCreating)
		return;
	bCreating = true;
	OpenAcntOrder();
	bCreating = false;
}

void CMainFrame::OnShowBalance()
{
	OpenAcntJango();
}

void CMainFrame::OnShowOrderList()
{
	HdOrderDialog* dlg = new HdOrderDialog();
	dlg->Create(IDD_ORDER_TOTAL, this);
	dlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnShowStopList()
{

}

void CMainFrame::OnChartOrder()
{
	_ChartContainer = new VtChartContainer();
	_ChartContainer->Create(IDD_CHART_CONTAINER, this);
	_ChartContainer->ShowWindow(SW_SHOW);
}


void CMainFrame::OnChartSpread()
{
	_ChartContainer = new VtChartContainer();
	_ChartContainer->Create(IDD_CHART_CONTAINER, this);
	_ChartContainer->ShowWindow(SW_SHOW);
}

void CMainFrame::OnManageMultiAccount()
{
	VtFundDesigner* fundDesigner = new VtFundDesigner();
	fundDesigner->SelTab = 1;
	fundDesigner->Create(IDD_FUND_DESIGNER, this);
	fundDesigner->ShowWindow(SW_SHOW);
}

void CMainFrame::OnMultiAccountOrderWindow()
{
	if (bCreating)
		return;
	OpenFundOrder();
	bCreating = false;
}

void CMainFrame::OnShowMultiAccountBalance()
{
	OpenFundJango();
}

void CMainFrame::OnStrategyToolbar()
{
	if (VtGlobal::StrategyToolWnd)
		return;

	VtStrategyToolWnd* dlg = new VtStrategyToolWnd(this);
	dlg->Create(IDD_STRATEGE_TOOLS, this);
	dlg->ShowWindow(SW_SHOW);
	VtGlobal::StrategyToolWnd = dlg;
}


void CMainFrame::OnSettimeToServer()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->GetServerTime();
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 2) {
		SmSymbolReader* symReader = SmSymbolReader::GetInstance();
		VtHdClient* hdClient = VtHdClient::GetInstance();
		if (symReader->DomesticSymbolMasterFileSet.size() > 0) {
			std::string file_name = *(symReader->DomesticSymbolMasterFileSet.begin());
			hdClient->DownloadDomesticMasterFile(file_name);
		}
		else {
			KillTimer(2);
			hdClient->DownloadMasterFiles("futures");
		}
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnAutoSignalConnect()
{
	VtAutoSignalManagerDialog* dlg = new VtAutoSignalManagerDialog();
	dlg->Create(IDD_SYS_AUTO_CONNECT, this);
	dlg->ShowWindow(SW_SHOW);
}


void CMainFrame::OnSaveCurrentScreen()
{
	SaveSettings();
}

void CMainFrame::OrderStressTest()
{
	// �ֹ� ��Ʈ���� �׽�Ʈ
	std::thread threadObj2([] {
		VtSystemManager* sysMgr = VtSystemManager::GetInstance();
		VtSystem* sys = sysMgr->GetSystem(_T("KP_1A"));
		std::string name = sys->Name();
		try
		{
			for (int i = 0; i < 10000; i++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				sys->PutEntranceOrder(VtPositionType::Sell);
			}
		}
		catch (std::exception e)
		{
			AfxMessageBox(e.what());
		}
	});



	std::thread threadObj([] {
		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		SharedSystemVec& sysMap = outSysMgr->GetSysMap();
		for (int i = 0; i < 10000; i++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			for (auto it = sysMap.begin(); it != sysMap.end(); ++it) {
				VtSystem* shareSys = (*it).get();
				std::string name = shareSys->Name();
				shareSys->PutEntranceOrder(VtPositionType::Buy);
			}
		}
	});

	std::thread threadObj3([] {
		VtSystemManager* sysMgr = VtSystemManager::GetInstance();
		VtSystem* sys = sysMgr->GetSystem(_T("KP_1A"));
		std::string name = sys->Name();
		try
		{
			for (int i = 0; i < 10000; i++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				sys->PutEntranceOrder(VtPositionType::Sell);
			}
		}
		catch (std::exception e)
		{
			AfxMessageBox(e.what());
		}
	});

	std::thread threadObj4([] {
		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		SharedSystemVec& sysMap = outSysMgr->GetSysMap();
		for (int i = 0; i < 10000; i++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			for (auto it = sysMap.begin(); it != sysMap.end(); ++it) {
				VtSystem* shareSys = (*it).get();
				std::string name = shareSys->Name();
				shareSys->PutEntranceOrder(VtPositionType::Buy);
			}
		}
	});
	threadObj.detach();
	threadObj2.detach();
	threadObj3.detach();
	threadObj4.detach();
	if (threadObj2.joinable())
		threadObj2.join();
	if (threadObj.joinable())
		threadObj.join();
	if (threadObj3.joinable())
		threadObj3.join();
	if (threadObj4.joinable())
		threadObj4.join();
}

void CMainFrame::HogaStressTest()
{
	std::thread threadObj1([] {
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol("101P6000");
		if (sym)
			orderDlgMgr->OnReceiveHoga(sym);
	});

	std::thread threadObj2([] {
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol("301P6267");
		//if (sym)
		//	orderDlgMgr->OnReceiveHoga(sym);
	});

	std::thread threadObj3([] {
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol("201P6272");
		//if (sym)
		//	orderDlgMgr->OnReceiveHoga(sym);
	});

	threadObj1.detach();
	threadObj2.detach();
	threadObj3.detach();
	if (threadObj1.joinable())
		threadObj1.join();
	if (threadObj2.joinable())
		threadObj2.join();
	if (threadObj3.joinable())
		threadObj3.join();
}

void CMainFrame::SiseStressTest()
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol("101P6000");
	VtSymbol* sym2 = symMgr->FindSymbol("301P6272");
	VtSymbol* sym3 = symMgr->FindSymbol("201P6272");

	for (int i = 0; i < 10000; i++) {
		orderDlgMgr->OnReceiveHoga(sym);
		//orderDlgMgr->OnReceiveHoga(sym2);
		//orderDlgMgr->OnReceiveHoga(sym3);
	}
}


void CMainFrame::OnSetSystemMonth()
{
	
}


void CMainFrame::OnSetSysmonth()
{
	VtSystemSetMonthsDialog dlg;
	dlg.DoModal();
}


void CMainFrame::OnSetOrder()
{
	VtSystemOrderConfig orderCfgDlg;
	orderCfgDlg._PriceType = VtSystemManager::PriceType;
	orderCfgDlg._OrderTick = VtSystemManager::OrderTick;
	int result = orderCfgDlg.DoModal();
	if (result == IDOK) {
		VtSystemManager::PriceType = orderCfgDlg._PriceType;
		VtSystemManager::OrderTick = orderCfgDlg._OrderTick;

		VtSystemManager* sysMgr = VtSystemManager::GetInstance();
		std::map<std::string, VtSystem*>& sysMap = sysMgr->GetSystemMap();
		for (auto it = sysMap.begin(); it != sysMap.end(); ++it) {
			VtSystem* sys = it->second;
			sys->PriceType(orderCfgDlg._PriceType);
			sys->OrderTick(orderCfgDlg._OrderTick);
		}
	}
}


void CMainFrame::OnOrderTest()
{
	_OrderPanel = new SmOrderPanel();
	_OrderPanel->Create(IDD_ORDER_PANEL, this);
	_OrderPanel->ShowWindow(SW_SHOW);
}


void CMainFrame::OnAi()
{
	if (VtGlobal::StrategyToolWnd)
		return;

	VtStrategyToolWnd* dlg = new VtStrategyToolWnd(this);
	dlg->Create(IDD_STRATEGE_TOOLS, this);
	dlg->ShowWindow(SW_SHOW);
	VtGlobal::StrategyToolWnd = dlg;
}


void CMainFrame::OnCorBtwnPort()
{
	SmPortfolioDlg* dlg;
	dlg = new SmPortfolioDlg(this);
	dlg->Mode(0);
	dlg->Create(IDD_PORT_COR, this);
	dlg->ShowWindow(SW_SHOW);
}


void CMainFrame::OnCorBtwnSig()
{
	SmPortfolioDlg* dlg;
	dlg = new SmPortfolioDlg(this);
	dlg->Mode(1);
	dlg->Create(IDD_PORT_COR, this);
	dlg->ShowWindow(SW_SHOW);
}

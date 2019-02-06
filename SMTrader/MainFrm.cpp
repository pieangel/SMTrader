
// MainFrm.cpp : CMainFrame 클래스의 구현
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
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
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

	LoadSettings();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	*/

	CRect rcToolbar;
	m_wndMenuBar.GetWindowRect(rcToolbar);
	_MainToolbar.Create(IDD_MAIN_TOOLBAR, this);
	_MainToolbar.SetWindowPos(nullptr, 0, rcToolbar.Height(), 0, 0, SWP_NOSIZE);
	_MainToolbar.ShowWindow(SW_HIDE);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	//DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	//CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
// 	if (!CreateDockingWindows())
// 	{
// 		TRACE0("도킹 창을 만들지 못했습니다.\n");
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

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
// 	CMFCToolBar::EnableQuickCustomization();
// 
// 	if (CMFCToolBar::GetUserImages() == NULL)
// 	{
// 		// 사용자 정의 도구 모음 이미지를 로드합니다.
// 		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
// 		{
// 			CMFCToolBar::SetUserImages(&m_UserImages);
// 		}
// 	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
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

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	_MainToolbar.MainFrm(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 파일 뷰를 만듭니다.
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
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

// CMainFrame 진단

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


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
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
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
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
	VtLogInDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnClose()
{
	SaveSettings();
	if (!ClearAllResources())
		return;

	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMDIFrameWndEx::OnShowWindow(bShow, nStatus);
	if (!_Init) {
		_Init = true;
		// 증권사 클라이언트 초기회
		VtHdClient* client = VtHdClient::GetInstance();
		client->Init();

		// 로그인 대화상자를 띄운다.
		VtLogInDlg loginDlg;
		VtSaveManager* saveMgr = VtSaveManager::GetInstance();
		int Res = loginDlg.DoModal();
		if (Res == IDOK) {
			saveMgr->SaveLoginInfo(_T("SmTrader.cfg"), (LPCTSTR)loginDlg.id, (LPCTSTR)loginDlg.pwd, (LPCTSTR)loginDlg.cert, loginDlg.Save);

			ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
			std::string section = _T("ACCOUNT_INFO");
			std::string name, value;
			name = _T("savepwd");
			value = configMgr->getString(section, name);
			if (!saveMgr->IsAccountFileExist() || value.compare(_T("false")) == 0) {
				VtGlobal* global = VtGlobal::GetInstance();
				VtAccountPasswordDlg dlg;
				dlg.FromServer(true);
				int Ret = dlg.DoModal();
			}

			VtRealtimeRegisterManager* regMgr = VtRealtimeRegisterManager::GetInstance();
			regMgr->RegisterCurrent();
			VtHdClient* client = VtHdClient::GetInstance();
			std::string fileName = _T("product.cod");
			client->GetMasterFile(fileName);
			GetSymbolCode();
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

		VtSaveManager* saveMgr = VtSaveManager::GetInstance();
		saveMgr->LoadSystems(_T("systemlist.dat"));
		saveMgr->LoadOrderWndList(_T("orderwndlist.dat"), this);
	}
}

void CMainFrame::OnReceiveQuoteHd(VtSymbol* sym)
{
	_MainToolbar.OnReceiveQuoteHd(sym);
}

void CMainFrame::OpenAcntOrder()
{
	VtOrderWndHd* pOrderDlg = new VtOrderWndHd(this);
	pOrderDlg->WindowHeight(750);
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
		AfxMessageBox(_T("펀드가 만들어지지 않았습니다. 최소한 하나의 펀드는 존재해야 합니다."));
		return;
	}
	bCreating = true;
	VtOrderWndHd* pOrderDlg = new VtOrderWndHd(this);
	pOrderDlg->WindowHeight(750);
	// For the order with the accuount and the product code.
	pOrderDlg->SetType(1);
	//pOrderDlg->CenterWndCount(0);
	pOrderDlg->Create(IDD_ORDER_WND_HD, this);
	//pOrderDlg->SetWindowText(_T("펀드주문창"));
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


bool CMainFrame::ClearAllResources()
{
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::vector<std::string> msgList = totalOrderMgr->GetUnsettledList();
	if (totalOrderMgr->GetUnsettledCount() > 0) {
		CString msg;
		msg.Append(_T("다음과 같은 주문이 남아 있습니다.!\n"));
		for (auto it = msgList.begin(); it != msgList.end(); ++it) {
			msg.Append((*it).c_str());
			msg.Append(_T("\n"));
		}
		msg.Append(_T("이대로 종료하시겠습니까?"));
		int Ret = AfxMessageBox(msg, MB_YESNO);
		if (Ret == IDNO)
			return false;
	}

	VtHdClient* client = VtHdClient::GetInstance();
	// 먼저 클라이언트가 메시지를 받는 것을 막는다. 
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
	VtChartContainerManager::DestroyInstance();

	//VtColorManager::DestroyInstance();
	VtFundOrderManagerSelecter::DestroyInstance();

	VtSubAccountManager::DestroyInstance();

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	wndMgr->DestroyAllWindow();

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

	return true;
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
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	saveMgr->SaveFundList(_T("fundlist.dat"));
	saveMgr->SaveAccountList(_T("accountlist.dat"));
	saveMgr->SaveOrderWndList(_T("orderwndlist.dat"), this);
	saveMgr->SaveSystems(_T("systemlist.dat"));
	saveMgr->SaveTotal(_T("smtrader.sav"), this);
}

void CMainFrame::LoadSettings()
{
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	saveMgr->LoadAccountList(_T("accountlist.dat"));

	saveMgr->LoadFundList(_T("fundlist.dat"));
}



void CMainFrame::OnServerMsg()
{

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
	HdOrderDialog dlg;
	dlg.DoModal();
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
	VtStrategyToolWnd* dlg = new VtStrategyToolWnd(this);
	dlg->Create(IDD_STRATEGE_TOOLS, this);
	dlg->ShowWindow(SW_SHOW);
}


void CMainFrame::OnSettimeToServer()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->GetServerTime();
}

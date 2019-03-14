
// SMTrader.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SMTrader.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SMTraderDoc.h"
#include "SMTraderView.h"
#include "VtGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//
// Global font
//
TApplicationFont g_Font(_T("����"));

//
// Callback functions for EnumChildWindows
//

// lParam is a pointer to CFont object
BOOL __stdcall SetChildFont(HWND hwnd, LPARAM lparam)
{
	CFont *pFont = (CFont*)lparam;
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	pWnd->SetFont(pFont);
	return TRUE;
}

//=============================================================================
// TApplicationFont
//
//=============================================================================

TApplicationFont::TApplicationFont(LPCTSTR szFaceName)
{
	m_strFaceName = szFaceName;
	createFont();
}

TApplicationFont::~TApplicationFont()
{
}

void TApplicationFont::createFont(void)
{
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

	CWindowDC wdc(NULL);
	int nLPixY = GetDeviceCaps(wdc.m_hDC, LOGPIXELSY);

	m_nFontSize = -8; // metrics.lfStatusFont.lfHeight;
	m_nPointSize = -MulDiv(m_nFontSize, 72, nLPixY);
	_tcscpy(metrics.lfStatusFont.lfFaceName, m_strFaceName);
	m_Font.CreateFontIndirect(&metrics.lfStatusFont);
}

CFont *TApplicationFont::GetFont(void)
{
	return &m_Font;
}

CString& TApplicationFont::GetFaceName(void)
{
	return m_strFaceName;
}

int TApplicationFont::GetFontSize(void)
{
	return m_nFontSize;
}

int TApplicationFont::GetPointSize(void)
{
	return m_nPointSize;
}

void TApplicationFont::SetFaceName(LPCTSTR szFaceName)
{
	m_strFaceName = szFaceName;
	m_Font.DeleteObject();
	createFont();
}
// CSMTraderApp

BEGIN_MESSAGE_MAP(CSMTraderApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSMTraderApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSMTraderApp ����

CSMTraderApp::CSMTraderApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SMTrader.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

int CSMTraderApp::RegisterOCX(CString strFileName)
{
#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - start");
#endif
	int			iReturn = 1;
	CString		szErrorMsg;

	strFileName.Replace("'\'", "\\");
	// Initialize OLE.
	if (FAILED(OleInitialize(NULL))) {
		AfxMessageBox("DLLRegister OleInitialize ����");
		return 1;
	}

	SetErrorMode(SEM_FAILCRITICALERRORS);       // Make sure LoadLib fails.
												// Load the library.
	HINSTANCE hLib = LoadLibraryEx(strFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hLib == NULL) {
		szErrorMsg.Format("File Name=%s, GetLastError() NO = 0x%08lx\n", strFileName, GetLastError());
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupOle;
	}

	HRESULT(STDAPICALLTYPE * lpDllEntryPoint)(void);
	// Find the entry point.
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, "DllRegisterServer");
	if (lpDllEntryPoint == NULL) {
		// 		TCHAR szExt[_MAX_EXT];
		// 		_tsplitpath(strFileName, NULL, NULL, NULL, szExt);

		TCHAR drive[255];
		TCHAR szExt[255];
		TCHAR path[MAX_PATH];
		TCHAR filename[MAX_PATH];
		_tsplitpath_s((LPTSTR)(LPCTSTR)strFileName, drive, _countof(drive), path, _countof(path), filename, _countof(filename), szExt, _countof(szExt));

		if ((_stricmp(szExt, ".dll") != 0) && (_stricmp(szExt, ".ocx") != 0)) {
			szErrorMsg.Format("File Name=%s, GetProcAddress Fail\n", strFileName);
			AfxMessageBox(szErrorMsg);
		}

		iReturn = 0;
		goto CleanupLibrary;
	}

	// Call the entry point.
	if (FAILED((*lpDllEntryPoint)())) {
		szErrorMsg.Format("File Name=%s, lpDllEntryPoint Fail\n", strFileName);
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupLibrary;
	}
	return iReturn;

CleanupLibrary:
	FreeLibrary(hLib);

CleanupOle:
	OleUninitialize();

#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - end");
#endif

	return iReturn;
}

// ������ CSMTraderApp ��ü�Դϴ�.

CSMTraderApp theApp;


// CSMTraderApp �ʱ�ȭ

BOOL CSMTraderApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// íƮ OCX ���� ��� ó��
	TCHAR iniFileName[500] = { 0 };

	GetModuleFileName(NULL, iniFileName, MAX_PATH);
	CString path = iniFileName;
	CString fileName = path.Left(path.ReverseFind('\\') + 1);
	fileName = fileName += "HDFCommAgent.ocx";
	RegisterOCX(fileName);


	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_SMTraderTYPE,
		RUNTIME_CLASS(CSMTraderDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CSMTraderView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	int nCmdShow = SW_NORMAL;
	UINT flags = WPF_SETMINPOSITION;
	CRect rect;

	int ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	rect.left = (ScreenWidth) / 2 - 200;
	rect.top = 10;
	int width = VtGlobal::GetHorWidthByScaleFactor(376);
	rect.right = rect.left + width;
	rect.bottom = rect.top + 78;
	pMainFrame->MoveWindow(rect);
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CSMTraderApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CSMTraderApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CSMTraderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSMTraderApp ����� ���� �ε�/���� �޼���

void CSMTraderApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CSMTraderApp::LoadCustomState()
{
}

void CSMTraderApp::SaveCustomState()
{
}

// CSMTraderApp �޽��� ó����




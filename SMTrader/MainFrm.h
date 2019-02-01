
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "VtProgressDlg.h"
#include "VtMainToolbar.h"

class VtSymbol;
class VtChartContainer;
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnLoginTest();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnReceiveComplete();
	void OnReceiveQuoteHd(VtSymbol* sym);
	void OpenAcntOrder();
	void OpenFundOrder();
	void OpenAcntJango();
	void OpenFundJango();
private:
	bool _Init = false;
	bool bCreating = false;
	bool _OrderWndCreated = false;
	bool _AccountsInfoReceived = false;
	VtProgressDlg* ProgressDlg = nullptr;
	VtMainToolbar _MainToolbar;
	VtChartContainer* _ChartContainer = nullptr;
	bool ClearAllResources();
public:
	void GetSymbolCode();
	void SaveSettings();
	void LoadSettings();
	// ���� ���� �Լ�
	afx_msg void OnServerMsg();
	afx_msg void OnOpenSavedScreen();

	// ���� ���� �Լ�
	afx_msg void OnManageSubAccount();
	afx_msg void OnShowAccountDeposit();
	afx_msg void OnManagePassword();

	// �ֹ� ���� �Լ�
	afx_msg void OnOrderWindow();
	afx_msg void OnShowBalance();
	afx_msg void OnShowOrderList();
	afx_msg void OnShowStopList();

	// ��Ʈ ���� �Լ�
	afx_msg void OnChartOrder();
	afx_msg void OnChartSpread();

	// �ٰ��� ���� �Լ�
	afx_msg void OnManageMultiAccount();
	afx_msg void OnMultiAccountOrderWindow();
	afx_msg void OnShowMultiAccountBalance();
	afx_msg void OnStrategyToolbar();
	afx_msg void OnSettimeToServer();
};



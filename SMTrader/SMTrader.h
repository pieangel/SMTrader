
// SMTrader.h : SMTrader ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

BOOL __stdcall SetChildFont(HWND hwnd, LPARAM lparam);

class TApplicationFont : public CObject {
private:
	CString m_strFaceName;
	int m_nFontSize;
	int m_nPointSize;
	CFont m_Font;

private:
	void createFont(void);

public:
	TApplicationFont(LPCTSTR szFaceName);
	virtual ~TApplicationFont();

	CFont *GetFont(void);
	CString& GetFaceName(void);
	int GetFontSize(void);
	int GetPointSize(void);

	void SetFaceName(LPCTSTR szFaceName);
};


// CSMTraderApp:
// �� Ŭ������ ������ ���ؼ��� SMTrader.cpp�� �����Ͻʽÿ�.
//

class CSMTraderApp : public CWinAppEx
{
public:
	CSMTraderApp();


// �������Դϴ�.
public:
	int RegisterOCX(CString strFileName);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSMTraderApp theApp;

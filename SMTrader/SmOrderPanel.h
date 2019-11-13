#pragma once
#include "SmOrderGrid.h"
#include "SimpleBinStream.h"
#include "OXLayoutManager.h"
#include "VtCutManager.h"
#include "VtConfigGrid.h"
#include "ShadeButtonST.h"
#include "VtRefreshManager.h"
#include "GradientStatic.h"
#include <thread>
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
// SmOrderPanel dialog
class VtSymbol;
class SmTest {
public:
	void OnSise(VtSymbol* symbol);
};
class VtOrderWndHd;
class VtFund;
class SmOrderPanel : public CDialogEx
{
	DECLARE_DYNAMIC(SmOrderPanel)

public:
	SmOrderPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmOrderPanel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_PANEL };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	SmOrderGrid m_Grid;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void InitAll();
	void Activated(bool flag);
	void ResetByCenterRow();
	void ChangeFocus();
	int GetWindowWidth();
	void RefreshLayout(bool flag);
	VtOrderWndHd* ParentDlg() const { return _ParentDlg; }
	void ParentDlg(VtOrderWndHd* val) { _ParentDlg = val; }
	int x, y, width, height;
	int GetMaxWidth();
	void AdjustControlForMode();
	void RepositionControl(int height);
	void ResizeOrderGrid(int maxHeight);
	void ResizeOrderGrid();
	int GetCountOrderGridEnabledCol();
	bool ShowTickWnd();
	void ChangeAccount(VtAccount* acnt);
	void ChangeFund(VtFund* fund);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void ChangeSymbol(VtSymbol* symbol);
	void UnregisterOrderWnd();
private:
	VtOrderWndHd* _ParentDlg = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

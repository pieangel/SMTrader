#pragma once
#include <vector>
#include "VtOrderLeftWndHd.h"
#include "VtOrderRightWndHd.h"
#include "VtOrderCenterWndHd.h"
#include "afxwin.h"
#include "VtOrderWndEvent.h"
#include "Poco/BasicEvent.h"
#include "VtRealTickWnd.h"
#include "ETSLayout.h"
#include "SimpleBinStream.h"
//#include "TTComboBox.h"
#include "ShadeButtonST.h"
#include "SmOrderPanel.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

using Poco::BasicEvent;
const int MinOrderWndSize = 200;
const int CenterWndWidth = 486;
const int CenterTopGap = 114;
const int CenterTop = 24;
class VtOrderConfigManager;
class VtSymbol;
struct VtOrder;
struct VtPosition;
class VtAccount;
class VtFund;
class VtLayoutManager;
// 주문창 컨트롤 위치 정하기
// 제일 먼저 전체 크기를 정한다.
// 전체 크기로 늘린다.
// 왼쪽창과 오른쪽창, 중앙창의 위치와 크기를 정한다.
// 왼쪽창과 오른쪽창을 만든다.
// 컨트롤들을 이동시킨다.
// 중앙창을 개수만큼 만든다.
// 중앙창의 컨트롤들을 정렬시킨다.
// VtOrderWndHd dialog
const int StartWndID = 40000;
const int LimitCenterWidth = 264;

class VtOrderWndHd : public CDialog
{
	DECLARE_DYNAMIC(VtOrderWndHd)

public:
	VtOrderWndHd(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderWndHd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_WND_HD };
#endif

	// 함수 안에서는 객체를 바꾸지 않는다는 뜻
	//int CenterWndCount() const { return _CenterWndCount; }
	//void CenterWndCount(int val) { _CenterWndCount = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
	int WindowHeight() const { return _WindowHeight; }
	void WindowHeight(int val) { _WindowHeight = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
	bool EnableSizing() const { return _EnableSizing; }
	void EnableSizing(bool val) { _EnableSizing = val; }
	int GetCenterWndCount() {
		return _CenterWndVector.size();
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	static int _MaxWidth;
private:
	void RegisterRealtimeAccount(VtAccount* acnt);
	void UnregisterRealtimeAccount(VtAccount* acnt);
	VtAccount* _Account = nullptr;
	VtFund* _Fund = nullptr;
	VtOrderLeftWndHd _LeftWnd;
	VtOrderRightWndHd _RightWnd;
	//std::vector<VtOrderCenterWndHd*> _CenterWndVector;
	std::vector<SmOrderPanel*> _CenterWndVector;
	std::map<VtRealTickWnd*, VtRealTickWnd*> _RealTickWndVector;
	void Begin();
	void End();
	virtual BOOL OnInitDialog();
	void SetWindows();
	void InitSettings();
	bool _ShowLeftWnd = true;
	bool _ShowRightWnd = true;
	void RemoveRealTickWnd(VtRealTickWnd* realWnd);
private:
	//int  _CenterWndCount;
	int _WindowID;
	void CreateChildWindows();
	void CreateChildWindows(std::vector<CRect>& rcVec);
	void RemoveLastWindow();
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	void BlockEvent();
	void SetDefaultCenterWnd();
	void ResetByCenterRow();
	int ResizeWindow();
	void RepositionChildWindowsByReverse(int winWidth);
	void ResizeWindowByGridOption();
	void RefreshLayout(bool resize, bool recalGrid, bool forward = true);
	int GetTitleBarHeight();
	afx_msg void OnClose();
	void OnFundAdded();
	void OnFundDeleted(VtFund* fund);
	bool InitFund();
	void InitAccount();
	void OnReceiveHoga(VtSymbol* sym);
	void OnReceiveQuote(VtSymbol* sym);

	void OnExpected(VtSymbol* sym);
	void OnReceiveAccountInfo();
	void OnReceiveMsg(CString msg);
	void OnReceiveMsgWithReqId(int id, CString msg);
	void GetOptionSymbolMaster();
	void OnOutstanding();
	void AddWindow();
	SmOrderPanel* AddWindow(CRect& rcWnd);
	void RemoveWindow();
	void RecalcLayout(bool fixed, int maxHeight = 0);
	void ResizeWnd(int maxHeight);
	void ShowLeftWnd();
	void ShowRightWnd();
	bool GetShowLeftWnd()
	{
		return _ShowLeftWnd;
	}

	bool GetShowRightWnd()
	{
		return _ShowRightWnd;
	}

	VtFund* GetCurrentFund();
	void ShowHideCtrl();
public:
	BasicEvent<VtOrderWndEventArgs> _OrderWindowEvent;
	void FireOrderWindowEvent(VtOrderWndEventArgs&& arg)
	{
		_OrderWindowEvent(this, arg);
	}
	void SetActiveCenterWnd(SmOrderPanel* centerWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

private:
	bool _Sizing = false;
	void CreateChildWindow(SmOrderPanel* centerWnd, UINT id, CWnd* parent);
	void SetControlPositionsOnLeft(int left);
	void SetControlPositionsOnRight(int right);
	CRect GetLastCenterWindowPos();
	VtLayoutManager* _LayoutMgr = nullptr;
	void SaveControlPos();
	CRect GetClientArea(int resourceID);
	void ReposChildWindowsForward();
	void ReposChildWindowsBackward();
	std::vector<std::tuple<CWnd*, bool, CRect>> _ChildWindowInfo;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	bool ClickedRightExtend = false;
	void OnSymbolMaster(VtSymbol* sym);
	void OnRemain(VtPosition* posi);
	void OnOrderAccepted(VtOrder* order);
	void OnOrderUnfilled(VtOrder* order);
	void OnOrderFilled(VtOrder* order);

	void ChangeAccount(VtAccount* acnt);
	CShadeButtonST _BtnAddWnd;
	CShadeButtonST _BtnDelWnd;
	CShadeButtonST _BtnGetAcntInfo;
	CShadeButtonST _BtnShowLeft;
	CShadeButtonST _BtnShowRight;
	CComboBox _ComboAcnt;
	CStatic _StaticAcnt;
	CStatic _StaticAcntName;
	// Edit the funds.
	CShadeButtonST _BtnFundEditor;
	afx_msg void OnCbnSelchangeComboAccountHd();
	afx_msg void OnBnClickedBtnShowLeft();
	afx_msg void OnBnClickedBtnGetAcntInfo();
	afx_msg void OnBnClickedBtnDelwnd();
	afx_msg void OnBnClickedBtnAddwnd();
	afx_msg void OnBnClickedBtnShowright();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

	void SetAccount(VtAccount* acnt);
	void SetFund(VtFund* fund);

	void SetType(int type);
	afx_msg void OnStnClickedStaticAcntName();
	afx_msg void OnStnClickedStaticAcnt();
	afx_msg void OnBnClickedBtnEditFund();

private:
	int _XPos = 0;
	int _YPos = 0;
	int _WindowWidth = 823;
	int _WindowHeight = 0;
	int _DefaultHeight = 1000;
	void CalcLayout();
	int GetTotalCenterWidth();
	void RepositionControl();
	void RecalChildWindows();
	void ShowCtrlByType();
	bool _EnableOnSizeEvent = false;
	bool _OnResizing = false;
	bool _Capturing = false;
	bool _EnableSizing = true;
	std::string _DefaultAccountNo;
	std::string _DefaultFundName;
	int _OldWndWidth = 0;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);

protected:
	afx_msg LRESULT OnMyMouseMove(WPARAM, LPARAM param);
	afx_msg LRESULT OnMyMouseLDBClick(WPARAM, LPARAM param);
	afx_msg LRESULT OnMyMouseLButtonUp(WPARAM, LPARAM param);
	afx_msg LRESULT OnMyMouseLButtonDown(WPARAM, LPARAM param);
	afx_msg LRESULT OnMyMouseRButtonUp(WPARAM, LPARAM param);
	afx_msg LRESULT OnMyMouseRButtonDown(WPARAM, LPARAM param);
//public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	CEdit _EditPwd;
	afx_msg void OnEnChangeEditPwd();
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
};

#pragma once
#include "afxwin.h"
#include "BtnST.h"
#include "GradientStatic.h"
#include <vector>
#include <map>

// VtChartTimeToolBar dialog
class VtChartContainer;
class VtSystem;
class VtChartTimeToolBar : public CDialogEx
{
	DECLARE_DYNAMIC(VtChartTimeToolBar)

public:
	VtChartTimeToolBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtChartTimeToolBar();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIME_TOOLBAR };
#endif

	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	VtChartContainer* Container() const { return _Container; }
	void Container(VtChartContainer* val) { _Container = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButtonST _BtnDay;
	CButtonST _BtnMin;
	CButtonST _BtnMonth;
	CButtonST _BtnTick;
	CButtonST _BtnWeek;
	CComboBox _ComboTick;
	CComboBox _ComboTime;
	CButtonST _SearchSymbol;
	CComboBox _ComboSymbol;
	std::vector<CButtonST*> _ButtonVec;

private:
	int _Mode;
	VtChartContainer* _Container;
public:
	CGradientStatic _StaticTime;
	afx_msg void OnBnClickedBtnMonth();
	afx_msg void OnBnClickedBtnWeek();
	afx_msg void OnBnClickedBtnDay();
	afx_msg void OnBnClickedBtnMin();
	afx_msg void OnBnClickedBtnTick();
	afx_msg void OnCbnSelchangeComboTime();
	CComboBox _ComboSystem;
	CGradientStatic _StaticSystem;
	void InitSystem(std::vector<VtSystem*>& sysVector);
	afx_msg void OnCbnSelchangeComboSystem();
};

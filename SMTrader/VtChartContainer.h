#pragma once
#include <vector>
#include "SSplitter.h"
#include <set>
#include "Drawing/VtDrawingConstants.h"
#include "Xml/pugixml.hpp"
#include "VtChartDataRequest.h"
#include "VtChartFavoriteGrid.h"
#include "HdWindowEvent.h"
#include "Poco/BasicEvent.h"
#include "VtChartTimeToolBar.h"
#include "SimpleBinStream.h"
#include "Chart/SmChartDefine.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

using Poco::BasicEvent;
// VtChartContainer dialog
class VtChartWindow;
class VtSystem;
class SmChartData;
class VtChartContainer : public CDialogEx
{
	DECLARE_DYNAMIC(VtChartContainer)

public:
	VtChartContainer(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtChartContainer();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHART_CONTAINER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

public:
	void ChangeChartData(VtSymbol* symbol, SmChartType chart_type, int cycle);
	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(const VtSymbol* symbol);

	void RegisterChartCallback();
	void UnregisterChartCallback();
	void OnChartEvent(const SmChartData* chart_data);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	VtChartWindow* ActiveChartWnd() const { return _ActiveChartWnd; }
	void ActiveChartWnd(VtChartWindow* val) { _ActiveChartWnd = val; }
	int CurMode() const { return _CurMode; }
	void CurMode(int val) { _CurMode = val; }

	void OnChartPattern(UINT id);
	void OnChartIndex(UINT id);
	void OnChartCompare(UINT id);
	std::vector<VtChartWindow*>& ChartList()
	{
		return _ChartList;
	}

	void InsertIndex(CMFCPropertyGridProperty* prop);
	VtChartWindow* AddChart();
	void AddChart(int mode);
	void AddChart(std::vector<std::pair<std::string, VtMainChartType>>& keyList, int mode);
	std::vector<CSSplitter*> _SplitVector;
	//std::vector<VtChartWindow*> _ChartList;
	std::vector<VtChartWindow*> _ChartList;
	//std::set<VtChartWindow*> _ChartSet;
	void ClearSplitters();
	void StartByOne();
	void RefreshSplitters();

	void SetActiveChartWindow(VtChartWindow* chart);
	void ClearEvent();
	void ShowChartCtrl();
	void SetDocTitle(std::string title);

	void SetEditOption(EditOption a_EditOption);
	void DeleteSelectedObject();
	void DeleteAllObjects();
	afx_msg void OnDestroy();
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
	void SetChartData(std::vector<VtChartDataRequest> reqList);
	void ShowRefChart(std::string symCode, bool show);
	BasicEvent<HdWindowEventArgs> _WindowEvent;
	void FireWindowEvent(HdWindowEventArgs&& arg)
	{
		_WindowEvent(this, arg);
	}
private:
	void StartByOneByTwo();
	void StartByTwoByTwo();
	void AddChartOneByOne();
	void AddChartOneByTwo();
	void AddChartTwoByOne();
	void AddChartTwoByTwo();
	void AddChartFourByOne();
	void AddChartFourByTwo();
	void AddChartOneByFour();
	void AddChartEightByOne();
	void AddChartOneByEight();
	void AddChartFourByFour();

	void AddChartOneByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList);
	void AddChartTwoByOne(std::vector<std::pair<std::string, VtMainChartType>>& keyList);
	void AddChartTwoByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList);
	void AddChartFourByOne(std::vector<std::pair<std::string, VtMainChartType>>& keyList);
	void AddChartFourByTwo(std::vector<std::pair<std::string, VtMainChartType>>& keyList);


	void CopyLegacyChart(std::vector<VtChartWindow*>& chartList);
	int _CurMode = 0;

	CWnd* _MainWnd = nullptr;
	VtChartWindow*  _ActiveChartWnd = nullptr;
	VtChartWindow* CreateChartWindow();
	void CreateChartByMode(int mode);
public:
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnReceiveQuoteHd(VtSymbol* sym);
	void OnReceiveChartData(VtChartData* data);
	void SetSelectedChartData(std::string selectedCode);
	void ChangeChartTime(int newTime);
private:
	int _GridWidth;
	int _ToolBarHeight;
	VtChartFavoriteGrid _FavGrid;
	virtual void PostNcDestroy();
	VtChartTimeToolBar _TimeToolBar;
public:
	afx_msg void OnNcDestroy();
	void InitSystem();
	void ChangeSystem(VtSystem* newSystem);

private:
	std::set<double> _DateTimeSet;
	void SetDateTime(VtChartData* chartData);
};

#pragma once
#include "Chart/ChartViewer.h"
#include "Drawing/VtDrawingConstants.h"
#include "afxwin.h"
#include "Global/VtDefine.h"
#include <string>
#include <vector>
#include <map>
#include <locale.h>
#include "VtUniqueChartID.h"
#include "afxbutton.h"
#include "VtChartDataRequest.h"
#include "FinanceChart.h"
#include "VtChartData.h"
#include "Drawing/VtUniqueID.h"
#include "Drawing/VtDrawingConstants.h"
#include "Drawing/VtPoint.h"
#include "Drawing/VtShapesCollection.h"
#include "VtIndexDefine.h"
#include "VtReferenceChart.h"
#include "Xml/pugixml.hpp"
#include "Sound/Pipe.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

#define WM_CHARTDATA_RECEIVED (WM_USER + 1000)
#define WM_CHARTDATA_UPDATED (WM_USER + 1001)
// VtChartWindow dialog
class VtChartFrm;
class VtChartData;
class VtChartContainer;
class CSSplitter;
class VtSymbol;
class HdChartFrm;
class VtColorManager;
class VtChartWindow : public CDialogEx, public VtUniqueChartID
{
	DECLARE_DYNAMIC(VtChartWindow)

public:
	VtChartWindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtChartWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHART };
#endif

	VtChartContainer* ParentView() const { return _ParentView; }
	void ParentView(VtChartContainer* val) { _ParentView = val; }
	int CtrlHeight();
	void CtrlHeight(int val) { _CtrlHeight = val; }
	//HdChartFrm* ChartFrm() const { return _ChartFrm; }
	//void ChartFrm(HdChartFrm* val) { _ChartFrm = val; }
	std::string ChartDataKey() const { return _ChartDataKey; }
	void ChartDataKey(std::string val) { _ChartDataKey = val; }
	std::string RealtimeDataKey() const { return _RealtimeDataKey; }
	void RealtimeDataKey(std::string val) { _RealtimeDataKey = val; }
	VtAttachDirection AttachDirection() const { return _AttachDirection; }
	void AttachDirection(VtAttachDirection val) { _AttachDirection = val; }
	void ShowChartCtrl();
	void ShowChartCtrl(bool flag);
	bool Active() const { return _Active; }
	void Active(bool val) { _Active = val; }
	std::string SelectedDataSetName() const { return _SelectedDataSetName; }
	void SelectedDataSetName(std::string val);
	VtSystem* System() const { return _System; }
	void System(VtSystem* val) { _System = val; }
	int SignalSize() const { return _SignalSize; }
	void SignalSize(int val) { _SignalSize = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

	void InsertIndex(CMFCPropertyGridProperty* prop);
	void CallTest();
	void OnReceiveChartData(VtChartData* data);
	void OnReceiveQuoteHd(VtSymbol* sym);
	void OnReceiveRefData(VtChartData* data);
	void OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose = 0);
	void ClearResource();
	void DeleteRefChartData();
	void UnRegisterData(VtChartData* data);
	void MainChartType(VtMainChartType val);
	void UseMainAxis(bool use);
public:
	CChartViewer m_ChartViewer;
	CSSplitter* ParentSplit = nullptr;
	void RecalcLayout();
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	void RegisterRealtimeDataRequest(VtChartData* data);
	void RegisterCyclicDataRequest(VtChartData* data);
	std::string _ChartDataKey;
	std::string _RealtimeDataKey;
	int _CtrlHeight = 24;
	int _ButtonWidth = 24;
	int _EditWidth = 60;
	bool _Active = true;
	bool _FullScreen = false;
	double _CurViewPortLeft = 0.0;
	double _CurViewPortRight = 1.0;
	//HdChartFrm* _ChartFrm = nullptr;
	// Moves the scroll bar when the user clicks on it
	double moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// Update controls when the view port changed
	void updateControls(CChartViewer *viewer);
	// Adjust the size of the chart control.
	void AdjustChartSize();

	void AdjustChartToCount(int count);

	VtChartContainer* _ParentView = nullptr;
	VtAttachDirection _AttachDirection;
public:
	afx_msg LRESULT OnChartDataUpdated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChartDataReceived(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewPortChanged();
	afx_msg void OnMouseMovePlotArea();
	afx_msg void OnMouseLButtonDownOverChart();
	afx_msg void OnMouseRButtonDownOverChart();
	afx_msg void OnMouseLButtonUpOverChart();
	afx_msg void OnMouseMoveOverChart();
	afx_msg void OnMouseLButtonDblClkOverChart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CScrollBar _HScrollBar;
	//CSkinScrollBar _HScrollBar;
	//HBITMAP	m_hBmpScrollBar;
	void SetChartData(VtChartData* chartData);
	VtChartData* GetChartData();
	void SetEditOption(EditOption a_EditOption);
	void DeleteSelectedObject();
	void DeleteAllObjects();
	void Undo();
	void Redo();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit _EditCount;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	void OnChartPattern(UINT id);
	void OnChartIndex(UINT id);
	//void OnChartCompare(UINT id);
	void AddCompareData(int startAfter, VtChartData* data);
	void AddCompareData(VtChartData* data);
	void AddCompareData(VtChartDataRequest req);
	afx_msg void OnCompacting(UINT nCpuTime);
	afx_msg void OnClose();
	afx_msg void OnBnClickedChartBtnFullscrn();
	CMFCButton _BtnDelChart;
	CMFCButton _BtnFullScreen;
	CMFCButton _BtnZoomin;
	CMFCButton _BtnZoomout;
	afx_msg void OnBnClickedChartBtnZoomin();
	afx_msg void OnBnClickedChartBtnZoomout();
	afx_msg void OnBnClickedChartBtnDel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

public:
	void DrawChart(CChartViewer* a_pChartViewer, int mode);
	void SetDefaultChartData();
	void SetDefaultRefChartData();
	std::vector<VtReferenceChart*>& GetRefChartVector()
	{
		return RefChartVector;
	}
	std::vector<VtChartData*> GetChartDataList();
	void ShowChart(std::string symCode, bool show);
	void StopChartDataEvent();
	void SetSelectedChartData(std::string selectedCode);
	void ChangeChartTime(int newTime);
	void ChangeSystem(VtSystem* newSystem);
private:
	VtReferenceChart* FindRefChart(std::string symCode);
	//VtMainChartType _MainChartType = VtMainChartType::CandleStick;
	int _Index = 0;
	//CChartViewer* _ChartViewer = nullptr;
	FinanceChart* _SourceChart = nullptr;
	XYChart*      _MainChart = nullptr;
	int _NoOfPoints = 0;
	int _BottomMargin = 70;
	// Attributes
	double _MinPoints = 1.0;
	int _XtraPoints = 0;
	int _Height = 800;
	int _Width = 600;
	int _OriginalHeight = 0;
	int _YAxisGap = 30;
	VtChartData* _Data = nullptr;
	bool _Init = false;

	int startIndex = 0;
	int endIndex = 0;

	bool _ShowAll = true;

	std::string _ChartTile;

	void GetZoomRange(CChartViewer * a_pChartViewer, int& start, int& end);
	int CalcChartHeight();
	int GetMaxDataCount();

	void initChartViewer(CChartViewer *viewer);

	std::vector<VtLayerInfo> _LayerList;
	LineLayer *_MainLayer = nullptr;

	VtColorManager* _ColorMgr;

	CPoint _MouseClickPoint;

	void DrawTitleValue(DrawArea* drawArea);

	VtChartData* _SelectedData = nullptr;

	void TrackFinance(MultiChart *m, int mouseX);

	Axis* FindAxis(std::string dataName);


	std::string _SelectedDataSetName;
	bool _UseMainAxis = false;
	//std::map<std::string, VtChartData*> _CompareDataMap;
	int  _ZoomStartIndex = 0;
	int  _StartID = 0;
	bool _RedimStarted = false;
	bool _multiLineStarted = false;
	bool _isMouseLButtonDown = false;
	VtPoint _start;
	VtPoint _temp;
	VtPoint _offset;
	EditOption _editOpt = EditOption::None;
	EditStatus _editStatus = EditStatus::None;
	RedimStatus _redimStatus = RedimStatus::Idle;

	VtShapesCollection _shapesCollection;
	void MouseToChart(CPoint src, VtPoint& des);
	VtSymbol* _SelSymbol = nullptr;
	void SetEditStatus(EditStatus status)
	{
		_editStatus = status;
	}

	VtColor _creationFillColor;
	VtColor _creationPenColor;
	int _creationPenWidth = 1;
	bool _isCreationFilled = true;
	std::vector<VtPoint> _penPoints;
	VtPoint _fiboPoints[6];
	void DrawRealtimeValues(DrawArea* d);
	void DrawCurrentValue(VtLayerInfo&& layerInfo, DrawArea* drawArea);
	std::vector<VtReferenceChart*> RefChartVector;

	void ValueToPixel();
	void PixelToValue();

	void SetDataCount();

	//void DrawRefChart();
	void DrawRefChart(std::vector<VtLayerInfo>& layerList);
	void DrawRealtimeValues(DrawArea* d, std::vector<VtLayerInfo>& layerList);
	Axis* FindAxis(std::string dataName, std::vector<VtLayerInfo>& layerList);

	bool _Drawing = false;

	void HitTest(CPoint point);
	void ShowAllReferenceChart(bool flag);
	std::pair<int, VtReferenceChart*> RefChartIndex(std::string symCode);
	int RefChartIndexBySel(std::string selString);
	void SetSelectedData(std::string selString);
	bool _ShowMainChart;

	std::vector<VtChartDataRequest> _ChartDataReqVector;

	VtSystem* _System;
	int _SignalSize = 2;
	bool _SelectedSystem = false;
	void DrawSignal(DoubleArray& timeStamp, DoubleArray& highArray, DoubleArray& lowArray, DrawArea* drawArea);
	bool HitTestSystem(CPoint point);
	void ShowSystemProperty(VtSystem* system);
};

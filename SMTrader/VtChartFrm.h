#pragma once
#include "FinanceChart.h"
#include "VtChartData.h"
#include "Drawing/VtUniqueID.h"
#include "Drawing/VtDrawingConstants.h"
#include "Drawing/VtPoint.h"
#include "Drawing/VtShapesCollection.h"
#include <vector>
#include <string>
#include <map>
#include "Global/VtDefine.h"
#include "VtUniqueChartID.h"
#include <locale.h>
#include "VtIndexDefine.h"
#include "VtReferenceChart.h"
#include "VtChartDataRequest.h"
#include <string>
#include "Xml/pugixml.hpp"
#include "Sound/Pipe.h"

class CChartViewer;
class VtSymbol;
class CWnd;
struct VtOrder;
class VtChartOrderManager;
class VtChartWindow;
class VtSystem;
class VtColorManager;

class VtBaseIndex;
class VtBaseStress;
class VtBaseBand;
//const int StressLineWidth = 5;
class VtChartFrm : public VtUniqueChartID
{
public:
	VtChartFrm();
	~VtChartFrm();
	void ChangeChartData(VtChartDataRequest&& req, std::string newKey);
	std::string GetMainDataKey();
	int Height() const { return _Height; }
	void Height(int val) { _Height = val; }
	int Width() const { return _Width; }
	void Width(int val) { _Width = val; }
	VtChartData* Data() const { return _Data; }
	void Data(VtChartData* val) { _Data = val; }
	int XtraPoints() const { return _XtraPoints; }
	void XtraPoints(int val) { _XtraPoints = val; }
	double MinPoints() const { return _MinPoints; }
	void MinPoints(double val) { _MinPoints = val; }
	EditOption EditOpt() const { return _editOpt; }
	void EditOpt(EditOption val) { _editOpt = val; }
	//CWnd* Parent() const { return _Parent; }
	//void Parent(CWnd* val) { _Parent = val; }
	CChartViewer* ChartViewer() const { return _ChartViewer; }
	void ChartViewer(CChartViewer* val) { _ChartViewer = val; }
	std::string ChartTile() const { return _ChartTile; }
	void ChartTile(std::string val) { _ChartTile = val; }
	VtChartOrderManager* ChartOrderManager() const { return _ChartOrderManager; }
	void ChartOrderManager(VtChartOrderManager* val) { _ChartOrderManager = val; }
	VtChartWindow* ParentChartWnd() const { return _ParentChartWnd; }
	void ParentChartWnd(VtChartWindow* val) { _ParentChartWnd = val; }
	int NoOfPoints() const { return _NoOfPoints; }
	void NoOfPoints(int val) { _NoOfPoints = val; }
	VtMainChartType MainChartType() const { return _MainChartType; }
	void MainChartType(VtMainChartType val) { _MainChartType = val; }
	int CalcChartHeight();
	void DrawCurrentValue(double close, double usd, DrawArea* drawArea);
	void DrawRealtimeValues(DrawArea* d);
	void DrawCurrentValue(VtLayerInfo&& layerInfo, DrawArea* drawArea);
	void ResetChart();
	std::vector<VtReferenceChart *>* RefChartVectro() { return &RefChartVector; }
	void DrawTitleValue(DrawArea* drawArea);
	bool UseMainAxis() const { return _UseMainAxis; }
	void UseMainAxis(bool val) { _UseMainAxis = val; }

	void SetSystem(VtSystemType sysType);
	int SignalSize() const { return _SignalSize; }
	void SignalSize(int val) { _SignalSize = val; }
	bool SelectedSystem() const { return _SelectedSystem; }
	void SelectedSystem(bool val) { _SelectedSystem = val; }
	VtSystem* System() const { return _System; }
	void System(VtSystem* val) { _System = val; }
	void LoadSystem();
	void UpdateSystem(VtChartData* data, int count);
	void UpdateIndex(VtBaseIndex* index);
	void InsertIndex(VtIndexBase* index);
	void DeleteIndex(int num);
	void DeleteSystem();

	void InsertBand(VtBandBase* band);
	void UpdateBand(VtBaseBand* band);
	void DeleteBand(int num);

	void InsertStress(VtStressBase* stress);
	void UpdateStress(VtBaseStress* stress);
	void DeleteStress(int num);

	void UpdateReferenceChart(VtReferenceChart* refChart);
	

	std::vector<VtBaseIndex*> IndexVector;
	std::vector<VtBaseBand*> BandVector;
	std::vector<VtBaseStress*> StressVector;
	std::vector<VtReferenceChart*> RefChartVector;
	//std::vector<VtChartData*> _CompareDataVector;

	void DrawBand();
	void DrawStress(DrawArea* drawArea);
	void DrawDailyLine(DrawArea* d);
	void DrawRefChart();

	void RemoveChartWindow();
	
	VtSystemType CurSystemType() const { return _CurSystemType; }
	void CurSystemType(VtSystemType val) { _CurSystemType = val; }
private:
	CPipe _SoundPipe;
	void PlaySignalSound(VtPositionType pos);
	void Reset();
	void ReleaseResource();
	std::map<std::string, std::string> _ChangingDataMap;
	bool _ShowDailyLine = false;
	VtSystem* _TempSystem = nullptr;
	void InitMaList();
	void DrawVrobo1Stress(DrawArea* drawArea);
	void DrawVrobo2Stress(DrawArea* drawArea);
	VtChartData* FindCompareData(int index);
	void ShowProperty();
	VtSystemType _CurSystemType;
	void OnSystemProperty();
	bool _SelectedSystem = false;
	int CalcTotalIndexHeight();
	int CalcTotalRefChartHeight();
	void DrawIndex(std::vector<VtLayerInfo>& layerList);
	int _SignalSize = 2;
	void DrawSignal(DoubleArray& timeStamp, DoubleArray& highArray,DoubleArray& lowArray, DrawArea* drawArea);
	VtSystem* _System = nullptr;

	bool FindCompareData(VtChartData* data);
	void AdjustLastData();
	int GetMaxDataCount();
	bool _SelectedChart = false;
	CPoint _MouseClickPoint;
	double _RealTimeClose = 0.0;
	double _RealTimeCloseUSD = 0.0;

	//int CalcChartHeight();
	VtMainChartType _MainChartType = VtMainChartType::CandleStick;
	int _Index = 0;
	CChartViewer* _ChartViewer = nullptr;
	//CWnd*         _Parent = nullptr;
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
	void GetZoomRange(CChartViewer * a_pChartViewer, int& start, int& end);
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
	void ReloadSystem(VtSystem* sys);
	void OnDeleteKey();
	// 차트 데이터는 fullCode를 가지고 있다.
	VtChartData* GetChartData()
	{
		return _Data;
	}
	void OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose = 0);
	void DrawChart(CChartViewer* a_pChartViewer, int mode);
	void SetChartData(VtChartData* a_ChartData);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);

	static void PixelToValue(VtPoint& location0, VtPoint& location1, bool byValue);
	static void ValueToPixel(VtValue& value0, VtValue& value1, bool byValue);

	void DeleteSelectedObject();
	void DeleteAllObjects();
	void Undo();
	void Redo();
	void SetChartSize(int width, int height);

	void AddCompareData(VtChartData* data);
	void RemoveCompareData(std::string key);
	void RemoveCompareData(VtChartData* data);

	bool HitTestSymbol(CPoint point);
	void HitTest(CPoint point);
	bool HitTestSystem(CPoint point);
	void DeleteRefChartData();
	void InsertIndex(CMFCPropertyGridProperty* prop);
	void InsertSystem(int index);
	// For Drawing
private:
	bool _ShowAll = true;
	void ShowAllIndex(bool flag);
	void ShowAllReferenceChart(bool flag);
	VtBaseIndex* FindIndex(std::string indexName);
	VtBaseBand* FindBand(std::string bandName);
	VtBaseStress* FindStress(std::string stressName);
	int startIndex = 0;
	int endIndex = 0;
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

	void DrawInstantObject();
	void DrawObjects(int idStart, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	void DrawObjects(XYChart* chart, DrawArea* d);
	void DrawSelection(DrawArea* d);
	void DrawCreatingObject(DrawArea* d);
	void ChangeDimension();
public:
	void ValueToPixel();
	void PixelToValue();


	bool Init() const { return _Init; }
	void Init(bool val) { _Init = val; }
	double m_CurViewPortLeft = 0.0;
	double m_CurViewPortRight = 1.0;

	// Initialize the CChartViewer
	void initChartViewer(CChartViewer *viewer);

private:
	std::string _ChartTile;
	void CreateLineWithOrder(VtOrder* order);
public:
	void PutOrder(POINT& pt, VtOrderCmd cmd, int Qty);
	void AddPreOrder(VtOrder* order);
	void OnAccepted(VtOrder* order);
	void OnFilled(VtOrder* order);
	void OnConfirmModify(VtOrder* order);
	void OnConfirmCancel(VtOrder* order);
	/// <summary>
	/// 모든 주문이 전송되고 최초로 서버에서 주문번호와 함께 응답이 왔을 때 처리
	/// </summary>
	/// <param name="order"></param>
	void OrderArrived(VtOrder* order);
private:
	VtChartOrderManager* _ChartOrderManager = nullptr;
	void ChangeOrder(int yPos, VtElement* selObj);

	VtChartWindow* _ParentChartWnd = nullptr;
	void InitChartIndex();

	VtColorManager* _ColorMgr;
public:
	std::map<VtChartIndex, bool> ChartIndexMap;
	void ShowChartIndex(int index);
	void DeleteAllChartIndex();
	void OnReceiveChartData(VtChartData* data);
	void OnReceiveQuoteHd(VtSymbol* sym);

	int MACDHeight = 180;
	int StocasticHeight = 180;
	int RSIHeight = 180;
	int TRIXHeight = 180;
	std::vector<VtLayerInfo> _LayerList;
	LineLayer *_MainLayer = nullptr;
	void DrawSelected(DoubleArray& xData, DoubleArray& yData, XYChart* chart);
	void DrawSelected(DoubleArray& xData, DrawArea* d, int startIndex, int nofPoints);
	void trackFinance(MultiChart *m, int mouseX);
	void TrackFinance(MultiChart *m, int mouseX);
	void DrawTrackLine(MultiChart*m, int mouseX);
	Axis* FindAxis(std::string dataName);
	VtChartData* _SelectedData = nullptr;
};

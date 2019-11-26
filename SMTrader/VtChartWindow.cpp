// VtChartWindow.cpp : implementation file
//

#include "stdafx.h"
#include "VtChartWindow.h"
#include "afxdialogex.h"
#include <string>
#include "VtChartData.h"
#include "VtChartFrm.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "MainFrm.h"
#include "VtChartWindowManager.h"
#include "VtChartDataRequest.h"
#include "SSplitter.h"
#include "VtChartDataManager.h"
//#include "VcExchangeManager.h"
#include "HdChartFrm.h"
#include "VtReferenceChart.h"
#include "resource.h"

#include "chartdir.h"
#include "Chart/ChartViewer.h"
#include <time.h>
#include "Global/VtDefine.h"
#include "Drawing/VtElement.h"
#include "MainFrm.h"
#include "resource.h"
#include "Drawing/VtFibo.h"
#include "Util/formatstdstring.h"
#include <math.h>
#include "Drawing/VtLine.h"
#include "Drawing/VtHLine.h"
#include "Drawing/VtVLine.h"
#include "Drawing/VtRect.h"
#include "Drawing/VtEllipse.h"
#include "Drawing/VtMultiLine.h"
#include "Poco/NumberFormatter.h"
#include "VtSymbolManager.h"
#include "VtOrder.h"
#include "VtChartOrderManager.h"
#include "VtChartWindow.h"
#include "VtGlobal.h"
#include "String.h"
#include "VtSymbol.h"
#include "XFormatNumber.h"
#include "conversion.hpp"
#include "System/VtSystemManager.h"
#include "System/VtSystem.h"
#include "System/VtMaSystem.h"
#include "System/VtSignal.h"
#include "VtIndexFactory.h"
#include "VtIndexDefine.h"
#include "VtBaseIndex.h"
#include "VtMaIndex.h"
#include "VtVolumeIndex.h"
#include "VtBollingerBandIndex.h"
#include "VtPriceChannelIndex.h"
#include "VtMacdIndex.h"
#include "VtStochasticIndex.h"
#include "VtRsiIndex.h"
#include "VtTrixIndex.h"
#include "VtAtrIndex.h"
#include "VtNDailyBand.h"
#include "VtDailyBand.h"
#include "VtEntireBand.h"
#include "VtOrbBand.h"
#include "VtBaseStress.h"
#include "VtChartDataManager.h"
#include "VtChartWindowManager.h"
#include "VtTaskManager.h"
#include "VtRobo1Stress.h"
#include "VtSimpleMaStress.h"
#include "ZmConfigManager.h"
#include "System/VtSystemManager.h"
#include "VtColorManager.h"
#include <mmsystem.h>
#include "VtColorManager.h"
#include "VtChartContainer.h"
#include "VtChartData.h"
#include "VtRealtimeRegisterManager.h"
#include "VtHdCtrl.h"
#include "VtSystemProperty.h"
#include <algorithm>
#include <iterator>
#include "VtProductCategoryManager.h"
using namespace convert;
using Poco::NumberFormatter;

// VtChartWindow dialog

IMPLEMENT_DYNAMIC(VtChartWindow, CDialogEx)

VtChartWindow::VtChartWindow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CHART, pParent)
{
	_ParentView = nullptr;

	_Active = true;

	_ColorMgr = new VtColorManager();

	_SelectedData = nullptr;

	_SourceChart = nullptr;

	_Drawing = false;

	_ShowMainChart = true;

	_System = nullptr;

	_SelectedSystem = false;

}

VtChartWindow::~VtChartWindow()
{
	if (_SourceChart)
	{
		delete _SourceChart;
		_SourceChart = nullptr;
	}
	
	if (_ColorMgr)
	{
		delete _ColorMgr;
		_ColorMgr = nullptr;
	}
}

int VtChartWindow::CtrlHeight()
{
	if (_Active)
		return 19;
	else
		return 0;
}

void VtChartWindow::ShowChartCtrl()
{
	_Active = !_Active;
	if (_Active) {
		_EditCount.ShowWindow(SW_SHOW);
		_HScrollBar.ShowWindow(SW_SHOW);
		_BtnDelChart.ShowWindow(SW_SHOW);
		_BtnFullScreen.ShowWindow(SW_SHOW);
		_BtnZoomin.ShowWindow(SW_SHOW);
		_BtnZoomout.ShowWindow(SW_SHOW);
	} else {
		_EditCount.ShowWindow(SW_HIDE);
		_HScrollBar.ShowWindow(SW_HIDE);
		_BtnDelChart.ShowWindow(SW_HIDE);
		_BtnFullScreen.ShowWindow(SW_HIDE);
		_BtnZoomin.ShowWindow(SW_HIDE);
		_BtnZoomout.ShowWindow(SW_HIDE);
	}

	AdjustChartSize();
}

void VtChartWindow::ShowChartCtrl(bool flag)
{
	_Active = flag;
	if (flag)
	{
		_EditCount.ShowWindow(SW_SHOW);
		_HScrollBar.ShowWindow(SW_SHOW);
		_BtnDelChart.ShowWindow(SW_SHOW);
		_BtnFullScreen.ShowWindow(SW_SHOW);
		_BtnZoomin.ShowWindow(SW_SHOW);
		_BtnZoomout.ShowWindow(SW_SHOW);
	}
	else
	{
		_EditCount.ShowWindow(SW_HIDE);
		_HScrollBar.ShowWindow(SW_HIDE);
		_BtnDelChart.ShowWindow(SW_HIDE);
		_BtnFullScreen.ShowWindow(SW_HIDE);
		_BtnZoomin.ShowWindow(SW_HIDE);
		_BtnZoomout.ShowWindow(SW_HIDE);
	}

	AdjustChartSize();
}

void VtChartWindow::SelectedDataSetName(std::string val)
{
	_SelectedDataSetName = val;
}

void VtChartWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_CHART_HSCRL, _HScrollBar);
	DDX_Control(pDX, IDC_STATIC_CHART, m_ChartViewer);
	DDX_Control(pDX, ID_CHART_EDIT_COUNT, _EditCount);
	DDX_Control(pDX, ID_CHART_BTN_DEL, _BtnDelChart);
	DDX_Control(pDX, ID_CHART_BTN_FULLSCRN, _BtnFullScreen);
	DDX_Control(pDX, ID_CHART_BTN_ZOOMIN, _BtnZoomin);
	DDX_Control(pDX, ID_CHART_BTN_ZOOMOUT, _BtnZoomout);
}


void VtChartWindow::Save(simple::file_ostream<same_endian_type>& ss)
{

}

void VtChartWindow::Load(simple::file_istream<same_endian_type>& ss)
{

}

void VtChartWindow::InsertIndex(CMFCPropertyGridProperty* prop)
{
// 	if (_ChartFrm)
// 		_ChartFrm->InsertIndex(prop);
}

void VtChartWindow::CallTest()
{

}

void VtChartWindow::OnReceiveChartData(VtChartData* data)
{
	if (!data)
		return;
	SyncDateTime(data);

	VtChartDataManager* dataMgr = VtChartDataManager::GetInstance();
	dataMgr->FindDataBorderQueue(data, data->TimeInfo.data());
	if (_System)
	{
		_System->SetDataMap(data);
	}
	initChartViewer(&m_ChartViewer);
	DrawChart(&m_ChartViewer, 0);
	if (_ParentView)
	{
		_ParentView->OnReceiveChartData(data);
	}
	
}

void VtChartWindow::SyncDateTime(VtChartData* data)
{
	if (!data || data->InputDateTimeMap.size() == 0 || data->SyncDateTimeMap.size() == 0)
		return;

	// 데이타임셋이 비어 있을 때는 먼저 채운다.
	if (_DateTimeSet.size() == 0) {
		for (auto it = data->SyncDateTimeMap.begin(); it != data->SyncDateTimeMap.end(); ++it) {
			_DateTimeSet.insert(it->first);
		}
	}

	// 새로 추가된 데이타임 키만 복사해 준다.
	for (auto it = data->InputDateTimeMap.begin(); it != data->InputDateTimeMap.end(); ++it) {
		_DateTimeSet.insert(it->first);
	}

	// 키가 차트 전체 데이터 갯수를 넘으면 앞에 있는 키를 제거해 준다.
	int count = _DateTimeSet.size() - ChartDataSize;
	if (count > 0) {
		auto it = _DateTimeSet.begin();
		int curCount = 0;
		while (curCount < count) {
			_DateTimeSet.erase(it++);
			curCount++;
		}
	}

	ChartDataItemMap newDataMap;
	int addedCnt = 0; // 새롭게 추가된 데이터의 갯수
	// 기존 데이터를 복사해 온다. 데이타임 키에 없는 값들은 값없음으로 표시한다.
	for (auto rit = _DateTimeSet.rbegin(); rit != _DateTimeSet.rend(); ++rit) {
		auto key = *rit;
		VtChartDataItem item;

		// 현재 차트에서 날짜를 찾으면 값을 복사해 온다.
		auto chart_itr = data->SyncDateTimeMap.find(key);
		if (chart_itr !=  data->SyncDateTimeMap.end()) {
			// 기존데이터를 불러온다.
			item = data->GetChartData(chart_itr->second);
			// 기존 인덱스를 가져온다.
			item.Index = chart_itr->second;
		}
		else {
			VtDate date = VtGlobal::GetDate(key.first);
			VtTime time = VtGlobal::GetTime(key.second);
			item.Date = date;
			item.Time = time;
			item.DateTime = Chart::chartTime(date.year, date.month, date.day, time.hour, time.min, time.sec);
			addedCnt++;
		}
		// 새데이터 맵에 넣어준다.
		newDataMap[key] = item;
	}

	// 싱크맵이 만들어지지 않았거나 추가된것이 없다면 아무일도 하지 않는다.
	if (data->SyncDateTimeMap.size() == 0 || addedCnt == 0)
		return;

	// 새로운 데이터를 복사해 온다.
	for (auto it = data->InputDateTimeMap.rbegin(); it != data->InputDateTimeMap.rend(); ++it) {
		VtChartDataItem item = data->GetInputChartData(it->second);
		item.Index = it->second;
		newDataMap[it->first] = item;
		// 현재 차트에서 날짜를 찾으면 루푸를 나간다.
		auto chart_itr = data->SyncDateTimeMap.find(it->first);
		if (chart_itr != data->SyncDateTimeMap.end()) {
			break;
		}
	}
	// 기존 데이타임 싱크맵을 삭제한다.
	data->SyncDateTimeMap.clear();
	// 데이타임싱크맵을 새로만들고 싱크된 새로운 데이터를 넣어준다.
	int i = ChartDataSize - newDataMap.size();
	for (auto it = newDataMap.begin(); it != newDataMap.end(); ++it) {
		data->SyncDateTimeMap[it->first] = i;
		data->SetChartData(i, it->second);
		i++;
	}
}

// in case of being received real time data.
void VtChartWindow::OnReceiveQuoteHd(VtSymbol* sym)
{
 	if (!sym)
 		return;
	DrawChart(&m_ChartViewer, 0);
	if (_ParentView)
	{
		_ParentView->OnReceiveQuoteHd(sym);
	}
}

void VtChartWindow::OnReceiveRefData(VtChartData* data)
{
	
}

void VtChartWindow::RecalcLayout()
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	CWnd* wnd = (CWnd*)GetDlgItem(ID_CHART_EDIT_COUNT);
	int leftWidth = _EditWidth + _ButtonWidth * 4;
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(rcWnd.Width() - leftWidth, rcWnd.Height() - _CtrlHeight, _EditWidth, _CtrlHeight, TRUE);

	wnd = (CWnd*)GetDlgItem(ID_CHART_BTN_ZOOMIN);
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(rcWnd.Width() - leftWidth + _EditWidth, rcWnd.Height() - _CtrlHeight, _ButtonWidth, _CtrlHeight, TRUE);

	wnd = (CWnd*)GetDlgItem(ID_CHART_BTN_ZOOMOUT);
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(rcWnd.Width() - leftWidth + _EditWidth + _ButtonWidth * 1, rcWnd.Height() - _CtrlHeight, _ButtonWidth, _CtrlHeight, TRUE);

	wnd = (CWnd*)GetDlgItem(ID_CHART_BTN_FULLSCRN);
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(rcWnd.Width() - leftWidth + _EditWidth + _ButtonWidth * 2, rcWnd.Height() - _CtrlHeight, _ButtonWidth, _CtrlHeight, TRUE);

	wnd = (CWnd*)GetDlgItem(ID_CHART_BTN_DEL);
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(rcWnd.Width() - leftWidth + _EditWidth + _ButtonWidth * 3, rcWnd.Height() - _CtrlHeight, _ButtonWidth, _CtrlHeight, TRUE);

	wnd = (CWnd*)GetDlgItem(ID_CHART_HSCRL);
	if (wnd->GetSafeHwnd())
		wnd->MoveWindow(0, rcWnd.Height() - _CtrlHeight, rcWnd.Width() - leftWidth, _CtrlHeight, TRUE);

}

void VtChartWindow::SaveToXml(pugi::xml_node& node)
{
// 	if (_ChartFrm)
// 		_ChartFrm->SaveToXml(node);
}

void VtChartWindow::LoadFromXml(pugi::xml_node& node)
{
// 	if (_ChartFrm)
// 		_ChartFrm->LoadFromXml(node);
}

BEGIN_MESSAGE_MAP(VtChartWindow, CDialogEx)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_CONTROL(CVN_ViewPortChanged, IDC_STATIC_CHART, OnViewPortChanged)
	ON_CONTROL(CVN_MouseMovePlotArea, IDC_STATIC_CHART, OnMouseMovePlotArea)
	ON_CONTROL(CVN_LButtonDownChart, IDC_STATIC_CHART, OnMouseLButtonDownOverChart)
	ON_CONTROL(CVN_LButtonUpChart, IDC_STATIC_CHART, OnMouseLButtonUpOverChart)
	ON_CONTROL(CVN_MouseMoveChart, IDC_STATIC_CHART, OnMouseMoveOverChart)
	ON_CONTROL(CVN_LButtonDblClk, IDC_STATIC_CHART, OnMouseLButtonDblClkOverChart)

	ON_WM_MOUSEHWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_CHARTDATA_RECEIVED, OnChartDataReceived)
	ON_MESSAGE(WM_CHARTDATA_UPDATED, OnChartDataUpdated)
	ON_WM_COMPACTING()
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_CHART_BTN_FULLSCRN, &VtChartWindow::OnBnClickedChartBtnFullscrn)
	ON_BN_CLICKED(ID_CHART_BTN_ZOOMIN, &VtChartWindow::OnBnClickedChartBtnZoomin)
	ON_BN_CLICKED(ID_CHART_BTN_ZOOMOUT, &VtChartWindow::OnBnClickedChartBtnZoomout)
	ON_BN_CLICKED(ID_CHART_BTN_DEL, &VtChartWindow::OnBnClickedChartBtnDel)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// VtChartWindow message handlers


void VtChartWindow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	RecalcLayout();
	AdjustChartSize();
}

void VtChartWindow::SetChartData(VtChartData* chartData)
{
	if (chartData == nullptr) return;

	_Data = chartData;
	_NoOfPoints = _Data->GetDataCount();
	_Data->AddChartWindow(this);
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	chartDataMgr->AddChartDataRequest(10, _Data);
}

VtChartData* VtChartWindow::GetChartData()
{
	return _Data;
}

void VtChartWindow::SetEditOption(EditOption a_EditOption)
{
	//_ChartFrm->EditOpt(a_EditOption);
}

void VtChartWindow::DeleteSelectedObject()
{
	//_ChartFrm->DeleteSelectedObject();
}

void VtChartWindow::DeleteAllObjects()
{
	//_ChartFrm->DeleteAllObjects();
}

void VtChartWindow::Undo()
{
	//_ChartFrm->Undo();
}

void VtChartWindow::Redo()
{
	//_ChartFrm->Redo();
}


void VtChartWindow::OnMouseMovePlotArea()
{
	// Get the focus to ensure being able to receive mouse wheel events
	m_ChartViewer.SetFocus();

	TrackFinance((MultiChart *)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
	m_ChartViewer.updateDisplay();
}

void VtChartWindow::OnMouseLButtonDownOverChart()
{
	if (_ParentView)
		_ParentView->SetActiveChartWindow(this);
	SetFocus();
	HitTest(m_ChartViewer.MousePoint);
	DrawChart(&m_ChartViewer, 0);
	OnLButtonDown(m_ChartViewer.nFlags, m_ChartViewer.MousePoint);
}

void VtChartWindow::OnMouseRButtonDownOverChart()
{
	if (_ParentView)
		_ParentView->SetActiveChartWindow(this);
	//_ChartFrm->OnLButtonDown(m_ChartViewer.nFlags, m_ChartViewer.MousePoint);
}

void VtChartWindow::OnMouseLButtonUpOverChart()
{
	OnLButtonUp(m_ChartViewer.nFlags, m_ChartViewer.MousePoint);
}

void VtChartWindow::OnMouseMoveOverChart()
{
// 	_ChartFrm->trackFinance((MultiChart *)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
// 	if (!_ChartFrm->Init())
// 		return;
// 
// 	m_ChartViewer.updateDisplay();
// 	_ChartFrm->OnMouseMove(m_ChartViewer.nFlags, m_ChartViewer.MousePoint);
	TrackFinance((MultiChart *)m_ChartViewer.getChart(), m_ChartViewer.getPlotAreaMouseX());
}

void VtChartWindow::OnMouseLButtonDblClkOverChart()
{
	//_ChartFrm->OnLButtonDblClk(m_ChartViewer.nFlags, m_ChartViewer.MousePoint);

	if (_SelectedSystem)
	{
		ShowSystemProperty(_System);
		return;
	}

	if (_SelectedDataSetName.length() > 0)
	{
		//ShowProperty();
		return;
	}

	int sx = _start.x;
	int sy = _start.y;
	//CalcPixelSize();
	int tmpX = m_ChartViewer.MousePoint.x - _offset.x;
	int tmpY = m_ChartViewer.MousePoint.y - _offset.y;

	/*
	switch (_editOpt)
	{
	case EditOption::MultiLine:
		if (_multiLineStarted == true)
		{
			_shapesCollection.AddMultiLine(sx, sy, tmpX, tmpY, _creationPenColor, _creationFillColor, _creationPenWidth, _penPoints);
			ChangeDimension();
		}
		break;
	default:
		SetEditStatus(EditStatus::None);
		break;
	}

	if (_multiLineStarted)
	{
		// 객체를 그리고 난 다음에는 무조건 선택 상태로 되돌린다.
		_isMouseLButtonDown = false;
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		//pMainWnd->ResetDrawing();
		_multiLineStarted = false;
	}

	// Save the coordinates of the elements in the collection of the shape.
	//_shapesCollection.PixelToValue(_MainChart);
	*/

	DrawChart(&m_ChartViewer, 0);
}

//
// The ViewPortChanged event handler. This event occurs if the user scrolls or zooms in or 
// out the chart by dragging or clicking on the chart. It can also be triggered by calling
// CChartViewer.updateViewPort.
//
void VtChartWindow::OnViewPortChanged()
{
	//TRACE(_T("OnViewPortChanged!\n"));
	// In addition to updating the chart, we may also need to update other controls that
	// changes based on the view port.
	updateControls(&m_ChartViewer);
	// Update the chart if necessary
	if (m_ChartViewer.needUpdateChart())
	{
		ValueToPixel();
		DrawChart(&m_ChartViewer, 0);
	}
}


void VtChartWindow::RegisterRealtimeDataRequest(VtChartData* data)
{
	if (!data)
		return;
	
// 	VtChartDataRequestManager* reqMgr = VtChartDataRequestManager::GetInstance();
// 		reqMgr->RegisterRealtimeChartData(data);
// 		VtChartDataManager* dataMgr = VtChartDataManager::GetInstance();
// 		dataMgr->RegisterWindow(data, this);
}

void VtChartWindow::RegisterCyclicDataRequest(VtChartData* data)
{
	if (!data)
		return;
// 	VtChartDataRequestManager* reqMgr = VtChartDataRequestManager::GetInstance();
// 	reqMgr->RegisterTimerChartDataRequest(data);
// 
}

void VtChartWindow::OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose /*= 0*/)
{
	
	try
	{
		std::string key = exc;
		key.append(_T(":"));
		key.append(shortCode);
		if (_RealtimeDataKey.compare(key) == 0)
		{
			//_ChartFrm->OnReceiveRealtimeData(exc, shortCode, krwClose, usdClose);
		}
	}
	catch (std::exception e)
	{
		std::string msg = e.what();
	}
	
}

void VtChartWindow::ClearResource()
{
	
}

void VtChartWindow::DeleteRefChartData()
{
	//_ChartFrm->DeleteRefChartData();
}

void VtChartWindow::UnRegisterData(VtChartData* data)
{
	if (!data)
		return;

	// 주기 데이터 요청 취소
	//VtChartDataRequestManager* reqMgr = VtChartDataRequestManager::GetInstance();
	//reqMgr->UnRegisterTimerChartDataRequest(data);
	// 실시간 데이터 요청 취소
	//reqMgr->UnRegisterRealtimeChartData(data);
	//VtChartDataManager* dataMgr = VtChartDataManager::GetInstance();
	//dataMgr->RemoveWindow(data, this);
}

void VtChartWindow::MainChartType(VtMainChartType val)
{
	//if (_ChartFrm)
	//	_ChartFrm->MainChartType(val);
}

void VtChartWindow::UseMainAxis(bool use)
{
	//_ChartFrm->UseMainAxis(use);
}

void VtChartWindow::OnReceiveFirstChartData(VtChartData* data)
{
	if (!data)
		return;
	// 새로 추가된 데이타임 키만 복사해 준다.
	for (auto it = data->InputDateTimeMap.begin(); it != data->InputDateTimeMap.end(); ++it) {
		_DateTimeSet.insert(it->first);
	}

	// 키가 차트 전체 데이터 갯수를 넘으면 앞에 있는 키를 제거해 준다.
	int count = _DateTimeSet.size() - ChartDataSize;
	if (count > 0) {
		auto it = _DateTimeSet.begin();
		int curCount = 0;
		while (curCount < count) {
			_DateTimeSet.erase(it++);
			curCount++;
		}
	}

	for (auto it = _ChartDataVec.begin(); it != _ChartDataVec.end(); ++it) {
		VtChartData* curData = *it;
		if (data != curData) {
			SyncDateTime(curData);
		}
	}
}

//
// Handle scroll bar events
//
double VtChartWindow::moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//
	// Get current scroll bar position
	//
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&info);

	//
	// Compute new position based on the type of scroll bar events
	//
	int newPos = info.nPos;
	switch (nSBCode)
	{
	case SB_LEFT:
		newPos = info.nMin;
		break;
	case SB_RIGHT:
		newPos = info.nMax;
		break;
	case SB_LINELEFT:
		newPos -= (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_LINERIGHT:
		newPos += (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_PAGELEFT:
		newPos -= info.nPage;
		break;
	case SB_PAGERIGHT:
		newPos += info.nPage;
		break;
	case SB_THUMBTRACK:
		newPos = info.nTrackPos;
		break;
	}
	if (newPos < info.nMin) newPos = info.nMin;
	if (newPos > info.nMax) newPos = info.nMax;

	// Update the scroll bar with the new position
	pScrollBar->SetScrollPos(newPos);

	// Returns the position of the scroll bar as a ratio of its total length
	return ((double)(newPos - info.nMin)) / (info.nMax - info.nMin);
}


//
// Initialize the CChartViewer 
//
void VtChartWindow::updateControls(CChartViewer *viewer)
{
	// In this demo, we need to update the scroll bar to reflect the view port position and
	// width of the view port.

	_HScrollBar.EnableWindow(viewer->getViewPortWidth() < 1);
	if (viewer->getViewPortWidth() < 1)
	{
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = 0x1fffffff;
		info.nPage = (int)ceil(viewer->getViewPortWidth() * (info.nMax - info.nMin));
		info.nPos = (int)(0.5 + viewer->getViewPortLeft() * (info.nMax - info.nMin)) + info.nMin;
		_HScrollBar.SetScrollInfo(&info);
	}
}

void VtChartWindow::AdjustChartSize()
{
	if (m_ChartViewer.GetSafeHwnd() == nullptr)
		return;

	CRect rcWnd;
	GetClientRect(rcWnd);
	int width = rcWnd.Width();
	int height = rcWnd.Height() - CtrlHeight();
	m_ChartViewer.MoveWindow(0, 0, width, height);
	_Width = width;
	_Height = CalcChartHeight();
	DrawChart(&m_ChartViewer, 0);
}



void VtChartWindow::AdjustChartToCount(int count)
{
	if (count < 5)
		return;

	int dataCount = _Data->GetDataCount();
	double width = 1.0;
	width =((double)count / (double)dataCount);
	if (width < m_ChartViewer.getZoomInWidthLimit())
		return;

	double left = 1 - width;

	if ((width != m_ChartViewer.getViewPortWidth()))
	{
		m_ChartViewer.setViewPortWidth(width);
		m_ChartViewer.setViewPortLeft(left);
		m_ChartViewer.updateViewPort(true, false);
	}
}

BOOL VtChartWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	_BtnDelChart.SetImage(IDB_BITMAP_OUT);
	CRect rcMargin(0, 0, 0, 0);
	_BtnDelChart.SetTextMargin(rcMargin);
	_BtnDelChart.SetWindowText(_T(""));
	ShowChartCtrl(true);
	m_ChartViewer.ChartWindow(this);
	SetDefaultChartData();
	SetDefaultRefChartData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT VtChartWindow::OnChartDataUpdated(WPARAM wParam, LPARAM lParam)
{
	
	return 1;
}

LRESULT VtChartWindow::OnChartDataReceived(WPARAM wParam, LPARAM lParam)
{
	
	return 1;
}

void VtChartWindow::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	// TODO: Add your message handler code here and/or call default
	if (!m_ChartViewer.isMouseOnPlotArea())
		return ;
	CString msg;
	double newVpWidth = m_ChartViewer.getViewPortWidth() * (zDelta > 0 ? 0.9 : 1 / 0.9);
	msg.Format(_T("newVpWidth = %lf\n"), zDelta > 0 ? 0.9 : 1 / 0.9);
	//TRACE(msg);
	

	// 휠을 아래로 내리면
	// 축소(Reduce)가 아닌 확대(Expand) 이므로 false 대입
	double left = m_ChartViewer.getViewPortLeft();
	double width = m_ChartViewer.getViewPortWidth();
	double unit = 0.05;
	if (zDelta < 0)
	{
		width -= unit;
		if (width < 0.05)
		{
			width = 0.05;
		}
	}
	else
	{

		width += unit;
		if (width >= 1.0)
		{
			width = 1.0;
		}
	}

	left = _CurViewPortRight - width;
	msg.Format(_T("left = %lf, newVpWidth = %lf \n"), left, width);
	//TRACE(msg);

	m_ChartViewer.setViewPortWidth(width);
	m_ChartViewer.setViewPortLeft(left);
	m_ChartViewer.updateViewPort(true, false);


	CString strCount;
	//strCount.Format(_T("%d"), _ChartFrm->NoOfPoints());
	//_EditCount.SetWindowText(strCount);
	DrawChart(&m_ChartViewer, 0);
}

void VtChartWindow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == nullptr)
		return;

	
	if (pScrollBar->m_hWnd == _HScrollBar.m_hWnd)
	{
		SCROLLINFO si;
		si.fMask = SIF_ALL;
		_HScrollBar.GetScrollInfo(&si, SIF_ALL);
		switch (nSBCode)
		{
		case SB_LINEUP:
			si.nPos--;
			break;
		case SB_LINEDOWN:
			si.nPos++;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		}
		if (si.nPos > (int)(si.nMax - si.nMin - si.nPage + 1)) si.nPos = si.nMax - si.nMin - si.nPage + 1;
		if (si.nPos < si.nMin) si.nPos = si.nMin;
		_HScrollBar.SetScrollInfo(&si);
	}

	
	CString msg;
	double newViewPortLeft = moveScrollBar(nSBCode, nPos, pScrollBar);
	msg.Format(_T("viewPorLeft = %lf, viewPortWidth = %lf \n"), newViewPortLeft, m_ChartViewer.getViewPortWidth());
	//TRACE(msg);
	// Update the view port if the scroll bar has really moved
	if (newViewPortLeft != m_ChartViewer.getViewPortLeft())
	{
		_CurViewPortLeft = moveScrollBar(nSBCode, nPos, pScrollBar);
		_CurViewPortRight = _CurViewPortLeft + m_ChartViewer.getViewPortWidth();
		msg.Format(_T("really moved  >>> viewPorLeft = %lf, viewPortWidth = %lf \n"), newViewPortLeft, m_ChartViewer.getViewPortWidth());
		//TRACE(msg);
		m_ChartViewer.setViewPortLeft(_CurViewPortLeft);
		m_ChartViewer.updateViewPort(true, false);
	}
}


BOOL VtChartWindow::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN )
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CString strCount;
			_EditCount.GetWindowText(strCount);
			int count = _ttoi(strCount);
			AdjustChartToCount(count);
			return TRUE;
		}
		else if (pMsg->wParam == VK_DELETE)
		{
			//DeleteRefChartData();
// 			if (_ChartFrm)
// 			{
// 				//_ChartFrm->OnDeleteKey();
// 			}
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}


void VtChartWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//if (_ParentView)
	//	_ParentView->SetActiveChartWindow(this);
	//SetFocus();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void VtChartWindow::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int i = 0;
	CDialogEx::OnMButtonDown(nFlags, point);
}


void VtChartWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	//if (_ParentView)
	//	_ParentView->SetActiveChartWindow(this);
	// TODO: Add your message handler code here and/or call default
// 	CMenu mnuPopupSubmit;
// 	mnuPopupSubmit.LoadMenu(IDR_POPUP_PRODUCT);
// 
// 
// 	// Get a pointer to the first item of the menu
// 	CMenu *mnuPopupMenu = mnuPopupSubmit.GetSubMenu(0);
// 	ASSERT(mnuPopupMenu);
// 
// 	ClientToScreen(&point);
// 	// Find out if the user right-clicked the button
// 	mnuPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());

	CDialogEx::OnRButtonDown(nFlags, point);
}

void VtChartWindow::OnChartPattern(UINT id)
{
	m_ChartViewer.OnChartPattern(id);
}

void VtChartWindow::OnChartIndex(UINT id)
{
	m_ChartViewer.OnChartIndex(id);
}

void VtChartWindow::AddCompareData(VtChartData* data)
{
	if (!data || !_Data)
		return;

	// 메인 차트와 같으면 반환
	if (data->Key().compare(_Data->Key()) == 0)
		return;
	VtReferenceChart* refChart = new VtReferenceChart();
	refChart->show = true;
	refChart->name = data->Key();
	refChart->data = data;
	refChart->embedded = true;
	refChart->shareAxisWithMain = false;
	refChart->height = 150;
	refChart->type = 0;
	refChart->color = _ColorMgr->GetColor(refChart->data->ColorIndex());

	RefChartVector.push_back(refChart);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	data->AddChartWindow(this);
	chartDataMgr->AddChartDataRequest(10, data);

}



void VtChartWindow::AddCompareData(VtChartDataRequest req)
{
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->Add(req);
	AddCompareData(chartData);
}

void VtChartWindow::AddCompareData(int startAfter, VtChartData* data)
{
	if (!data || !_Data)
		return;

	// 메인 차트와 같으면 반환
	if (data->Key().compare(_Data->Key()) == 0)
		return;
	VtReferenceChart* refChart = new VtReferenceChart();
	refChart->show = true;
	refChart->name = data->Key();
	refChart->data = data;
	refChart->embedded = true;
	refChart->shareAxisWithMain = false;
	refChart->height = 150;
	refChart->type = 0;
	refChart->color = _ColorMgr->GetColor(refChart->data->ColorIndex());

	RefChartVector.push_back(refChart);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	data->AddChartWindow(this);
	chartDataMgr->AddChartDataRequest(startAfter, 10, data);
}

/*
void VtChartWindow::OnChartCompare(UINT id)
{
	m_ChartViewer.OnChartCompare(id);
}
*/


void VtChartWindow::OnCompacting(UINT nCpuTime)
{
	CDialogEx::OnCompacting(nCpuTime);

	// TODO: Add your message handler code here
}


void VtChartWindow::OnClose()
{
	CDialogEx::OnClose();
}


void VtChartWindow::OnBnClickedChartBtnFullscrn()
{
	// TODO: Add your control notification handler code here
	CSSplitter* parent = ParentSplit;
	_FullScreen = !_FullScreen;
	VtAttachDirection myDir = _AttachDirection;
	if (_FullScreen)
	{
		while (parent)
		{
			if (myDir == VtAttachDirection::AttachLeft)
				parent->HideRightPane();
			else if (myDir == VtAttachDirection::AttachRight)
				parent->HideLeftPane();
			else if (myDir == VtAttachDirection::AttachTop)
				parent->HideBottomPane();
			else if (myDir == VtAttachDirection::AttachBottom)
				parent->HideTopPane();
			myDir = parent->AttachDirection();
			parent = parent->ParentSplit;
		}
	}
	else
	{
		while (parent)
		{
			if (myDir == VtAttachDirection::AttachLeft)
				parent->ShowRightPane();
			else if (myDir == VtAttachDirection::AttachRight)
				parent->ShowLeftPane();
			else if (myDir == VtAttachDirection::AttachTop)
				parent->ShowBottomPane();
			else if (myDir == VtAttachDirection::AttachBottom)
				parent->ShowTopPane();
			myDir = parent->AttachDirection();
			parent = parent->ParentSplit;
		}
	}
}


void VtChartWindow::OnBnClickedChartBtnZoomin()
{
	// TODO: Add your control notification handler code here
	// 휠을 아래로 내리면
	// 축소(Reduce)가 아닌 확대(Expand) 이므로 false 대입
	double left = m_ChartViewer.getViewPortLeft();
	double width = m_ChartViewer.getViewPortWidth();
	double unit = 0.05;
	width -= unit;
	if (width < 0.05)
	{
		width = 0.05;
	}

	left = _CurViewPortRight - width;
	//TRACE(msg);

	m_ChartViewer.setViewPortWidth(width);
	m_ChartViewer.setViewPortLeft(left);
	m_ChartViewer.updateViewPort(true, false);


	CString strCount;
	//strCount.Format(_T("%d"), _ChartFrm->NoOfPoints());
	//_EditCount.SetWindowText(strCount);
	DrawChart(&m_ChartViewer, 0);
}


void VtChartWindow::OnBnClickedChartBtnZoomout()
{
	// TODO: Add your control notification handler code here
	double left = m_ChartViewer.getViewPortLeft();
	double width = m_ChartViewer.getViewPortWidth();
	double unit = 0.05;
	width += unit;
	if (width >= 1.0)
	{
		width = 1.0;
	}

	left = _CurViewPortRight - width;
	//TRACE(msg);

	m_ChartViewer.setViewPortWidth(width);
	m_ChartViewer.setViewPortLeft(left);
	m_ChartViewer.updateViewPort(true, false);


	CString strCount;
	//strCount.Format(_T("%d"), _ChartFrm->NoOfPoints());
	//_EditCount.SetWindowText(strCount);
	DrawChart(&m_ChartViewer, 0);
}


void VtChartWindow::OnBnClickedChartBtnDel()
{
	// TODO: Add your control notification handler code here
	//_ChartFrm->ResetChart();
	//_ChartFrm->DeleteRefChartData();
	Invalidate(TRUE);
}


void VtChartWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	AfxMessageBox(_T("ChartWindowKeyDown!"));
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void VtChartWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int i = 0;
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void VtChartWindow::DrawChart(CChartViewer* a_pChartViewer, int mode)
{
	if (!a_pChartViewer || !_Data || !_Data->Filled() || _Data->GetDataCount() == 0)
		return;

	if (_Drawing)
		return;
	_Drawing = true;

	GetZoomRange(a_pChartViewer, startIndex, endIndex);
	int noOfPoints = endIndex - startIndex + 1;
	_NoOfPoints = noOfPoints;

	CString msg;
	msg.Format(_T("width = %d, height = %d, point count = %d\n"), _Width, _Height, noOfPoints);
	//TRACE(msg);

	DoubleArray timeStamps;
	/*
	if (RefChartVector.size() > 0)
	{
		VtReferenceChart* ref = RefChartVector.front();
		timeStamps = DoubleArray(ref->data->DateTime.data() + startIndex, noOfPoints);
	}
	else
	{
		timeStamps = DoubleArray(_Data->DateTime.data() + startIndex, noOfPoints);
	}
	*/
	std::vector<double>& datetime_vec = _Data->GetDataArray(_T("datetime"));
	std::vector<double>& open_vec = _Data->GetDataArray(_T("open"));
	std::vector<double>& high_vec = _Data->GetDataArray(_T("high"));
	std::vector<double>& low_vec = _Data->GetDataArray(_T("low"));
	std::vector<double>& close_vec = _Data->GetDataArray(_T("close"));
	std::vector<double>& volume_vec = _Data->GetDataArray(_T("volume"));

	timeStamps = DoubleArray(datetime_vec.data() + startIndex, noOfPoints);
	DoubleArray highData = DoubleArray(high_vec.data() + startIndex, noOfPoints);
	DoubleArray lowData = DoubleArray(low_vec.data() + startIndex, noOfPoints);
	DoubleArray openData = DoubleArray(open_vec.data() + startIndex, noOfPoints);
	DoubleArray closeData = DoubleArray(close_vec.data() + startIndex, noOfPoints);
	DoubleArray volData = DoubleArray(volume_vec.data() + startIndex, noOfPoints);

	ArrayMath minArray(lowData);
	ArrayMath maxArray(highData);
	int minIndex = minArray.minIndex();
	int maxIndex = maxArray.maxIndex();


	_SourceChart = new FinanceChart(_Width);

	
	//_SourceChart->setMargins(0, 35, 0, 35);
	// Add a title to the chart
	TCHARtoUTF8 title(_ChartTile.c_str());
	_SourceChart->addTitle(title);

	// Disable default legend box, as we are using dynamic legend
	_SourceChart->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);

	// Set the data into the finance chart object
	_SourceChart->setData(timeStamps, highData, lowData, openData, closeData, volData, _XtraPoints);

	_Height = CalcChartHeight();

	
	// Add the main chart with 240 pixels in height
	_MainChart = _SourceChart->addMainChart(_Height);

	
	_MainChart->setPlotArea(30, 35, _Width - 140, _Height, 0xffffff, 0xffffff, 0xffffff, _MainChart->dashLineColor(0xdddddd, Chart::DotLine), -1);
	_MainChart->yAxis()->setOffset(_YAxisGap, 0);

	if (_SelectedDataSetName.length() == 0 || _SelectedDataSetName.compare(_T("MainChart")) == 0)
		_MainChart->yAxis()->setColors(0x00dddddd, 0x00000000, 0x00000000);
	else
		_MainChart->yAxis()->setColors(0xffdddddd, 0xff000000, 0xff000000);
	//_MainChart->setNumberFormat(',');
	std::string yAxisFormat;
	yAxisFormat = _T("{value|");
	yAxisFormat.append(std::to_string(_Data->Decimal()));
	yAxisFormat.append(_T(",.}"));
	_MainChart->yAxis()->setLabelFormat(yAxisFormat.c_str());
	//_MainChart->yAxis()->setLabelFormat(_T("{value|2,.}"));
	

	//if (_ChartOrderManager)
	//	_ChartOrderManager->Chart(_MainChart);

	_LayerList.clear();

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(_Data->SymbolCode());
	VtLayerInfo layerInfo;
	std::string text = _T("MainChart;");
	text.append(sym ? sym->Name : _Data->SymbolCode());
	TCHARtoUTF8 convert(text.c_str());
	if (_ShowMainChart)
	{
		if (_Data->MainChartType() == VtMainChartType::ClosePrice)
		{
			_MainLayer = _SourceChart->addCloseLine(_ColorMgr->GetColor(_Data->ColorIndex()), convert);
			_MainLayer->setLineWidth(3);
			layerInfo.layer = _MainLayer;
		}
		else if (_Data->MainChartType() == VtMainChartType::TypicalPrice)
		{
			_MainLayer = _SourceChart->addTypicalPrice(_ColorMgr->GetColor(_Data->ColorIndex()), convert);
			_MainLayer->setLineWidth(3);
			layerInfo.layer = _MainLayer;
		}
		else if (_Data->MainChartType() == VtMainChartType::WeightedClose)
		{
			_MainLayer = _SourceChart->addWeightedClose(_ColorMgr->GetColor(_Data->ColorIndex()), convert);
			_MainLayer->setLineWidth(3);
			layerInfo.layer = _MainLayer;
		}
		else if (_Data->MainChartType() == VtMainChartType::MedianPrice)
		{
			_MainLayer = _SourceChart->addMedianPrice(_ColorMgr->GetColor(_Data->ColorIndex()), convert);
			_MainLayer->setLineWidth(3);
			layerInfo.layer = _MainLayer;
		}
		else if (_Data->MainChartType() == VtMainChartType::OHLC)
		{
			layerInfo.layer = _SourceChart->addHLOC(0x8000, 0x800000, convert);
		}
		else
		{
			// Add candlestick symbols to the main chart, using green/red for up/down days
			CandleStickLayer* candle = _SourceChart->addCandleStick(0x00ff00, 0xff0000, convert);
			candle->setColors(0xff0000, 0xff0000, 0x0000ff, 0x0000ff);
			layerInfo.layer = candle;
		}

		layerInfo.selected = false;
		layerInfo.chart = _MainChart;
		layerInfo.axis = _MainChart->yAxis();
		layerInfo.data = _Data;
		layerInfo.color = _ColorMgr->GetColor(_Data->ColorIndex());
		layerInfo.colorIndex = _Data->ColorIndex();
		_LayerList.push_back(layerInfo);
	}

	DrawRefChart(_LayerList);

	DrawArea* d = _MainChart->makeChart();


	//if (_shapesCollection.CheckChangeDimension(_ChartViewer, _Data->GetDataCount(), _MainChart))
	//	_shapesCollection.ValueToPixel(0, startIndex, _MainChart);

	//if (mode == 0)
	//	DrawObjects(0, 0, d, _MainChart, _offset.x, _offset.y);


	DrawTitleValue(d);

	//DrawStress(d);

	DrawSignal(timeStamps, highData, lowData, d);

	SetDataCount();

	TrackFinance(_SourceChart, (int)_MouseClickPoint.x);
	
	FinanceChart* chart = (FinanceChart*)a_pChartViewer->getChart();
	if (chart)
	{
		delete chart;
	}
	a_pChartViewer->setChart(_SourceChart);

	_Drawing = false;
}

void VtChartWindow::SetDefaultChartData()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 총호가 수량과 건수
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (!sym)
		return;
	VtRealtimeRegisterManager* realMgr = VtRealtimeRegisterManager::GetInstance();
	VtChartDataRequest req;
	req.chartType = VtChartType::MIN;
	req.productCode = sym->ShortCode;
	req.mainChartType = VtMainChartType::ClosePrice;
	req.count = 9999;
	req.cycle = 1;
	req.next = 0;
	req.reqKey = 0;
	req.seq = 0;
	//req.domestic = false;
	req.domestic = true;
	req.keyvalue = _T("");
	req.initial = _T("KP");
	req.decimal = sym->Decimal;
	req.colorIndex = _ColorMgr->GetNextColorIndex();
	_ChartDataReqVector.push_back(req);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->Add(req);
	SetChartData(chartData);
	_ChartDataVec.push_back(chartData);

	realMgr->RegisterProduct(req.productCode);
}

void VtChartWindow::SetDefaultRefChartData()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtRealtimeRegisterManager* realMgr = VtRealtimeRegisterManager::GetInstance();

	VtChartData* chartData = nullptr;
	VtChartDataRequest req;

	std::vector<std::string> symList;
	symList.push_back(_T("ESM19"));
	symList.push_back(_T("NQM19"));
	symList.push_back(_T("CNH19"));
	symList.push_back(_T("NKM19"));
	symList.push_back(_T("HSIH19"));

	for (int i = 0; i < symList.size(); ++i)
	{
		std::string temp;
		temp = VtHdCtrl::PadRight(symList[i], ' ', 32);
		realMgr->RegisterProduct(temp);
	}

	std::vector<std::string> nameList;
	nameList.push_back(_T("미니 S&P 500"));
	nameList.push_back(_T("미니 NASDAQ 100"));
	nameList.push_back(_T("SGX FTSE 차이나 A50"));
	nameList.push_back(_T("SGX Nikkei 225"));
	nameList.push_back(_T("Hang Seng Index"));

	std::vector<std::string> initialList;
	initialList.push_back(_T("SP"));
	initialList.push_back(_T("NQ"));
	initialList.push_back(_T("A50"));
	initialList.push_back(_T("NK"));
	initialList.push_back(_T("HSI"));

	int interval = 2;
	VtSymbol* sym = nullptr;
	for (int i = 0; i < symList.size(); ++i)
	{
		sym = symMgr->FindSymbol(symList[i]);
		if (!sym)
		{
			sym = new VtSymbol();
			sym->ShortCode = symList[i];
			sym->Name = nameList[i];
			sym->Decimal = 2;
			symMgr->AddHdSymbol(sym);
		}

		req.chartType = VtChartType::MIN;
		req.productCode = sym->ShortCode;
		req.mainChartType = VtMainChartType::ClosePrice;
		req.count = 9999;
		req.cycle = 1;
		req.next = 0;
		req.reqKey = 0;
		req.seq = 0;
		req.domestic = false;
		//req.domestic = true;
		req.keyvalue = _T("");
		req.initial = initialList[i];
		req.decimal = sym->Decimal;
		req.colorIndex = _ColorMgr->GetNextColorIndex();
		_ChartDataReqVector.push_back(req);
		chartData = chartDataMgr->Add(req);
		AddCompareData(interval * (i + 1), chartData);
		_ChartDataVec.push_back(chartData);
	}
}

std::vector<VtChartData*> VtChartWindow::GetChartDataList()
{
	std::vector<VtChartData*> dataList;
	if (_Data)
		dataList.push_back(_Data);
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* ref = *it;
		dataList.push_back(ref->data);
	}

	return dataList;
}

void VtChartWindow::ShowChart(std::string symCode, bool show)
{
	if (_Data && _Data->SymbolCode().compare(symCode) == 0)
	{
		_ShowMainChart = show;
		_SelectedDataSetName = _T("");
		DrawChart(&m_ChartViewer, 0);
		return;
	}
	VtReferenceChart* refChart = FindRefChart(symCode);
	if (refChart)
	{
		_SelectedDataSetName = _T("");
		refChart->show = show;
		DrawChart(&m_ChartViewer, 0);
	}
}

void VtChartWindow::StopChartDataEvent()
{
	// TODO: Add your message handler code here and/or call default
	if (_Data)
	{
		// Return the used color
		_ColorMgr->ReturnColorIndex(_Data->ColorIndex());
		// Remove the chart window used from the map.
		_Data->RemoveChartWindow(this);
	}


	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		// Return the used color
		_ColorMgr->ReturnColorIndex(chart->data->ColorIndex());
		// Remove the chart window used from the map.
		chart->data->RemoveChartWindow(this);
		delete *it;
	}

	RefChartVector.clear();
}

void VtChartWindow::SetSelectedChartData(std::string selectedCode)
{
	if (_Data && _Data->SymbolCode().compare(selectedCode) == 0)
	{
		_SelectedDataSetName = _T("MainChart");
		_SelectedData = _Data;
	}
	else
	{
		std::pair<int, VtReferenceChart*> index = RefChartIndex(selectedCode);
		if (std::get<0>(index) > -1)
		{
			_SelectedDataSetName = _T("Reference>");
			_SelectedDataSetName.append(std::to_string(std::get<0>(index)));
			_SelectedData = std::get<1>(index)->data;
		}
		else
		{
			_SelectedDataSetName = _T("");
			_SelectedData = _Data;
		}
	}
	DrawChart(&m_ChartViewer, 0);
}

void VtChartWindow::ChangeChartTime(int newTime)
{
	// 먼저 기존 서비스를 중지시킨다.
	StopChartDataEvent();
	// 새로운 서비스를 시작한다.
	int i = 0, interval = 2;
	for (auto it = _ChartDataReqVector.begin(); it != _ChartDataReqVector.end(); ++it)
	{
		VtChartDataRequest& req = *it;
		req.cycle = newTime;

		VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
		VtChartData* chartData = chartDataMgr->Add(req);
		chartData->FilledCount(0);
		if (req.domestic)
			SetChartData(chartData);
		else
			AddCompareData(interval * i++, chartData);
	}
}

void VtChartWindow::ChangeSystem(VtSystem* newSystem)
{
	if (!newSystem)
		return;
	if (_System)
		_System->ResetSignal();
	if (_Data)
		newSystem->SetDataMap(_Data);
}

VtReferenceChart* VtChartWindow::FindRefChart(std::string symCode)
{
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->data && (chart->data->SymbolCode().compare(symCode) == 0))
			return chart;
	}

	return nullptr;
}

void VtChartWindow::GetZoomRange(CChartViewer * a_pChartViewer, int& start, int& end)
{
	start = 0;
	end = 0;
	if (_Data == nullptr || a_pChartViewer == nullptr)
		return;

	start = (int)(a_pChartViewer->getViewPortLeft() * GetMaxDataCount());
	end = (int)((a_pChartViewer->getViewPortLeft() + a_pChartViewer->getViewPortWidth()) * GetMaxDataCount());

	DoubleArray timeStamps = DoubleArray(_Data->DateTime.data(), GetMaxDataCount());

	double viewPortStartDate = timeStamps[start];//a_pChartViewer->getValueAtViewPort("x", a_pChartViewer->getViewPortLeft());
	double viewPortEndDate = a_pChartViewer->getValueAtViewPort("x", a_pChartViewer->getViewPortLeft() +
		a_pChartViewer->getViewPortWidth());

	// Get the array indexes that corresponds to the visible start and end dates
	int startIndex = (int)floor(Chart::bSearch(timeStamps, viewPortStartDate));
	int endIndex = (int)ceil(Chart::bSearch(timeStamps, viewPortEndDate));

	if (start >= GetMaxDataCount())
		start = GetMaxDataCount() - 1;
	if (end >= GetMaxDataCount())
		end = GetMaxDataCount() - 1;
}

int VtChartWindow::CalcChartHeight()
{
	CRect rect;

	GetClientRect(rect);

	int height = rect.Height();

	int totalIndexHeight = 0;// CalcTotalIndexHeight();
	height -= totalIndexHeight;
	int totalRefChartHeight = 0; // CalcTotalRefChartHeight();
	height -= totalRefChartHeight;

	if (Active())
		height = height - 72 - CtrlHeight();
	else
		height = height - 69 - CtrlHeight();

	if ((totalIndexHeight + totalRefChartHeight) == 0)
	{
		_ShowAll = false;
	}
	else
	{
		_ShowAll = true;
	}

	return height;
}

int VtChartWindow::GetMaxDataCount()
{
	if (!_Data)
		return 0;
	int maxCount = _Data->GetDataCount();

	CString cnt;
	cnt.Format(_T("chartdataCount = %d"), maxCount);
	//TRACE(cnt);

	
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->data->GetDataCount() > maxCount)
			maxCount = chart->data->GetDataCount();

		//cnt.Format(_T("chartdataCount = %d"), data->GetDataCount());
		//TRACE(cnt);
	}
	
	return maxCount;
}

void VtChartWindow::initChartViewer(CChartViewer *viewer)
{
	if (!_Data || _Init) return;

	auto time = _Data->DateTime;
	int dataSize = _Data->GetDataCount();
	// Set the full x range to be the duration of the data
	viewer->setFullRange("x", time[0], time[dataSize - 1]);

	// Initialize the view port to show the latest 20% of the time range
	viewer->setViewPortWidth(0.06);
	viewer->setViewPortLeft(1 - viewer->getViewPortWidth());
	viewer->setZoomInWidthLimit(0.05);

	// Set the maximum zoom to 10 points
	viewer->setZoomInWidthLimit((_MinPoints + _XtraPoints) / dataSize);

	// Initially set the mouse to drag to scroll mode.
	viewer->setMouseUsage(Chart::MouseUsageDefault);

	_Height = CalcChartHeight();

	_Init = true;
}

void VtChartWindow::DrawTitleValue(DrawArea* drawArea)
{
	PlotArea* plotArea = _MainChart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + _MainChart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + _MainChart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();
	VtChartData* selData = _Data;
	if (_SelectedData) {
		selData = _SelectedData;
	}

	std::string colorText = _ColorMgr->RGBToString(selData->ColorIndex());
	std::string strValue = _T("<*font = times.ttf, size = 18, color = ");// FF0000* > ");
	strValue.append(colorText);
	strValue.append(_T("*>"));
	std::string text = strValue;
	if (selData->Currency().compare(_T("KRW")) == 0)
		strValue = string_cast<double>(selData->RealTimeClose(), selData->Decimal(), convert::thou_sep);
	else
		strValue = string_cast<double>(selData->RealTimeClose(), selData->Decimal(), convert::thou_sep);

	text.append(strValue);
	text.append(_T("<*font = gulim.ttc, size = 10, color = ff0000*>"));
	text.append(selData->Currency());
	text.append(_T("<*font = gulim.ttc, size = 10, color = 000000*>"));
	text.append(TCHARtoUTF8(_T(" 거래소:")));
	text.append(selData->Exchange());
	text.append(TCHARtoUTF8(_T(",종목이름:")));
	
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(selData->SymbolCode());
	if (sym)
		text.append(TCHARtoUTF8(sym->Name.c_str()));
	int cycle = selData->Cycle();
	CString strCycle;
	strCycle.Format(_T("(%d"), cycle);
	std::string temp;
	temp = strCycle;
	text.append(temp);
	switch (selData->ChartType()) {
	case VtChartType::DAY:
		text.append(TCHARtoUTF8(_T("일")));
		break;
	case VtChartType::MONTH:
		text.append(TCHARtoUTF8(_T("월")));
		break;
	case VtChartType::WEEK:
		text.append(TCHARtoUTF8(_T("주")));
		break;
	case VtChartType::HOUR:
		text.append(TCHARtoUTF8(_T("시")));
		break;
	case VtChartType::MIN:
		text.append(TCHARtoUTF8(_T("분")));
		break;
	case VtChartType::TICK:
		text.append(TCHARtoUTF8(_T("틱")));
		break;
	default:
		break;
	}
	text.append(_T(")"));
	TTFText* t = drawArea->text(text.c_str(), _T("굴림"), 14);
	t->draw(plotAreaLeftX, 10, 0xff0000);
	t->destroy();

	
	sym = symMgr->FindSymbol(selData->SymbolCode());
	if (sym) {
		
		std::string title;
		// 한글을 위해서는 반드시 gulim.ttc 폰트를 설정해야 한다.
		strValue = _T("<* font = H2HDRM.TTF, size = 14, color = ");
		strValue.append(colorText);
		strValue.append(_T(" *>"));
		title.append(strValue);
		title.append(sym->Name.c_str());
		title.append(_T(" : "));
		title.append(sym->ShortCode);
		title.append(_T(" 거래소:("));
		title.append(selData->Exchange());
		title.append(_T(")"));
		_ChartTile = title;
	}
}

void VtChartWindow::TrackFinance(MultiChart *m, int mouseX)
{
	CString pos;
	pos.Format(_T("tracFinance :: x = %d, y = %d \n"), mouseX, m_ChartViewer.getPlotAreaMouseY());
	//TRACE(pos);

	if (!_Data)
		return;


	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = m->initDynamicLayer();

// 	if (_SourceChart != nullptr && _isMouseLButtonDown)
// 	{
// 		//DrawArea *d = _SourceChart->initDynamicLayer();
// 		if (_editStatus == EditStatus::DrawRect)
// 			DrawCreatingObject(d);
// 		else if (_editStatus == EditStatus::SelectRect)
// 			DrawSelection(d);
// 		else if (_editStatus == EditStatus::Redim)
// 		{
// 			if (_RedimStarted == true)
// 			{
// 				//DrawChart(_ChartViewer, 1);
// 				_RedimStarted = false;
// 			}
// 			//DrawChart(_ChartViewer, 0);
// 		}
// 	}

	// It is possible for a FinanceChart to be empty, so we need to check for it.
	if (m->getChartCount() == 0)
		return;

	// Get the data x-value that is nearest to the mouse
	int xValue = (int)(((XYChart *)m->getChart(0))->getNearestXValue(mouseX));

	// Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
	XYChart *c = 0;
	PlotArea *plotArea = nullptr;
	int plotAreaLeftX = 0;
	int plotAreaTopY = 0;
	Axis* axis = nullptr;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();

	for (int i = 0; i < m->getChartCount(); ++i) {
		c = (XYChart *)m->getChart(i);
		plotArea = c->getPlotArea();
		plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
		plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();

		c->setDefaultFonts("gulim.ttc", "gulim.ttc");
		// Variables to hold the legend entries
		ostringstream ohlcLegend;
		vector<string> legendEntries;
		std::string name;
		// Iterate through all layers to find the highest data point
		for (int j = 0; j < c->getLayerCount(); ++j)
		{
			Layer *layer = c->getLayerByZ(j);
			int xIndex = layer->getXIndexOf(xValue);
			int dataSetCount = layer->getDataSetCount();

			// In a FinanceChart, only layers showing OHLC data can have 4 data sets
			if (dataSetCount == 4)
			{
				double highValue = layer->getDataSet(0)->getValue(xIndex);
				double lowValue = layer->getDataSet(1)->getValue(xIndex);
				double openValue = layer->getDataSet(2)->getValue(xIndex);
				double closeValue = layer->getDataSet(3)->getValue(xIndex);

				name = layer->getDataSet(0)->getDataName();
				int pos = name.find(_T(";"));
				name = name.substr(0, pos);
				ohlcLegend << "<*block*>";
				if (name.find(_T("Reference")) != std::string::npos && RefChartVector.size() > 0)
				{
					pos = name.find(_T(">"));
					if (pos != -1)
					{
						std::string index = name.substr(pos + 1, name.length() - pos);
						int idx = std::stoi(index);
						if (idx >= 0 || idx < RefChartVector.size())
						{
							VtReferenceChart* refChart = RefChartVector[idx];
							VtSymbol* sym = symMgr->FindSymbol(refChart->data->SymbolCode());

							int cycle = refChart->data->Cycle();
							CString strCycle;
							strCycle.Format(_T("(%d"), cycle);
							std::string temp;
							temp = strCycle;
							std::string text = temp;
							switch (refChart->data->ChartType())
							{
							case VtChartType::DAY:
								text.append(_T("일"));
								break;
							case VtChartType::MONTH:
								text.append(_T("월"));
								break;
							case VtChartType::WEEK:
								text.append(_T("주"));
								break;
							case VtChartType::HOUR:
								text.append(_T("시"));
								break;
							case VtChartType::MIN:
								text.append(_T("분"));
								break;
							case VtChartType::TICK:
								text.append(_T("틱"));
								break;
							default:
								break;
							}
							text.append(_T(")"));

							std::string label;
							label = _T("[거래소:");
							label.append(refChart->data->Exchange());
							label.append(_T(",종목이름:"));
							label.append(sym->Name);
							label.append(text);
							label.append(_T("]"));
							ohlcLegend << TCHARtoUTF8(label.c_str());
						}
					}
				}
				if (name.length() > 0 && closeValue != Chart::NoValue) {

					// Build the OHLC legend
					ohlcLegend << TCHARtoUTF8(_T(" 시가:"));
					ohlcLegend << c->formatValue(openValue, "{value|P4}");
					ohlcLegend << TCHARtoUTF8(_T(", 고가:"));
					ohlcLegend << c->formatValue(highValue, "{value|P4}");
					ohlcLegend << TCHARtoUTF8(_T(", 저가:"));
					ohlcLegend << c->formatValue(lowValue, "{value|P4}");
					ohlcLegend << TCHARtoUTF8(_T(", 종가:"));
					ohlcLegend << c->formatValue(closeValue, "{value|P4}");

					// We also draw an upward or downward triangle for up and down days and the %
					// change
					double lastCloseValue = layer->getDataSet(3)->getValue(xIndex - 1);
					if (lastCloseValue != Chart::NoValue) {
						double change = closeValue - lastCloseValue;
						double percent = change * 100 / closeValue;
						string symbol = (change >= 0) ?
							"<*font,color=008800*><*img=@triangle,width=8,color=008800*>" :
							"<*font,color=CC0000*><*img=@invertedtriangle,width=8,color=CC0000*>";

						ohlcLegend << "  " << symbol << " " << c->formatValue(change, "{value|P4}");
						ohlcLegend << " (" << c->formatValue(percent, "{value|2}") << "%)<*/font*>";
					}

					ohlcLegend << "<*/*>";
				}

				if (_SelectedDataSetName.length() > 0 && _SelectedDataSetName.compare(name) == 0)
				{
					for (int k = 0; k < _NoOfPoints; k = k + 20)
					{
						double curVal = 0.0;
						for (int s = 0; s < 4; ++s)
						{
							curVal = layer->getDataSet(s)->getValue(k);
							int yVal = c->getYCoor(curVal);
							int xVal = c->getXCoor(k);
							d->circle(plotAreaLeftX + xVal - 30, plotAreaTopY + yVal - 35, 4, 4, 0xffffff, 0x000000);
						}
					}
				}
			}
			else
			{
				// Iterate through all the data sets in the layer
				for (int k = 0; k < layer->getDataSetCount(); ++k)
				{
					DataSet *dataSet = layer->getDataSetByZ(k);
					int pos = 0;
					double value = 0.0;
					name = dataSet->getDataName();
					if (name.find(_T("Reference")) != std::string::npos && RefChartVector.size() > 0)
					{
						pos = name.find(_T(";"));
						name = name.substr(0, pos);
						pos = name.find(_T(">"));
						if (pos != -1)
						{
							std::string index = name.substr(pos + 1, name.length() - pos);
							int idx = std::stoi(index);
							if (idx >= 0 || idx < RefChartVector.size())
							{
								VtReferenceChart* refChart = RefChartVector[idx];
								VtSymbol* sym = symMgr->FindSymbol(refChart->data->SymbolCode());

								int cycle = refChart->data->Cycle();
								CString strCycle;
								strCycle.Format(_T("(%d"), cycle);
								std::string temp;
								temp = strCycle;
								std::string text = temp;
								switch (refChart->data->ChartType())
								{
								case VtChartType::DAY:
									text.append(_T("일"));
									break;
								case VtChartType::MONTH:
									text.append(_T("월"));
									break;
								case VtChartType::WEEK:
									text.append(_T("주"));
									break;
								case VtChartType::HOUR:
									text.append(_T("시"));
									break;
								case VtChartType::MIN:
									text.append(_T("분"));
									break;
								case VtChartType::TICK:
									text.append(_T("틱"));
									break;
								default:
									break;
								}
								text.append(_T(")"));

								std::string label;
								label = _T("[거래소:");
								label.append(refChart->data->Exchange());
								label.append(_T(",종목이름:"));
								label.append(sym->Name);
								label.append(text);
								label.append(_T("]"));

								value = layer->getDataSet(0)->getValue(xIndex);
								name = label;
								name = name + ": " + c->formatValue(value, "{value|P3}");

								ostringstream legendEntry;
								legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color="
									<< hex << dataSet->getDataColor() << "*> " << TCHARtoUTF8(name.c_str()) << "<*/*>";
								legendEntries.push_back(legendEntry.str());
							}
						}
					}
					else
					{
						pos = name.find(_T(";"));
						name = name.substr(pos + 1, name.length() - 1);
						value = dataSet->getValue(xIndex);
						if ((0 != name.size()) && (value != Chart::NoValue))
						{

							// In a FinanceChart, the data set name consists of the indicator name and its
							// latest value. It is like "Vol: 123M" or "RSI (14): 55.34". As we are
							// generating the values dynamically, we need to extract the indictor name
							// out, and also the volume unit (if any).

							// The volume unit
							string unitChar;

							// The indicator name is the part of the name up to the colon character.
							int delimiterPosition = (int)name.find(':');
							if (name.npos != delimiterPosition) {

								// The unit, if any, is the trailing non-digit character(s).
								int lastDigitPos = (int)name.find_last_of("0123456789");
								if ((name.npos != lastDigitPos) && (lastDigitPos + 1 < (int)name.size()) &&
									(lastDigitPos > delimiterPosition))
									unitChar = name.substr(lastDigitPos + 1);

								name.resize(delimiterPosition);
							}

							// In a FinanceChart, if there are two data sets, it must be representing a
							// range.
							if (dataSetCount == 2) {
								// We show both values in the range in a single legend entry
								value = layer->getDataSet(0)->getValue(xIndex);
								double value2 = layer->getDataSet(1)->getValue(xIndex);
								name = name + ": " + c->formatValue(min(value, value2), "{value|P3}");
								name = name + " - " + c->formatValue(max(value, value2), "{value|P3}");
							}
							else
							{
								// In a FinanceChart, only the layer for volume bars has 3 data sets for
								// up/down/flat days
								if (dataSetCount == 3)
								{
									// The actual volume is the sum of the 3 data sets.
									value = layer->getDataSet(0)->getValue(xIndex) + layer->getDataSet(1
									)->getValue(xIndex) + layer->getDataSet(2)->getValue(xIndex);
								}

								// Create the legend entry
								name = name + ": " + c->formatValue(value, "{value|P3}") + unitChar;
							}

							// Build the legend entry, consist of a colored square box and the name (with
							// the data value in it).
							ostringstream legendEntry;
							legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color="
								<< hex << dataSet->getDataColor() << "*> " << name << "<*/*>";
							legendEntries.push_back(legendEntry.str());
						}
					}
				}

				int setCount = layer->getDataSetCount();
				if (setCount > 0)
				{
					std::string alias;
					std::string dataName = layer->getDataSet(0)->getDataName();
					int pos = (int)dataName.find(';');
					if (pos != std::string::npos)
					{
						alias = dataName.substr(0, pos);
					}
					if (alias.length() > 0 &&
						_SelectedDataSetName.length() > 0 &&
						_SelectedDataSetName.find(alias) != std::string::npos)
					{
						for (int k = 0; k < _NoOfPoints; k = k + 10)
						{
							double curVal = 0.0;
							for (int s = 0; s < setCount; ++s)
							{
								curVal = layer->getDataSet(s)->getValue(k);
								int yVal = c->getYCoor(curVal);
								int xVal = c->getXCoor(k);
								if (dataName.find(_T("Reference")) != std::string::npos)
								{
									Axis* axis = FindAxis(dataName);
									yVal = c->getYCoor(curVal, axis);
								}
								else if (dataName.find(_T("Vol")) != std::string::npos)
								{
									Axis* axis = _MainChart->yAxis2();
									yVal = c->getYCoor(curVal, axis);
								}
								d->circle(plotAreaLeftX + xVal - 30, plotAreaTopY + yVal - 35, 4, 4, 0xffffff, 0x000000);
							}
						}
					}

				}
			}
		}

		int height = plotArea->getHeight();
		int width = plotArea->getWidth();
		ostringstream legendText;
		legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
			<< "*><*font=arialbd.ttf*>[" << c->xAxis()->getFormattedLabel(xValue, "mm/dd hh:nn")
			<< "]<*/font*>" << ohlcLegend.str();
		for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i) {
			legendText << "      " << legendEntries[i];
		}
		legendText << "<*/*>";

		DoubleArray datetime = DoubleArray(_Data->DateTime.data() + startIndex, _NoOfPoints);
		double first = datetime[0];
		int oldx = 0, oldy = 0;
		oldx = _MainChart->getXCoor(0);
		int color = 0xcc000000;
		double oldDate = Chart::getChartYMD(datetime[0]);
		for (int i = 1; i < datetime.len; i++) {
			double newDate = Chart::getChartYMD(datetime[i]);
			int xpos = _MainChart->getXCoor(i);
			if (oldDate != newDate) {
				d->line(xpos, plotAreaTopY, xpos, plotAreaTopY + height, color, 1);
			}

			oldDate = newDate;
		}

		d->line(plotAreaLeftX + width + _YAxisGap, plotAreaTopY, plotAreaLeftX + width + _YAxisGap, plotAreaTopY + height, 0xc6cbcc, 1);

		d->line(plotAreaLeftX, plotAreaTopY, plotAreaLeftX + width, plotAreaTopY, 0xc6cbcc, 1);
		d->line(plotAreaLeftX, plotAreaTopY, plotAreaLeftX, plotAreaTopY + height, 0xc6cbcc, 1);
		d->line(plotAreaLeftX, plotAreaTopY + height, plotAreaLeftX + width, plotAreaTopY + height, 0xc6cbcc, 1);

		d->rect(plotAreaLeftX, plotAreaTopY, plotAreaLeftX + 10, plotAreaTopY + 10, 0xffffff, 0x000000);
		d->line(plotAreaLeftX, plotAreaTopY, plotAreaLeftX + 10, plotAreaTopY + 10, 0xffffff, 1);
		d->line(plotAreaLeftX + 10, plotAreaTopY, plotAreaLeftX, plotAreaTopY + 10, 0xffffff, 1);

		d->rect(plotAreaLeftX + 11, plotAreaTopY + 1, plotAreaLeftX + 19, plotAreaTopY + 9, 0x000000, 0xffffff);
		d->line(plotAreaLeftX + 11, plotAreaTopY + 5, plotAreaLeftX + 19, plotAreaTopY + 5, 0x000000, 1);

		// Display the legend on the top of the plot area
		TTFText *t = d->text(legendText.str().c_str(), "gulim.ttc", 8);
		t->draw(plotAreaLeftX + 23, plotAreaTopY, 0x000000, Chart::TopLeft);
		t->destroy();

		int entireWidth = m->getWidth();
		int entireHeight = m->getHeight();
		d->line(0, 0, entireWidth, 0, 0x000000, 1);
		d->line(0, entireHeight - 1, entireWidth, entireHeight - 1, 0x000000, 1);
		d->line(0, 0, 0, entireHeight, 0x000000, 1);
		d->line(entireWidth - 1, 0, entireWidth - 1, entireHeight, 0x000000, 1);
		DrawRealtimeValues(d);
	}
}

Axis* VtChartWindow::FindAxis(std::string dataName)
{
	for (auto it = _LayerList.begin(); it != _LayerList.end(); ++it) {
		VtLayerInfo layerInfo = *it;
		Layer* layer = layerInfo.layer;
		int datasetCount = layer->getDataSetCount();
		if (datasetCount > 0) {
			for (int i = 0; i < datasetCount; ++i) {
				std::string name = layer->getDataSet(i)->getDataName();
				if (name.find(dataName) != std::string::npos) {
					return layerInfo.axis;
				}
			}
		}
	}

	return nullptr;
}

Axis* VtChartWindow::FindAxis(std::string dataName, std::vector<VtLayerInfo>& layerList)
{
	for (auto it = layerList.begin(); it != layerList.end(); ++it) {
		VtLayerInfo layerInfo = *it;
		Layer* layer = layerInfo.layer;
		int datasetCount = layer->getDataSetCount();
		if (datasetCount > 0) {
			for (int i = 0; i < datasetCount; ++i) {
				std::string name = layer->getDataSet(i)->getDataName();
				if (name.find(dataName) != std::string::npos) {
					return layerInfo.axis;
				}
			}
		}
	}

	return nullptr;
}

void VtChartWindow::HitTest(CPoint point)
{
	if (!_Init || !_SourceChart)
		return;

	_SelectedDataSetName = _T("");
	if (!_SourceChart)
		return;

	if (_SourceChart && _SourceChart->getChartCount() == 0)
		return;
	if (HitTestSystem(point))
		return;

	XYChart *c = 0;
	PlotArea *plotArea = nullptr;
	int plotAreaLeftX = 0;
	int plotAreaTopY = 0;
	int plotHeight = 0;
	bool found = false;
	CRect rcTest;
	std::string dataName;
	for (int i = 0; i < _SourceChart->getChartCount(); ++i) {
		c = (XYChart *)_SourceChart->getChart(i);
		plotArea = c->getPlotArea();
		plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
		plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
		plotHeight = plotArea->getHeight();
		for (int j = 0; j < c->getLayerCount(); ++j) {
			Layer *layer = c->getLayer(j);
			int xValue = (int)(c->getNearestXValue((double)point.x));
			int xIndex = layer->getXIndexOf(xValue);
			int yCoor = 0;
			int yCoor2 = 0;
			int xCoor = c->getXCoor(xValue);

			double yValue = 0;
			int dataSetCount = layer->getDataSetCount();
			if (dataSetCount == 4) {
				dataName = layer->getDataSet(0)->getDataName();
				double yValue2 = layer->getDataSet(1)->getValue(xIndex);
				if (dataName.find(_T("Reference")) != std::string::npos) {
					Axis* axis = FindAxis(dataName);
					yValue = layer->getDataSet(0)->getValue(xIndex);
					yCoor = c->getYCoor(yValue, axis);
					//yCoor += plotAreaTopY;
					yCoor2 = c->getYCoor(yValue2);
					//yCoor2 += plotAreaTopY;

					xCoor = plotAreaLeftX + xCoor - 30;
					yCoor = plotAreaTopY + yCoor - 35;
					yCoor2 = plotAreaTopY + yCoor2 - 35;
				} else {
					yValue = layer->getDataSet(0)->getValue(xIndex);
					yCoor = c->getYCoor(yValue);
					yCoor2 = c->getYCoor(yValue2);
				}
				//yValue = layer->getDataSet(0)->getValue(xIndex);
				//yCoor = c->getYCoor(yValue);
				rcTest.left = xCoor - 10;
				rcTest.top = yCoor - 5;
				rcTest.right = rcTest.left + 20;
				rcTest.bottom = yCoor2 + 5;
				if (rcTest.PtInRect(point)) {
					found = true;
					int delimiterPosition = (int)dataName.find(';');
					if (delimiterPosition != std::string::npos)
						_SelectedDataSetName = dataName.substr(0, delimiterPosition);
					//break;
				}
			} else {
				if (dataSetCount == 1) {
					dataName = layer->getDataSet(0)->getDataName();
					if (dataName.find(_T("Reference")) != std::string::npos) {
						Axis* axis = FindAxis(dataName);
						yValue = layer->getDataSet(0)->getValue(xIndex);
						yCoor = c->getYCoor(yValue, axis);
					} else {
						yValue = layer->getDataSet(0)->getValue(xIndex);
						yCoor = c->getYCoor(yValue);
					}

					xCoor = plotAreaLeftX + xCoor - 30;
					yCoor = plotAreaTopY + yCoor - 35;

					rcTest.left = xCoor - 10;
					rcTest.top = yCoor - 10;
					rcTest.right = rcTest.left + 20;
					rcTest.bottom = rcTest.top + 20;

					if (rcTest.PtInRect(point)) {
						found = true;
						int delimiterPosition = (int)dataName.find(';');
						if (delimiterPosition != std::string::npos)
							_SelectedDataSetName = dataName.substr(0, delimiterPosition);
						//AfxMessageBox(_SelectedDataSetName.c_str());
						break;
					}
				} else if (dataSetCount > 0) {
					dataName = layer->getDataSet(0)->getDataName();
					for (int k = 0; k < dataSetCount; ++k) {
						yValue = layer->getDataSet(k)->getValue(xIndex);
						yCoor = c->getYCoor(yValue);

						if (dataName.find(_T("Vol")) != std::string::npos) {
							Axis* axis = _MainChart->yAxis2();
							yCoor = c->getYCoor(yValue, axis);
						}

						xCoor = plotAreaLeftX + xCoor - 30;
						yCoor = plotAreaTopY + yCoor - 35;

						rcTest.left = xCoor - 10;
						rcTest.top = yCoor - 10;
						rcTest.right = rcTest.left + 20;
						rcTest.bottom = rcTest.top + 20;

						if (rcTest.PtInRect(point)) {
							found = true;
							int delimiterPosition = (int)dataName.find(';');
							if (delimiterPosition != std::string::npos)
								_SelectedDataSetName = dataName.substr(0, delimiterPosition);
							//AfxMessageBox(_SelectedDataSetName.c_str());
							break;
						}
					}
				}
			}
		}

		if (!found) {
			rcTest.left = plotAreaLeftX;
			rcTest.top = plotAreaTopY;
			rcTest.right = plotAreaLeftX + 10;
			rcTest.bottom = plotAreaTopY + 10;
			int pos;
			if (rcTest.PtInRect(point)) {
				dataName = c->getLayer(0)->getDataSet(0)->getDataName();
				pos = dataName.find(_T(";"));
				_SelectedDataSetName = dataName.substr(0, pos);
				if (_SelectedDataSetName.find(_T("MainChart")) != std::string::npos) {
					//Reset();
					//ReleaseResource();
					break;
				} else {
					;
					//OnDeleteKey();
				}
			}

			rcTest.left = plotAreaLeftX + 11;
			rcTest.top = plotAreaTopY + 1;
			rcTest.right = plotAreaLeftX + 19;
			rcTest.bottom = plotAreaTopY + 9;

			if (rcTest.PtInRect(point)) {
				dataName = c->getLayer(0)->getDataSet(0)->getDataName();
				pos = dataName.find(_T(";"));
				_SelectedDataSetName = dataName.substr(0, pos);
				if (_SelectedDataSetName.find(_T("MainChart")) != std::string::npos) {
					_ShowAll = !_ShowAll;
					ShowAllReferenceChart(_ShowAll);
					//ShowAllIndex(_ShowAll);
					_SelectedDataSetName = _T("");
					return;
				}
				if (_SelectedDataSetName.length() > 0) {
					int pos = 0;
					if (_SelectedDataSetName.find(_T("Reference")) != std::string::npos) {
						pos = _SelectedDataSetName.find(_T(">"));
						if (pos != std::string::npos) {
							std::string index = _SelectedDataSetName.substr(pos + 1, _SelectedDataSetName.length() - pos - 1);
							int num = std::stoi(index);
							if (RefChartVector.size() > 0) {
								VtReferenceChart* chart = RefChartVector[num];
								chart->show = false;
								DrawChart(&m_ChartViewer, 0);
							}
							_SelectedDataSetName = _T("");
						}
					} else {
						;
						/*
						VtBaseIndex* index = FindIndex(_SelectedDataSetName);
						if (index)
						{
							for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
							{
								VtBaseIndex* curIndex = *it;
								if (curIndex == index)
								{
									curIndex->Show = false;
									break;
								}
							}

							_SelectedDataSetName = _T("");

							DrawChart(_ChartViewer, 0);
						}
						*/
					}
				}
			}
		}
	}

	TRACE(_SelectedDataSetName.c_str());

	//RefChartIndexBySel(_SelectedDataSetName);
	SetSelectedData(_SelectedDataSetName);
}

void VtChartWindow::ShowAllReferenceChart(bool flag)
{
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		chart->show = flag;
	}
}

std::pair<int, VtReferenceChart*> VtChartWindow::RefChartIndex(std::string symCode)
{
	int result = -1;
	int i = 0;
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->show)
		{
			if (chart->data->SymbolCode().compare(symCode) == 0)
			{
				result = i;
				return std::make_pair(result, chart);
			}
			i++;
		}
	}

	return std::make_pair(result, nullptr);
}

int VtChartWindow::RefChartIndexBySel(std::string selString)
{
	int result = -1;
	std::size_t found = selString.rfind(_T(">"));
	if (found == std::string::npos)
		return result;
	std::string indexString = selString.substr(found + 1, selString.length() - (found ));
	int i = 0;
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->show)
		{
			if (i == std::stoi(indexString))
			{
				result = i;
				return result;
			}
			i++;
		}
	}

	return result;
}

void VtChartWindow::SetSelectedData(std::string selString)
{
	if (selString.find(_T("MainChart")) != std::string::npos)
	{
		_SelectedData = _Data;
	}
	else
	{
		int refIndex = RefChartIndexBySel(selString);
		if (refIndex != -1)
		{
			VtReferenceChart* chart = RefChartVector[refIndex];
			_SelectedData = chart->data;
		}
		else
		{
			_SelectedData = _Data;
		}
	}
}

void VtChartWindow::DrawSignal(DoubleArray& timeStamp, DoubleArray& highArray, DoubleArray& lowArray, DrawArea* drawArea)
{
	if (!_System || !_Data || !_System->Running())
		return;

	POINT old, cur;
	old.x = 0;
	old.y = 0;
	cur.x = 0;
	cur.y = 0;
	for (auto it = _System->SignalVector.begin(); it != _System->SignalVector.end(); ++it)
	{
		VtSignal* sig = *it;
		int index = (int)floor(Chart::bSearch(timeStamp, sig->DateTime()));
		if (index > 0 && index <= (_NoOfPoints - 1))
		{
			double close = 0;
			if (sig->SignalType() == VtPositionType::Sell || sig->SignalType() == VtPositionType::ExitBuy)
			{
				close = highArray[index];
			}
			else if (sig->SignalType() == VtPositionType::Buy || sig->SignalType() == VtPositionType::ExitBuy)
			{
				close = lowArray[index];
			}

			DoubleArray closeData = DoubleArray(_Data->Close.data() + startIndex, _NoOfPoints);

			double curValue = closeData[index];

			int xpos = _MainChart->getXCoor(index);
			int ypos = _MainChart->getYCoor(close);
			int valuePos = _MainChart->getYCoor(curValue);
			int gap = 0;
			int rectHeight = 6;
			int rectWidth = 2;
			int triHeight = 4;
			int triWidth = 8;
			int exRectHeight = 8;
			int exRectWidth = 4;
			if (_SignalSize == 1)
			{
				rectHeight = 8;
				rectWidth = 4;
				triHeight = 6;
				triWidth = 10;
				exRectHeight = 6;
				exRectWidth = 10;
			}
			else if (_SignalSize == 2)
			{
				rectHeight = 10;
				rectWidth = 8;
				triHeight = 12;
				triWidth = 18;
				exRectHeight = 12;
				exRectWidth = 12;
			}
			int barWidth = (_Width - _SourceChart->LeftMargin() - _SourceChart->RightMargin()) / (timeStamp.len - _XtraPoints);
			if (sig->SignalType() == VtPositionType::Sell)
			{
				int xarr[7], yarr[7];
				xarr[0] = xpos;
				xarr[1] = xpos - (triWidth / 2);
				xarr[2] = xpos - (rectWidth / 2);
				xarr[3] = xpos - (rectWidth / 2);
				xarr[4] = xpos + (rectWidth / 2);
				xarr[5] = xpos + (rectWidth / 2);
				xarr[6] = xpos + (triWidth / 2);

				yarr[0] = ypos - gap;
				yarr[1] = ypos - (triHeight + gap);
				yarr[2] = ypos - (triHeight + gap);
				yarr[3] = ypos - (gap + rectHeight + triHeight);
				yarr[4] = ypos - (gap + rectHeight + triHeight);
				yarr[5] = ypos - (triHeight + gap);
				yarr[6] = ypos - (triHeight + gap);
				drawArea->polygon(IntArray(xarr, 7), IntArray(yarr, 7), 0xffffff, 0x0000ff);

				int triX[3], triY[3];
				triX[0] = xpos + barWidth / 2;
				triX[1] = triX[0] + 3;
				triX[2] = triX[0] + 3;

				triY[0] = valuePos;
				triY[1] = valuePos - 2;
				triY[2] = valuePos + 2;

				drawArea->polygon(IntArray(triX, 3), IntArray(triY, 3), 0x0000ff, 0x0000ff);

				if (_SelectedSystem)
					drawArea->circle(xpos, ypos - (triHeight + gap), 4, 4, 0xffffff, 0x000000);
			}
			else if (sig->SignalType() == VtPositionType::Buy)
			{
				int xarr[7], yarr[7];
				xarr[0] = xpos;
				xarr[1] = xpos - (triWidth / 2);
				xarr[2] = xpos - (rectWidth / 2);
				xarr[3] = xpos - (rectWidth / 2);
				xarr[4] = xpos + (rectWidth / 2);
				xarr[5] = xpos + (rectWidth / 2);
				xarr[6] = xpos + (triWidth / 2);

				yarr[0] = ypos + gap;
				yarr[1] = ypos + (triHeight + gap);
				yarr[2] = ypos + (triHeight + gap);
				yarr[3] = ypos + (gap + rectHeight + triHeight);
				yarr[4] = ypos + (gap + rectHeight + triHeight);
				yarr[5] = ypos + (triHeight + gap);
				yarr[6] = ypos + (triHeight + gap);

				drawArea->polygon(IntArray(xarr, 7), IntArray(yarr, 7), 0xffffff, 0xff0000);


				int triX[3], triY[3];
				triX[0] = xpos - barWidth / 2;
				triX[1] = triX[0] - 3;
				triX[2] = triX[0] - 3;

				triY[0] = valuePos;
				triY[1] = valuePos - 2;
				triY[2] = valuePos + 2;

				drawArea->polygon(IntArray(triX, 3), IntArray(triY, 3), 0xff0000, 0xff0000);

				if (_SelectedSystem)
					drawArea->circle(xpos, ypos + (triHeight + gap), 4, 4, 0xffffff, 0x000000);
			}
			else if (sig->SignalType() == VtPositionType::ExitBuy)
			{
				if (_SignalSize == 1)
				{
					rectHeight = 4;
				}
				else if (_SignalSize == 2)
				{
					rectHeight = 6;
				}

				int xarr[11], yarr[11];
				xarr[0] = xpos;
				xarr[1] = xpos - (triWidth / 2);
				xarr[2] = xpos - (rectWidth / 2);
				xarr[3] = xpos - (rectWidth / 2);

				xarr[4] = xpos - (exRectWidth / 2);
				xarr[5] = xpos - (exRectWidth / 2);
				xarr[6] = xpos + (exRectWidth / 2);
				xarr[7] = xpos + (exRectWidth / 2);


				xarr[8] = xpos + (rectWidth / 2);
				xarr[9] = xpos + (rectWidth / 2);
				xarr[10] = xpos + (triWidth / 2);

				yarr[0] = ypos - gap;
				yarr[1] = ypos - (triHeight + gap);
				yarr[2] = ypos - (triHeight + gap);
				yarr[3] = ypos - (gap + rectHeight + triHeight);

				yarr[4] = ypos - (gap + rectHeight + triHeight);
				yarr[5] = ypos - (gap + rectHeight + triHeight + exRectHeight);
				yarr[6] = ypos - (gap + rectHeight + triHeight + exRectHeight);
				yarr[7] = ypos - (gap + rectHeight + triHeight);

				yarr[8] = ypos - (gap + rectHeight + triHeight);
				yarr[9] = ypos - (triHeight + gap);
				yarr[10] = ypos - (triHeight + gap);
				drawArea->polygon(IntArray(xarr, 11), IntArray(yarr, 11), 0xffffff, 0x0000ff);

				drawArea->line(xarr[4], yarr[4], xarr[6], yarr[6], 0xffffff);
				drawArea->line(xarr[5], yarr[5], xarr[7], yarr[7], 0xffffff);

				int triX[3], triY[3];
				triX[0] = xpos + barWidth / 2;
				triX[1] = triX[0] + 3;
				triX[2] = triX[0] + 3;

				triY[0] = valuePos;
				triY[1] = valuePos - 2;
				triY[2] = valuePos + 2;

				drawArea->polygon(IntArray(triX, 3), IntArray(triY, 3), 0x0000ff, 0x0000ff);

				if (_SelectedSystem)
					drawArea->circle(xpos, ypos - (triHeight + gap), 4, 4, 0xffffff, 0x000000);
			}
			else if (sig->SignalType() == VtPositionType::ExitSell)
			{
				if (_SignalSize == 1)
				{
					rectHeight = 4;
				}
				else if (_SignalSize == 2)
				{
					rectHeight = 6;
				}
				int xarr[11], yarr[11];
				xarr[0] = xpos;
				xarr[1] = xpos - (triWidth / 2);
				xarr[2] = xpos - (rectWidth / 2);
				xarr[3] = xpos - (rectWidth / 2);

				xarr[4] = xpos - (exRectWidth / 2);
				xarr[5] = xpos - (exRectWidth / 2);
				xarr[6] = xpos + (exRectWidth / 2);
				xarr[7] = xpos + (exRectWidth / 2);

				xarr[8] = xpos + (rectWidth / 2);
				xarr[9] = xpos + (rectWidth / 2);
				xarr[10] = xpos + (triWidth / 2);

				yarr[0] = ypos + gap;
				yarr[1] = ypos + (triHeight + gap);
				yarr[2] = ypos + (triHeight + gap);
				yarr[3] = ypos + (gap + rectHeight + triHeight);

				yarr[4] = ypos + (gap + rectHeight + triHeight);
				yarr[5] = ypos + (gap + rectHeight + triHeight + exRectHeight);
				yarr[6] = ypos + (gap + rectHeight + triHeight + exRectHeight);
				yarr[7] = ypos + (gap + rectHeight + triHeight);

				yarr[8] = ypos + (gap + rectHeight + triHeight);
				yarr[9] = ypos + (triHeight + gap);
				yarr[10] = ypos + (triHeight + gap);

				drawArea->polygon(IntArray(xarr, 11), IntArray(yarr, 11), 0xffffff, 0xff0000);
				drawArea->line(xarr[4], yarr[4], xarr[6], yarr[6], 0xffffff);
				drawArea->line(xarr[5], yarr[5], xarr[7], yarr[7], 0xffffff);

				int triX[3], triY[3];
				triX[0] = xpos - barWidth / 2;
				triX[1] = triX[0] - 3;
				triX[2] = triX[0] - 3;

				triY[0] = valuePos;
				triY[1] = valuePos - 2;
				triY[2] = valuePos + 2;

				drawArea->polygon(IntArray(triX, 3), IntArray(triY, 3), 0xff0000, 0xff0000);

				if (_SelectedSystem)
					drawArea->circle(xpos, ypos + (triHeight + gap), 4, 4, 0xffffff, 0x000000);
			}
			cur.x = xpos;
			cur.y = valuePos;
			if (old.x != 0 && old.y != 0)
			{
				if (sig->SignalType() == VtPositionType::ExitSell ||
					sig->SignalType() == VtPositionType::ExitBuy)
					drawArea->line(old.x, old.y, cur.x, cur.y, _MainChart->dashLineColor(0x000000, Chart::DotLine), 1);

			}

			old.x = xpos;
			old.y = valuePos;

			if (index == (_NoOfPoints - 1))
				break;
		}
	}
}

bool VtChartWindow::HitTestSystem(CPoint point)
{
	if (!_MainChart)
		return false;
	if (!_System)
		return false;

	_SelectedSystem = false;
	DoubleArray timeStamp = DoubleArray(_Data->DateTime.data() + startIndex, _NoOfPoints);
	DoubleArray highArray = DoubleArray(_Data->High.data() + startIndex, _NoOfPoints);
	DoubleArray lowArray = DoubleArray(_Data->Low.data() + startIndex, _NoOfPoints);
	CRect rcTest;
	for (auto it = _System->SignalVector.begin(); it != _System->SignalVector.end(); ++it)
	{
		VtSignal* sig = *it;
		int index = (int)floor(Chart::bSearch(timeStamp, sig->DateTime()));
		if (index > 0 && index < (_NoOfPoints - 1))
		{

			double close = 0;
			if (sig->SignalType() == VtPositionType::Sell || sig->SignalType() == VtPositionType::ExitBuy)
			{
				close = highArray[index];
			}
			else if (sig->SignalType() == VtPositionType::Buy || sig->SignalType() == VtPositionType::ExitBuy)
			{
				close = lowArray[index];
			}
			int xpos = _MainChart->getXCoor(index);
			int ypos = _MainChart->getYCoor(close);
			int valuePos = _MainChart->getYCoor(sig->Price());
			int gap = 0;
			int rectHeight = 6;
			int rectWidth = 2;
			int triHeight = 4;
			int triWidth = 8;
			int exRectHeight = 8;
			int exRectWidth = 4;
			if (_SignalSize == 1)
			{
				rectHeight = 8;
				rectWidth = 4;
				triHeight = 6;
				triWidth = 10;
				exRectHeight = 6;
				exRectWidth = 10;
			}
			else if (_SignalSize == 2)
			{
				rectHeight = 10;
				rectWidth = 8;
				triHeight = 12;
				triWidth = 18;
				exRectHeight = 8;
				exRectWidth = 18;
			}
			if (sig->SignalType() == VtPositionType::Sell)
			{
				rcTest.left = xpos - (triWidth / 2);
				rcTest.top = ypos - (gap + rectHeight + triHeight);
				rcTest.right = xpos + (triWidth / 2);
				rcTest.bottom = ypos + gap;
			}
			else if (sig->SignalType() == VtPositionType::Buy)
			{
				rcTest.left = xpos - (triWidth / 2);
				rcTest.top = ypos + gap;
				rcTest.right = xpos + (triWidth / 2);
				rcTest.bottom = ypos + (gap + rectHeight + triHeight);

			}
			else if (sig->SignalType() == VtPositionType::ExitBuy)
			{
				rcTest.left = xpos - (triWidth / 2);
				rcTest.top = ypos - (gap + rectHeight + triHeight);
				rcTest.right = xpos + (triWidth / 2);
				rcTest.bottom = ypos + gap;
			}
			else if (sig->SignalType() == VtPositionType::ExitSell)
			{
				rcTest.left = xpos - (triWidth / 2);
				rcTest.top = ypos + gap;
				rcTest.right = xpos + (triWidth / 2);
				rcTest.bottom = ypos + (gap + rectHeight + triHeight);
			}

			if (rcTest.PtInRect(point))
			{
				_SelectedSystem = true;
				break;
			}

			if (index == (_NoOfPoints - 1))
				break;
		}
	}

	return _SelectedSystem;
}

void VtChartWindow::ShowSystemProperty(VtSystem* system)
{
	if (!system)
		return;

	VtSystemProperty sysProp;
	sysProp.System(_System);
	sysProp.DoModal();
}

void VtChartWindow::MouseToChart(CPoint src, VtPoint& des)
{
	des.x = src.x;
	des.y = src.y;
}

void VtChartWindow::DrawRealtimeValues(DrawArea* d)
{
	if (!d)
		return;

	for (auto it = _LayerList.rbegin(); it != _LayerList.rend(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		DrawCurrentValue(std::move(layerInfo), d);
	}
}

void VtChartWindow::DrawRealtimeValues(DrawArea* d, std::vector<VtLayerInfo>& layerList)
{
	if (!d)
		return;

	for (auto it = layerList.rbegin(); it != layerList.rend(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		DrawCurrentValue(std::move(layerInfo), d);
	}
}

void VtChartWindow::DrawCurrentValue(VtLayerInfo&& layerInfo, DrawArea* drawArea)
{
	PlotArea* plotArea = layerInfo.chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + layerInfo.chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + layerInfo.chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	int valuePos = plotAreaTopY - 35 + layerInfo.chart->getYCoor(layerInfo.data->RealTimeClose(), layerInfo.axis);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap - 5;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = rectLeft - 14;
	xarr[1] = rectLeft;
	xarr[2] = rectLeft;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), layerInfo.color, layerInfo.color);
	drawArea->rect(rectLeft, rectTop, rectLeft + 85, rectBottom, layerInfo.color, layerInfo.color);
	std::string strValue;
	if (layerInfo.data->Currency().compare(_T("KRW")) == 0)
		strValue = string_cast<double>(layerInfo.data->RealTimeClose(), layerInfo.data->Decimal(), convert::thou_sep);
	else
		strValue = string_cast<double>(layerInfo.data->RealTimeClose(), layerInfo.data->Decimal(), convert::thou_sep);
	strValue.append(_T(":"));
	strValue.append(layerInfo.data->Initial());
	TTFText* t = drawArea->text(strValue.c_str(), _T("굴림"), 10);
	if (layerInfo.color != 0x00ffff00)
		t->draw(rectLeft, valuePos - 7, 0xffffff);
	else
		t->draw(rectLeft, valuePos - 7, 0x000000);

	t->destroy();
}

void VtChartWindow::ValueToPixel()
{
	int startIndex = 0;
	int endIndex = 0;
	GetZoomRange(&m_ChartViewer, startIndex, endIndex);
	_shapesCollection.ValueToPixel(_StartID, startIndex, _MainChart);
}

void VtChartWindow::PixelToValue()
{
	int startIndex = 0;
	int endIndex = 0;
	GetZoomRange(&m_ChartViewer, startIndex, endIndex);

	_shapesCollection.PixelToValue(_StartID, startIndex, _MainChart);
}

void VtChartWindow::SetDataCount()
{
	CString count;
	count.Format(_T("%d"), _NoOfPoints);
	_EditCount.SetWindowText(count);
}

/*
void VtChartWindow::DrawRefChart()
{
	int i = 0;
	VtLayerInfo layerInfo;
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (!chart->show)
			continue;

		VtChartData* chartData = chart->data;

		if (!chartData->Filled())
			continue;

		Axis* leftAxis = nullptr;

		DoubleArray highData = DoubleArray(chartData->High.data() + startIndex, _NoOfPoints);
		DoubleArray lowData = DoubleArray(chartData->Low.data() + startIndex, _NoOfPoints);
		DoubleArray openData = DoubleArray(chartData->Open.data() + startIndex, _NoOfPoints);
		DoubleArray closeData = DoubleArray(chartData->Close.data() + startIndex, _NoOfPoints);

		// Add a line layer to for the third data set using blue (0000cc) color, with a line width of 2
		// pixels. Bind the layer to the third y-axis.
		std::string exName = chartData->SymbolCode();
		exName.append(chartData->Exchange());
		CString refName;
		refName.Format(_T("Reference>%d;"), i);
		std::string dataName = refName;
		dataName.append(exName);

		chartData->LineColor(_ColorMgr->GetColor(chart->data->ColorIndex()));

		Layer *layer2 = nullptr;

		if (chart->embedded)
		{
			if (chart->shareAxisWithMain)
			{
				leftAxis = _MainChart->yAxis();
				layerInfo.shareAxis = true;
			}
			else
			{
				layerInfo.shareAxis = false;
				leftAxis = _MainChart->addAxis(Chart::Right, _YAxisGap);

				Axis* mainAxis = _MainChart->yAxis();
				std::string name = refName;
				name = name.substr(0, name.length() - 1);
				if (_SelectedDataSetName.length() > 0 && _SelectedDataSetName.compare(name) == 0)
				{
					leftAxis->setColors(0x00dddddd, 0x00000000, 0x00000000);
					//mainAxis->setColors(0xffdddddd, 0xff000000, 0xff000000);
					leftAxis->setTitle(chartData->SymbolCode().c_str())->setAlignment(Chart::TopRight);
				}
				else
				{
					leftAxis->setColors(0xffdddddd, 0xff000000, 0xff000000);
					//mainAxis->setColors(0x00dddddd, 0x00000000, 0x00000000);
				}
			}


			if (chart->data->MainChartType() == VtMainChartType::CandleStick)
			{
				layer2 = _MainChart->addCandleStickLayer(highData, lowData, openData,
					closeData, 0xff0000, 0x0000ff);
				((CandleStickLayer*)layer2)->setColors(0xff0000, 0xff0000, 0x0000ff, 0x0000ff);
				DataSet* dataSet = layer2->getDataSet(0);
				dataSet->setDataName(dataName.c_str());
			}
			else
			{
				layer2 = _MainChart->addLineLayer(closeData, chart->color, dataName.c_str());
			}

			layer2->setLegendOrder(i);
			layer2->setLineWidth(2);
			layer2->setUseYAxis(leftAxis);

			layerInfo.embed = true;
			layerInfo.chart = _MainChart;
			layerInfo.axis = leftAxis;
			layerInfo.layer = layer2;

		}
		else
		{
			XYChart* c = _SourceChart->AddReferenceChart2(chart->height, dataName, chart->type, 2, chart->color, &layer2, highData, lowData, openData, closeData, std::string(refName));

			c->yAxis()->setOffset(_YAxisGap, 0);
			c->setPlotArea(30, 35, _Width - 120, chart->height, 0xffffff, 0xffffff, 0xff000000, c->dashLineColor(0xdddddd, Chart::DotLine), -1);
			c->setNumberFormat(',');

			layerInfo.embed = false;
			layerInfo.chart = c;
			layerInfo.axis = c->yAxis();
			layerInfo.layer = layer2;

		}


		layerInfo.selected = false;
		layerInfo.data = chartData;
		layerInfo.color = chartData->LineColor();
		_LayerList.push_back(layerInfo);
		i++;
	}
}*/
void VtChartWindow::DrawRefChart(std::vector<VtLayerInfo>& layerList)
{
	int i = 0;
	VtLayerInfo layerInfo;
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (!chart->show)
			continue;

		VtChartData* chartData = chart->data;

		if (!chartData->Filled())
			continue;

		Axis* leftAxis = nullptr;

		std::vector<double>& open_vec = chartData->GetDataArray(_T("open"));
		std::vector<double>& high_vec = chartData->GetDataArray(_T("high"));
		std::vector<double>& low_vec = chartData->GetDataArray(_T("low"));
		std::vector<double>& close_vec = chartData->GetDataArray(_T("close"));

		DoubleArray highData = DoubleArray(high_vec.data() + startIndex, _NoOfPoints);
		DoubleArray lowData = DoubleArray(low_vec.data() + startIndex, _NoOfPoints);
		DoubleArray openData = DoubleArray(open_vec.data() + startIndex, _NoOfPoints);
		DoubleArray closeData = DoubleArray(close_vec.data() + startIndex, _NoOfPoints);

		// Add a line layer to for the third data set using blue (0000cc) color, with a line width of 2
		// pixels. Bind the layer to the third y-axis.
		std::string exName = chartData->SymbolCode();
		exName.append(chartData->Exchange());
		CString refName;
		refName.Format(_T("Reference>%d;"), i);
		std::string dataName = refName;
		dataName.append(exName);

		chartData->LineColor(_ColorMgr->GetColor(chart->data->ColorIndex()));

		Layer *layer2 = nullptr;

		if (chart->embedded)
		{
			if (chart->shareAxisWithMain)
			{
				leftAxis = _MainChart->yAxis();
				layerInfo.shareAxis = true;
			}
			else
			{
				layerInfo.shareAxis = false;
				leftAxis = _MainChart->addAxis(Chart::Right, _YAxisGap);

				std::string name = refName;
				name = name.substr(0, name.length() - 1);
				if (_SelectedDataSetName.length() > 0 && _SelectedDataSetName.compare(name) == 0)
				{
					leftAxis->setColors(0x00dddddd, 0x00000000, 0x00000000);
					leftAxis->setTitle(chartData->SymbolCode().c_str())->setAlignment(Chart::TopRight);
				}
				else
				{
					leftAxis->setColors(0xffdddddd, 0xff000000, 0xff000000);
				}
				std::string yAxisFormat;
				yAxisFormat = _T("{value|");
				yAxisFormat.append(std::to_string(chart->data->Decimal()));
				yAxisFormat.append(_T(",.}"));
				leftAxis->setLabelFormat(yAxisFormat.c_str());
			}


			if (chart->data->MainChartType() == VtMainChartType::CandleStick)
			{
				layer2 = _MainChart->addCandleStickLayer(highData, lowData, openData,
					closeData, 0xff0000, 0x0000ff);
				((CandleStickLayer*)layer2)->setColors(0xff0000, 0xff0000, 0x0000ff, 0x0000ff);
				DataSet* dataSet = layer2->getDataSet(0);
				dataSet->setDataName(dataName.c_str());
			}
			else
			{
				layer2 = _MainChart->addLineLayer(closeData, chart->color, dataName.c_str());
			}

			layer2->setLegendOrder(i);
			layer2->setLineWidth(2);
			layer2->setUseYAxis(leftAxis);

			layerInfo.embed = true;
			layerInfo.chart = _MainChart;
			layerInfo.axis = leftAxis;
			layerInfo.layer = layer2;

		}
		else
		{
			XYChart* c = _SourceChart->AddReferenceChart2(chart->height, dataName, chart->type, 2, chart->color, &layer2, highData, lowData, openData, closeData, std::string(refName));

			c->yAxis()->setOffset(_YAxisGap, 0);
			c->setPlotArea(30, 35, _Width - 120, chart->height, 0xffffff, 0xffffff, 0xff000000, c->dashLineColor(0xdddddd, Chart::DotLine), -1);
			//c->setNumberFormat(',');
			std::string yAxisFormat;
			yAxisFormat = _T("{value|");
			yAxisFormat.append(std::to_string(chart->data->Decimal()));
			yAxisFormat.append(_T(",.}"));
			c->yAxis()->setLabelFormat(yAxisFormat.c_str());

			layerInfo.embed = false;
			layerInfo.chart = c;
			layerInfo.axis = c->yAxis();
			layerInfo.layer = layer2;

		}


		layerInfo.selected = false;
		layerInfo.data = chartData;
		layerInfo.color = chartData->LineColor();
		layerList.push_back(layerInfo);
		i++;
	}
}

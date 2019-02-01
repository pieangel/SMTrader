#include "stdafx.h"
#include "VtChartFrm.h"
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
//#include "VcExchangeManager.h"
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
//#include "VtChartPropertyDlg.h"
//#include "VtIndexDlg.h"
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
using namespace convert;
using Poco::NumberFormatter;

inline unsigned int ARGB32(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255)
{
	return (b | (g << 8) | (r << 16) | (a << 24));
}

VtChartFrm::VtChartFrm() 
	: VtUniqueChartID()
{
	
	_ColorMgr = new VtColorManager();
	
	_Data = nullptr;

	_creationPenColor.A = 0;
	_creationPenColor.R = 0;
	_creationPenColor.G = 0;
	_creationPenColor.B = 0;

	_creationFillColor.A = 120;
	_creationFillColor.R = 21;
	_creationFillColor.G = 225;
	_creationFillColor.B = 103;

	for (int i = 0; i < 6; i++)
	{
		_fiboPoints[i].x = 0;
		_fiboPoints[i].y = 0;
	}

	_ChartTile = _T("ChartTitle");

	//_ChartOrderManager = new VtChartOrderManager();
	
	_ChartOrderManager = nullptr;

	_ParentChartWnd = nullptr;

	MACDHeight = 180;

	_SelectedChart = false;

	_System = nullptr;

	_SelectedSystem = false;

	_CurSystemType = VtSystemType::None;

	_TempSystem = nullptr;// new VtSystem();

	_ShowDailyLine = false;

	_ShowAll = true;

	_Init = false;

	_Index = 0;
	
}


VtChartFrm::~VtChartFrm()
{
	
	if (_ColorMgr)
	{
		delete _ColorMgr;
		_ColorMgr = nullptr;
	}
}

void VtChartFrm::PlaySignalSound(VtPositionType pos)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath = configMgr->GetAppPath();
	std::string buyPath = appPath;
	buyPath.append(_T("\\SignalBuy.wav"));
	std::string sellPath = appPath;
	sellPath.append(_T("\\SignalSell.wav"));
	if (pos == VtPositionType::Buy)
	{
		//PlaySound(buyPath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
		//PlaySound(NULL, NULL, 0);
		_SoundPipe.PlaySoundFile(buyPath);
	}
	else if (pos == VtPositionType::Sell)
	{
		//PlaySound(sellPath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
		//PlaySound(NULL, NULL, 0);
		_SoundPipe.PlaySoundFile(sellPath);
	}
	else if (pos == VtPositionType::ExitBuy)
	{
		_SoundPipe.PlaySoundFile(sellPath);
	}
	else if (pos == VtPositionType::ExitSell)
	{
		_SoundPipe.PlaySoundFile(buyPath);
	}
}

void VtChartFrm::Reset()
{
	_SourceChart = nullptr;

	_Data = nullptr;

	_SelectedChart = false;

	_System = nullptr;

	_SelectedSystem = false;

	_CurSystemType = VtSystemType::None;

	_TempSystem = new VtSystem();

	_ShowDailyLine = false;

	_ShowAll = true;

	_Init = false;
}

void VtChartFrm::ReleaseResource()
{
	if (nullptr != _SourceChart)
	{
		delete _SourceChart;
		_SourceChart = nullptr;
	}
	if (_ChartOrderManager)
	{
		delete _ChartOrderManager;
		_ChartOrderManager = nullptr;
	}

	if (_System)
	{
		delete _System;
		_System = nullptr;
	}

	for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
	{
		delete *it;
	}
	BandVector.clear();

	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		delete *it;
	}
	IndexVector.clear();

	for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
	{
		delete *it;
	}
	StressVector.clear();

	if (_TempSystem)
	{
		delete _TempSystem;
		_TempSystem = nullptr;
	}

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	if (_Data)
	{
		// Remove the chart window used from the map.
		_Data->RemoveChartWindow(_ParentChartWnd);
		VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
		// Remove the request of chart data from the timer.
		chartDataMgr->RemoveChartDataRequest(_Data);
	}

	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		// Return the used color
		_ColorMgr->ReturnColorIndex(chart->data->ColorIndex());
		// Remove the chart window used from the map.
		chart->data->RemoveChartWindow(_ParentChartWnd);
		VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
		// Remove the request of chart data from the timer.
		chartDataMgr->RemoveChartDataRequest(chart->data);
		delete *it;
	}
}

void VtChartFrm::ChangeChartData(VtChartDataRequest&& req, std::string newKey)
{
	/*
	VcClient* client = VcClient::GetInstance();
	VtChartWindowManager* chartWndMgr = VtChartWindowManager::GetInstance();
	VtChartDataRequestManager* reqMgr = VtChartDataRequestManager::GetInstance();

	if (_Data)
	{
		if (_Data->Key().compare(newKey) != 0)
		{
			chartWndMgr->ChangeChartWindow(newKey, _ParentChartWnd);
			//client->GetChartData(std::move(req));
			reqMgr->PutRequest(std::move(req));
		}
	}

	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;

		std::string oKey = VtChartDataManager::MakeChartDataKey(chart->data->Exchange(), chart->data->SymbolCode(), chart->data->ChartType(), chart->data->Cycle());
		std::string nKey = VtChartDataManager::MakeChartDataKey(chart->data->Exchange(), chart->data->SymbolCode(), req.chartType, req.cycle);


		if (oKey.compare(nKey) != 0)
		{
			_ChangingDataMap[nKey] = oKey;


			chartWndMgr->ChangeRefChartWindow(oKey, nKey, _ParentChartWnd);
			VtChartDataRequest newReq = req;
			newReq.type = VtChartRequestType::REF;
			newReq.exchange = chart->data->Exchange();
			newReq.currency = chart->data->Currency();
			newReq.productCode = chart->data->SymbolCode();

			reqMgr->PutRequest(std::move(newReq));
			//client->GetChartData(std::move(newReq));
		}
	}
	*/
}

std::string VtChartFrm::GetMainDataKey()
{
	std::string result(_T(""));
	if (_Data)
	{
		result = _Data->Key();
	}
	
	return result;
}

int VtChartFrm::CalcChartHeight()
{
	if (!_ParentChartWnd || !_ChartViewer)
		return 0;

	CRect rect;

	_ParentChartWnd->GetClientRect(rect);

	int height = rect.Height();

	int totalIndexHeight = CalcTotalIndexHeight();
	height -= totalIndexHeight;
	int totalRefChartHeight = CalcTotalRefChartHeight();
	height -= totalRefChartHeight;

	if (_ParentChartWnd->Active())
		height = height - 72 - _ParentChartWnd->CtrlHeight();
	else
		height = height - 69 - _ParentChartWnd->CtrlHeight();

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

void VtChartFrm::ShowProperty()
{
	int pos = 0;
	if (_SelectedDataSetName.find(_T("MainChart")) != std::string::npos)
	{
		pos = 1;
	}
	else if (_SelectedDataSetName.find(_T("Reference")) != std::string::npos)
	{
		pos = _SelectedDataSetName.find(_T(">"));
		if (pos == -1)
			return;
		std::string index = _SelectedDataSetName.substr(pos + 1, _SelectedDataSetName.length() - pos);
		int idx = std::stoi(index);
		if (idx < 0 || idx >= RefChartVector.size())
			return;

		VtReferenceChart* refChart = RefChartVector[idx];
// 		VtIndexDlg dlg;
// 		dlg.Mode = 4;
// 		dlg._CurRefChart = refChart;
// 		dlg._ChartFrm = this;
// 		dlg.DoModal();
	}
	else
	{
		VtBaseIndex* index = FindIndex(_SelectedDataSetName);
		VtBaseBand* band = FindBand(_SelectedDataSetName);
		VtBaseStress* stress = FindStress(_SelectedDataSetName);

		if (index)
		{
// 			VtIndexDlg dlg;
// 			dlg.Mode = 1;
// 			dlg._ChartFrm = this;
// 			dlg._CurIndex = index;
// 			dlg.DoModal();
		}
		else if (band)
		{
// 			VtIndexDlg dlg;
// 			dlg.Mode = 2;
// 			dlg._ChartFrm = this;
// 			dlg._CurBand = band;
// 			dlg.DoModal();
		}
		else if (stress)
		{
// 			VtIndexDlg dlg;
// 			dlg.Mode = 3;
// 			dlg._ChartFrm = this;
// 			dlg._CurStress = stress;
// 			dlg.DoModal();
		}
	}
}

void VtChartFrm::OnSystemProperty()
{
// 	VtChartPropertyDlg dlg;
// 	dlg.ChartFrm(this);
// 	dlg.Mode(0);
// 	dlg.CurSystem(_System);
// 	dlg.DoModal();
}

int VtChartFrm::CalcTotalIndexHeight()
{
	int totalHeight = 0;
	int margin = 3;
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		VtBaseIndex* index = *it;
		if (!index->Show)
			continue;

		switch (index->IndexBase.Type)
		{
		case VtIndexType::Macd:
			totalHeight += ((VtMacdIndex*)index)->Height + margin;
			break;
		case VtIndexType::Rsi:
			totalHeight += ((VtRsiIndex*)index)->Height + margin;
			break;
		case VtIndexType::Stochastic:
			totalHeight += ((VtStochasticIndex*)index)->Height + margin;
			break;
		case VtIndexType::Trix:
			totalHeight += ((VtTrixIndex*)index)->Height + margin;
			break;
		case VtIndexType::Atr:
			totalHeight += ((VtAtrIndex*)index)->Height + margin;
			break;
		}
	}

	return totalHeight;
}

int VtChartFrm::CalcTotalRefChartHeight()
{
	int total = 0;
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (!chart->embedded && chart->show)
		{
			total += chart->height + 4;
		}
	}

	return total;
}

void VtChartFrm::DrawIndex(std::vector<VtLayerInfo>& layerList)
{
	if (!_SourceChart || !_MainChart)
		return;
	XYChart* chart = nullptr;
	PlotArea* plotArea = nullptr;
	VtLayerInfo layerInfo;
	BYTE r, g, b;
	COLORREF color;
	int targetColor;
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		VtBaseIndex* index = *it;
		if (!index->Show)
			continue;

		switch (index->IndexBase.Type)
		{
		default:
			break;
		case VtIndexType::SimpleMa:
			_SourceChart->addSimpleMovingAvg2(index->IndexBase.Name.c_str(), ((VtMaIndex*)index)->Period, ((VtMaIndex*)index)->Color);
			break;
		case VtIndexType::DataVolume:
			layerInfo.layer = _SourceChart->addVolBars3(
				index->IndexBase.Name.c_str(),
				((VtVolumeIndex*)index)->Height,
				((VtVolumeIndex*)index)->UpColor,
				((VtVolumeIndex*)index)->DownColor,
				((VtVolumeIndex*)index)->FlatColor);
			break;
		case VtIndexType::BollingerBand:
			color = ((VtBollingerBandIndex*)index)->FillColor;
			r = GetRValue(color);
			g = GetGValue(color);
			b = GetBValue(color);

			targetColor = ARGB32(r, g, b, 200);
			layerInfo.layer = _SourceChart->addBollingerBand2(
				index->IndexBase.Name.c_str(),
				((VtBollingerBandIndex*)index)->Period,
				((VtBollingerBandIndex*)index)->Width,
				((VtBollingerBandIndex*)index)->LineColor,
				targetColor);
			break;
		case VtIndexType::PriceChannel:
			color = ((VtBollingerBandIndex*)index)->FillColor;
			r = GetRValue(color);
			g = GetGValue(color);
			b = GetBValue(color);

			targetColor = ARGB32(r, g, b, 200);
			layerInfo.layer = _SourceChart->addPriceChannel2(
				index->IndexBase.Name.c_str(),
				((VtPriceChannelIndex*)index)->Period,
				((VtPriceChannelIndex*)index)->LineColor,
				targetColor);
			break;
		case VtIndexType::Macd:
			chart = _SourceChart->addMACD2(
				index->IndexBase.Name.c_str(),
				((VtMacdIndex*)index)->Height,
				((VtMacdIndex*)index)->ShortPeriod,
				((VtMacdIndex*)index)->LongPeriod,
				((VtMacdIndex*)index)->Period,
				((VtMacdIndex*)index)->Color,
				((VtMacdIndex*)index)->SignalColor,
				((VtMacdIndex*)index)->DivColor);
			chart->yAxis()->setOffset(_YAxisGap, 0);
			chart->setPlotArea(30, 35, _Width - 120, ((VtMacdIndex*)index)->Height, 0xffffff, 0xffffff, 0xffffff, chart->dashLineColor(0xdddddd, Chart::DotLine), -1);
			chart->setNumberFormat(',');
			break;
		case VtIndexType::Rsi:
			chart = _SourceChart->addRSI2(
				index->IndexBase.Name.c_str(),
				((VtRsiIndex*)index)->Height,
				((VtRsiIndex*)index)->Period,
				((VtRsiIndex*)index)->Color,
				((VtRsiIndex*)index)->Range,
				((VtRsiIndex*)index)->UpColor,
				((VtRsiIndex*)index)->DownColor);
			chart->yAxis()->setOffset(_YAxisGap, 0);
			chart->setPlotArea(30, 35, _Width - 120, ((VtRsiIndex*)index)->Height, 0xffffff, 0xffffff, 0xffffff, chart->dashLineColor(0xdddddd, Chart::DotLine), -1);
			chart->setNumberFormat(',');
			break;
		case VtIndexType::Stochastic:
			chart = _SourceChart->addStochastics2(
				index->IndexBase.Name.c_str(),
				((VtStochasticIndex*)index)->Height,
				((VtStochasticIndex*)index)->FastK,
				((VtStochasticIndex*)index)->SlowK,
				((VtStochasticIndex*)index)->SlowD,
				((VtStochasticIndex*)index)->ColorSlowK,
				((VtStochasticIndex*)index)->ColorSlowD, 30, 0xccff0000, 0xcc0000ff);
			chart->yAxis()->setOffset(_YAxisGap, 0);
			chart->setPlotArea(30, 35, _Width - 120, ((VtStochasticIndex*)index)->Height, 0xffffff, 0xffffff, 0xffffff, chart->dashLineColor(0xdddddd, Chart::DotLine), -1);
			break;
		case VtIndexType::Trix:
			chart = _SourceChart->addTRIX2(
				index->IndexBase.Name.c_str(),
				((VtTrixIndex*)index)->Height,
				((VtTrixIndex*)index)->Period,
				((VtTrixIndex*)index)->Color);
			chart->yAxis()->setOffset(_YAxisGap, 0);
			chart->setPlotArea(30, 35, _Width - 120, ((VtTrixIndex*)index)->Height, 0xffffff, 0xffffff, 0xffffff, chart->dashLineColor(0xdddddd, Chart::DotLine), -1);
			chart->setNumberFormat(',');
			break;
		case VtIndexType::Atr:
			chart = _SourceChart->addATR2(
				index->IndexBase.Name.c_str(),
				((VtAtrIndex*)index)->Height,
				((VtAtrIndex*)index)->Period1,
				((VtAtrIndex*)index)->Period2,
				((VtAtrIndex*)index)->Color1,
				((VtAtrIndex*)index)->Color2);
			chart->yAxis()->setOffset(_YAxisGap, 0);
			chart->setPlotArea(30, 35, _Width - 120, ((VtAtrIndex*)index)->Height, 0xffffff, 0xffffff, 0xffffff, chart->dashLineColor(0xdddddd, Chart::DotLine), -1);
			chart->setNumberFormat(',');
			break;
		}
	}
}


void VtChartFrm::DrawSignal(DoubleArray& timeStamp, DoubleArray& highArray, DoubleArray& lowArray, DrawArea* drawArea)
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
					sig->SignalType() == VtPositionType::ExitBuy )
					drawArea->line(old.x, old.y, cur.x, cur.y, _MainChart->dashLineColor(0x000000, Chart::DotLine), 1);
				
			}

			old.x = xpos;
			old.y = valuePos;

			if (index == (_NoOfPoints - 1))
				break;
		}
	}
}

void VtChartFrm::DrawBand()
{
	_ShowDailyLine = false;
	for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
	{
		VtBaseBand* band = *it;
		switch (band->BandBase.Type)
		{
		case VtBandType::Ndaily:

			_SourceChart->addPreDailyHeight(
				band->BandBase.Name.c_str(),
				((VtNDailyBand*)band)->BackLen, 
				4, 
				((VtNDailyBand*)band)->HighColor,
				((VtNDailyBand*)band)->LowColor,
				0xff0000ff);
			break;
		case VtBandType::Daily:
			_SourceChart->addDailyHeight(
				band->BandBase.Name.c_str(), 
				((VtDailyBand*)band)->HighColor,
				((VtDailyBand*)band)->LowColor,
				0xff0000ff);
			if (((VtDailyBand*)band)->ShowDailyLine == 1)
				_ShowDailyLine = true;
			else
				_ShowDailyLine = false;
			break;
		case VtBandType::Entire:
			_SourceChart->addEntireHeight(
				band->BandBase.Name.c_str(), 
				((VtEntireBand*)band)->HighColor,
				((VtEntireBand*)band)->LowColor,
				0xff0000ff);
			break;
		case VtBandType::ORB:
			_SourceChart->addOrbBand(
				band->BandBase.Name.c_str(),
				((VtOrbBand*)band)->BackLen,
				((VtOrbBand*)band)->Dividance,
				((VtOrbBand*)band)->HighColor,
				((VtOrbBand*)band)->LowColor,
				0xff0000ff);
			break;
		default:
			break;
		}
	}
}

void VtChartFrm::InitMaList()
{
	VtGlobal* global = VtGlobal::GetInstance();
	VtBaseIndex* index = new VtMaIndex(VtIndexType::SimpleMa);
	CString multi;
	multi.Format(_T("단순이동평균 %d"), 1);
	index->IndexBase.Name = multi;
	
	((VtMaIndex*)index)->Period = 5;
	((VtMaIndex*)index)->Color = global->ColorTable[0];

	IndexVector.push_back(index);

	index = new VtMaIndex(VtIndexType::SimpleMa);
	multi.Format(_T("단순이동평균 %d"), 2);
	index->IndexBase.Name = multi;

	((VtMaIndex*)index)->Period = 20;
	((VtMaIndex*)index)->Color = global->ColorTable[1];

	IndexVector.push_back(index);

	index = new VtMaIndex(VtIndexType::SimpleMa);
	multi.Format(_T("단순이동평균 %d"), 3);
	index->IndexBase.Name = multi;

	((VtMaIndex*)index)->Period = 60;
	((VtMaIndex*)index)->Color = global->ColorTable[2];

	IndexVector.push_back(index);

	index = new VtMaIndex(VtIndexType::SimpleMa);
	multi.Format(_T("단순이동평균 %d"), 4);
	index->IndexBase.Name = multi;

	((VtMaIndex*)index)->Period = 120;
	((VtMaIndex*)index)->Color = global->ColorTable[3];

	IndexVector.push_back(index);

	index = new VtMaIndex(VtIndexType::SimpleMa);
	multi.Format(_T("단순이동평균 %d"), 5);
	index->IndexBase.Name = multi;

	((VtMaIndex*)index)->Period = 200;
	((VtMaIndex*)index)->Color = global->ColorTable[4];

	IndexVector.push_back(index);
}

void VtChartFrm::DrawVrobo1Stress(DrawArea* drawArea)
{
	if (_Data && _TempSystem)
	{
		DoubleArray closeData = DoubleArray(_Data->Close.data() + startIndex, _NoOfPoints);
		double first = closeData[0];
		int oldx = 0, oldy = 0;
		oldx = _MainChart->getXCoor(0);
		oldy = _MainChart->getYCoor(first);
		for (int i = 1; i < closeData.len; i++)
		{
			double cur = closeData[i];
			int xpos = _MainChart->getXCoor(i);
			int ypos = _MainChart->getYCoor(cur);
			if (_TempSystem)
			{
				double ma5 = _TempSystem->GetMA(i, (double*)closeData.data, TA_MAType::TA_MAType_SMA, 5);
				double ma20 = _TempSystem->GetMA(i, (double*)closeData.data, TA_MAType::TA_MAType_SMA, 20);
				double ma60 = _TempSystem->GetMA(i, (double*)closeData.data, TA_MAType::TA_MAType_SMA, 60);
				int color = 0x888888;
				if (cur < ma20 && ma5 < ma20)
					color = 0x0000ff;
				if (cur > ma20 && ma5 > ma20)
					color = 0xff0000;

				drawArea->line(oldx, oldy, xpos, ypos, color, StressLineWidth);
			}
			oldx = xpos;
			oldy = ypos;
		}
	}
}

void VtChartFrm::DrawVrobo2Stress(DrawArea* drawArea)
{
	if (_Data && _TempSystem)
	{
		DoubleArray closeData = DoubleArray(_Data->Close.data() + startIndex, _NoOfPoints);
		double first = closeData[0];
		int oldx = 0, oldy = 0;
		oldx = _MainChart->getXCoor(0);
		oldy = _MainChart->getYCoor(first);
		
		double macd = 0, macdSignal = 0, macdHist = 0.0, close = 0.0;
		double slowK = 0, slowD = 0, ma20 = 0.0;
		DoubleArray highData = DoubleArray(_Data->High.data() + startIndex, _NoOfPoints);
		DoubleArray lowData = DoubleArray(_Data->Low.data() + startIndex, _NoOfPoints);
		double* closeArray = (double*)closeData.data;
		double* highArray = (double*)highData.data;
		double* lowArray = (double*)lowData.data;
		
		int ShortPeriod = 12;
		int LongPeriod = 26;
		int SignalPeriod = 6;
		int FastK = 12;
		int SlowK = 5;
		int SlowD = 5;
		int color = 0x888888;
		int flag = 0;
		for (int i = 1; i < closeData.len; i++)
		{
			double cur = closeData[i];
			int xpos = _MainChart->getXCoor(i);
			int ypos = _MainChart->getYCoor(cur);

			if (_TempSystem)
			{
				_TempSystem->GetMacd(i, closeArray, ShortPeriod, LongPeriod, SignalPeriod, macd, macdSignal, macdHist);
				_TempSystem->GetStochastic(i, highArray, lowArray, closeArray, FastK, SlowK, SlowD, slowK, slowD);
				ma20 = _TempSystem->GetMA(i, closeArray, TA_MAType::TA_MAType_SMA, 20);

				if ((flag == 0 || flag == 2) && macdSignal > macd && slowK > slowD)
				{
					color = 0xff0000;
					flag = 1;
				}
				if (flag == 1 && macdSignal < macd && slowK < slowD)
				{
					if (close < ma20)
					{
						color = 0x0000ff;
						flag = 2;
					}
				}

				if (flag == 2 && close > ma20)
				{
					color = 0x888888;
					flag = 0;
				}

				drawArea->line(oldx, oldy, xpos, ypos, color, StressLineWidth);
			}
			oldx = xpos;
			oldy = ypos;
		}
	}
}

void VtChartFrm::DrawStress(DrawArea* drawArea)
{
	for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
	{
		VtBaseStress* stress = *it;
		switch (stress->StressBase.Type)
		{
		case VtStressType::VRobo1:
			DrawVrobo1Stress(drawArea);
			break;
		case VtStressType::VRobo2:
			DrawVrobo2Stress(drawArea);
			break;
		default:
			break;
		}
	}
}

void VtChartFrm::DrawDailyLine(DrawArea* d)
{
	if (!_MainChart )
		return;


	PlotArea* plotArea = _MainChart->getPlotArea();
	int plotAreaTopY = plotArea->getTopY() + _MainChart->getAbsOffsetY();
	int height = plotArea->getHeight();

	DoubleArray datetime = DoubleArray(_Data->DateTime.data() + startIndex, _NoOfPoints);
	double first = datetime[0];
	int oldx = 0, oldy = 0;
	oldx = _MainChart->getXCoor(0);
	int color = 0xcc000000;
	double oldDate = Chart::getChartYMD(datetime[0]);
	for (int i = 1; i < datetime.len; i++)
	{
		double newDate = Chart::getChartYMD(datetime[i]);
		int xpos = _MainChart->getXCoor(i);
		if (oldDate != newDate)
		{
			d->line(xpos, plotAreaTopY, xpos, plotAreaTopY + height, color, 1);
		}

		oldDate = newDate;
	}
}

void VtChartFrm::DrawRefChart()
{
	int i = 0;
	VtLayerInfo layerInfo;
	VtGlobal* global = VtGlobal::GetInstance();
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
		chartData->LineColor(global->ColorTable[i + 1]);

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
					mainAxis->setColors(0xffdddddd, 0xff000000, 0xff000000);
					leftAxis->setTitle(chartData->SymbolCode().c_str())->setAlignment(Chart::TopRight);
				}
				else
				{
					leftAxis->setColors(0xffdddddd, 0xff000000, 0xff000000);
					mainAxis->setColors(0x00dddddd, 0x00000000, 0x00000000);
				}
			}


			if (chart->type == 0)
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
}

void VtChartFrm::RemoveChartWindow()
{
	VtChartWindowManager* wndMgr = VtChartWindowManager::GetInstance();
	std::string key = _T("");
	if (_Data)
	{
		key = _Data->Key();
		wndMgr->RemoveChartWindow(key, _ParentChartWnd);
	}

	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		key = chart->data->Key();
		wndMgr->RemoveChartWindow(key, _ParentChartWnd);
	}
}

bool VtChartFrm::FindCompareData(VtChartData* data)
{
	/*
	for (auto it = _CompareDataVector.begin(); it != _CompareDataVector.end(); ++it)
	{
		VtChartData* chartData = *it;
		if (chartData == data)
			return true;
	}
	*/
	return false;
}

VtChartData* VtChartFrm::FindCompareData(int index)
{
	if (index < 0 || index >= RefChartVector.size())
		return nullptr;

	return RefChartVector[index]->data;
}

void VtChartFrm::AdjustLastData()
{
	int maxCount = GetMaxDataCount();
	if (_Data)
	{
		if (_Data->GetDataCount() < maxCount)
		{
			_Data->CopyPrevData(maxCount - 1);
		}
	}

	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->data->GetDataCount() < maxCount)
		{
			chart->data->CopyPrevData(maxCount - 1);
		}
	}
}

int VtChartFrm::GetMaxDataCount()
{
	if (!_Data)
		return 0;
	int maxCount = _Data->GetDataCount();

	CString cnt;
	cnt.Format(_T("chartdataCount = %d"), maxCount);
	//TRACE(cnt);

	/*
	for (auto it = _CompareDataVector.begin(); it != _CompareDataVector.end(); ++it)
	{
		VtChartData* data = *it;
		if (data->GetDataCount() > maxCount)
			maxCount = data->GetDataCount();

		//cnt.Format(_T("chartdataCount = %d"), data->GetDataCount());
		//TRACE(cnt);
	}
	*/

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

void VtChartFrm::GetZoomRange(CChartViewer * a_pChartViewer, int& start, int& end)
{
	start = 0;
	end = 0;
	if (_Data == nullptr || a_pChartViewer == nullptr)
		return;
	/*
	start = (int)(a_pChartViewer->getViewPortLeft() * _Data->GetDataCount());
	end = (int)((a_pChartViewer->getViewPortLeft() + a_pChartViewer->getViewPortWidth()) * _Data->GetDataCount());

	DoubleArray timeStamps = DoubleArray(_Data->DateTime, _Data->GetDataCount());

	double viewPortStartDate = timeStamps[start];//a_pChartViewer->getValueAtViewPort("x", a_pChartViewer->getViewPortLeft());
	double viewPortEndDate = a_pChartViewer->getValueAtViewPort("x", a_pChartViewer->getViewPortLeft() +
		a_pChartViewer->getViewPortWidth());

	// Get the array indexes that corresponds to the visible start and end dates
	int startIndex = (int)floor(Chart::bSearch(timeStamps, viewPortStartDate));
	int endIndex = (int)ceil(Chart::bSearch(timeStamps, viewPortEndDate));

	if (start >= _Data->GetDataCount())
		start = _Data->GetDataCount() - 1;
	if (end >= _Data->GetDataCount())
		end = _Data->GetDataCount() - 1;
	*/

	AdjustLastData();

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

void VtChartFrm::SaveToXml(pugi::xml_node& node)
{
	CString msg;
	msg.Format(_T("%d"), (int)MainChartType());
	std::string value = (LPCTSTR)msg;

	pugi::xml_node mainchart_type = node.append_child("mainchart_type");
	mainchart_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node chart_data = node.append_child("chart_data");
	if (_Data)
		_Data->SaveToXml(chart_data);

	pugi::xml_node index_list = node.append_child("index_list");
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{	
		VtBaseIndex* baseIndex = *it;
		pugi::xml_node chart_index = index_list.append_child("chart_index");

		baseIndex->SaveToXml(chart_index);
	}

	pugi::xml_node stress_list = node.append_child("stress_list");
	for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
	{
		VtBaseStress* baseStress = *it;
		pugi::xml_node chart_stress = stress_list.append_child("chart_stress");

		baseStress->SaveToXml(chart_stress);
	}


	pugi::xml_node band_list = node.append_child("band_list");
	for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
	{
		VtBaseBand* baseBand = *it;
		pugi::xml_node chart_band = band_list.append_child("chart_band");

		baseBand->SaveToXml(chart_band);
	}

	pugi::xml_node refchart_list = node.append_child("refchart_list");
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* refChart = *it;
		pugi::xml_node ref_chart = refchart_list.append_child("ref_chart");
		refChart->SaveToXml(ref_chart);
	}

	pugi::xml_node chart_system = node.append_child("chart_system");
	if (_System)
		_System->SaveToXml(chart_system);


}

void VtChartFrm::LoadFromXml(pugi::xml_node& node)
{
	std::string index = node.child_value("index");
	std::string mainchart_type = node.child_value("mainchart_type");
	
	_Index = std::stoi(index);
	_MainChartType = (VtMainChartType)std::stoi(mainchart_type);

	pugi::xml_node chart_data = node.child("chart_data");
	if (chart_data)
	{
		VtChartData* tempData = new VtChartData();
		std::string dataKey = tempData->LoadFromXml(chart_data);
		VtChartWindowManager* chartWndMgr = VtChartWindowManager::GetInstance();
		chartWndMgr->AddChartWindow(dataKey, _ParentChartWnd);
		delete tempData;
	}

	pugi::xml_node index_list = node.child("index_list");

	for (pugi::xml_node chart_index = index_list.child("chart_index"); chart_index; chart_index = chart_index.next_sibling("chart_index"))
	{
		std::string index_type = chart_index.child_value("index_type");
		VtBaseIndex* baseIndex = nullptr;
		VtIndexType type = (VtIndexType)std::stoi(index_type);
		switch (type)
		{
		case VtIndexType::SimpleMa:
			baseIndex = new VtMaIndex(VtIndexType::SimpleMa);
			break;
		case VtIndexType::DataVolume:
			baseIndex = new VtVolumeIndex(VtIndexType::DataVolume);
			break;
		case VtIndexType::BollingerBand:
			baseIndex = new VtBollingerBandIndex(VtIndexType::BollingerBand);
			break;
		case VtIndexType::PriceChannel:
			baseIndex = new VtPriceChannelIndex(VtIndexType::PriceChannel);
			break;
		case VtIndexType::Macd:
			baseIndex = new VtMacdIndex(VtIndexType::Macd);
			break;
		case VtIndexType::Rsi:
			baseIndex = new VtRsiIndex(VtIndexType::Rsi);
			break;
		case VtIndexType::Stochastic:
			baseIndex = new VtStochasticIndex(VtIndexType::Stochastic);
			break;
		case VtIndexType::Trix:
			baseIndex = new VtTrixIndex(VtIndexType::Trix);
			break;
		case VtIndexType::Atr:
			baseIndex = new VtAtrIndex(VtIndexType::Atr);
			break;
		default:
			break;
		}

		if (baseIndex)
		{
			baseIndex->LoadFromXml(chart_index);
			IndexVector.push_back(baseIndex);
		}
	}

	pugi::xml_node stress_list = node.child("stress_list");

	for (pugi::xml_node chart_stress = stress_list.child("chart_stress"); chart_stress; chart_stress = chart_stress.next_sibling("chart_stress"))
	{
		std::string stress_type = chart_stress.child_value("stress_type");
		VtBaseStress* baseStress = nullptr;
		VtStressType type = (VtStressType)std::stoi(stress_type);
		switch (type)
		{
		case VtStressType::VRobo1:
			baseStress = new VtRobo1Stress(type);
			break;
		case VtStressType::VRobo2:
			baseStress = new VtSimpleMaStress(type);
			break;
		}

		if (baseStress)
		{
			baseStress->LoadFromXml(chart_stress);
			StressVector.push_back(baseStress);
		}
	}

	pugi::xml_node band_list = node.child("band_list");

	for (pugi::xml_node chart_band = band_list.child("chart_band"); chart_band; chart_band = chart_band.next_sibling("chart_band"))
	{
		std::string band_type = chart_band.child_value("band_type");
		VtBaseBand* baseBand = nullptr;
		VtBandType type = (VtBandType)std::stoi(band_type);
		switch (type)
		{
		case VtBandType::Ndaily:
			baseBand = new VtNDailyBand(type);
			break;
		case VtBandType::Daily:
			baseBand = new VtDailyBand(type);
			break;
		case VtBandType::Entire:
			baseBand = new VtEntireBand(type);
			break;
		case VtBandType::ORB:
			baseBand = new VtOrbBand(type);
			break;
		default:
			break;

		}

		if (baseBand)
		{
			baseBand->LoadFromXml(chart_band);
			BandVector.push_back(baseBand);
		}
	}

	pugi::xml_node refchart_list = node.child("refchart_list");

	for (pugi::xml_node ref_chart = refchart_list.child("ref_chart"); ref_chart; ref_chart = ref_chart.next_sibling("ref_chart"))
	{
		VtReferenceChart* refChart = new VtReferenceChart();
		refChart->LoadFromXml(ref_chart);
		RefChartVector.push_back(refChart);
	}

	pugi::xml_node chart_system = node.child("chart_system");
	if (chart_system)
	{
		VtSystemManager* sysFactory = VtSystemManager::GetInstance();
		const char* system_type = chart_system.child_value("system_type");
		_System = sysFactory->CreateSystem((VtSystemType)std::stoi(system_type));
		_CurSystemType = _System->SystemType();
		_System->LoadFromXml(chart_system);
	}
}

void VtChartFrm::ReloadSystem(VtSystem* sys)
{
	if (!sys || !_Data)
		return;
	sys->Running(false);
	sys->ReloadSystem(0, _Data->GetDataCount() - 1);
	sys->Running(true);
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::OnDeleteKey()
{
	if (_shapesCollection.SelectedElement)
	{
		_shapesCollection.DeleteSelectedObject();
		DrawChart(_ChartViewer, 0);
		return;
	}
	if (_SelectedSystem)
	{
		if (_System)
		{
			delete _System;
			_System = nullptr;
			DrawChart(_ChartViewer, 0);
		}
		return;
	}

	if (_SelectedDataSetName.length() > 0)
	{
		int pos = 0;
		if (_SelectedDataSetName.find(_T("Reference")) != std::string::npos)
		{
			pos = _SelectedDataSetName.find(_T(">"));
			if (pos != std::string::npos)
			{
				std::string index = _SelectedDataSetName.substr(pos + 1, _SelectedDataSetName.length() - pos - 1);
				int num = std::stoi(index);
				if (RefChartVector.size() > 0)
				{
					VtReferenceChart* chart = RefChartVector[num];
					RemoveCompareData(chart->data);
					DrawChart(_ChartViewer, 0);
				}
				_SelectedDataSetName = _T("");
			}
		}
		else
		{
			//if (_SelectedDataSetName.compare(_T("Volume")) == 0)
			//	_SelectedDataSetName.replace(_T("Volume"), _T("거래량"));
			VtBaseIndex* index = FindIndex(_SelectedDataSetName);
			if (index)
			{
				for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
				{
					VtBaseIndex* curIndex = *it;
					if (curIndex == index)
					{
						IndexVector.erase(it);
						break;
					}
				}

				delete index;
				_SelectedDataSetName = _T("");

				DrawChart(_ChartViewer, 0);
			}

			VtBaseBand* band = FindBand(_SelectedDataSetName);
			if (band)
			{
				for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
				{
					VtBaseBand* curBand = *it;
					if (curBand == band)
					{
						BandVector.erase(it);
						break;
					}
				}

				delete band;
				_SelectedDataSetName = _T("");
				DrawChart(_ChartViewer, 0);
			}

			VtBaseStress* stress = FindStress(_SelectedDataSetName);
			if (stress)
			{
				for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
				{
					VtBaseStress* curStress = *it;
					if (curStress == stress)
					{
						StressVector.erase(it);
						break;
					}
				}

				delete stress;
				_SelectedDataSetName = _T("");
				DrawChart(_ChartViewer, 0);
			}
		}
		return;
	}
}

void VtChartFrm::OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose /*= 0*/)
{
	_RealTimeClose = krwClose;
	_RealTimeCloseUSD = usdClose;
	if (_Data)
	{
		int lastIndex = _Data->GetDataCount() - 1;
		_Data->Close[lastIndex] = krwClose;
		_Data->RealTimeClose(krwClose);
	}
}


void VtChartFrm::DrawCurrentValue(double close, double usd, DrawArea* drawArea)
{
	PlotArea* plotArea = _MainChart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + _MainChart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + _MainChart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	int valuePos = _MainChart->getYCoor(close);
	int rectTop = valuePos - 8;
	int rectLeft = plotAreaLeftX + width + _YAxisGap;
	int rectBottom = rectTop + 16;

	int xarr[3], yarr[3];
	xarr[0] = plotAreaLeftX + width + _YAxisGap - 14;
	xarr[1] = plotAreaLeftX + width + _YAxisGap;
	xarr[2] = plotAreaLeftX + width + _YAxisGap;

	yarr[0] = valuePos;
	yarr[1] = rectTop;
	yarr[2] = rectBottom;

	drawArea->polygon(IntArray(xarr, 3), IntArray(yarr, 3), 0xff0000, 0xff0000);
	drawArea->rect(rectLeft, rectTop, rectLeft + 60, rectBottom, 0xff0000, 0xff0000);
	CString value;
	value.Format(_T("%.0f"), close);
	TTFText* t = drawArea->text(value, _T("굴림"), 9);
	t->draw(plotAreaLeftX + width + _YAxisGap, valuePos - 6, 0xffffff);

}

void VtChartFrm::DrawCurrentValue(VtLayerInfo&& layerInfo, DrawArea* drawArea)
{
	PlotArea* plotArea = layerInfo.chart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + layerInfo.chart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + layerInfo.chart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();

	int valuePos = plotAreaTopY - 35  + layerInfo.chart->getYCoor(layerInfo.data->RealTimeClose(), layerInfo.axis);
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
	drawArea->rect(rectLeft, rectTop, rectLeft + 65, rectBottom, layerInfo.color, layerInfo.color);
	std::string strValue;
	if (layerInfo.data->Currency().compare(_T("KRW")) == 0)
		strValue = string_cast<double>(layerInfo.data->RealTimeClose(), 0, convert::thou_sep);
	else
		strValue = string_cast<double>(layerInfo.data->RealTimeClose(), 2, convert::thou_sep);
	TTFText* t = drawArea->text(strValue.c_str(), _T("굴림"), 10);
	if (layerInfo.color != 0x00ffff00)
		t->draw(rectLeft, valuePos - 7, 0xffffff);
	else
		t->draw(rectLeft, valuePos - 7, 0x000000);
}

void VtChartFrm::DrawRealtimeValues(DrawArea* d)
{
	if (!d)
		return;

	for (auto it = _LayerList.rbegin(); it != _LayerList.rend(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		DrawCurrentValue(std::move(layerInfo), d);
	}
}

void VtChartFrm::ResetChart()
{
	_Init = false;
}

void VtChartFrm::DrawTitleValue(DrawArea* drawArea)
{
	PlotArea* plotArea = _MainChart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + _MainChart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + _MainChart->getAbsOffsetY();
	int height = plotArea->getHeight();
	int width = plotArea->getWidth();
	VtChartData* selData = _Data;
	if (_SelectedData)
	{
		selData = _SelectedData;
	}
	

	std::string text = _T("<*font = times.ttf, size = 18, color = FF0000*>");
	std::string strValue;
	if (selData->Currency().compare(_T("KRW")) == 0)
		strValue = string_cast<double>(selData->RealTimeClose(), 0, convert::thou_sep);
	else
		strValue = string_cast<double>(selData->RealTimeClose(), 2, convert::thou_sep);
	
	text.append(strValue);
	text.append(_T("<*font = gulim.ttc, size = 10, color = ff0000*>"));
	text.append(selData->Currency());
	text.append(_T("<*font = gulim.ttc, size = 10, color = 000000*>"));
	text.append(TCHARtoUTF8(_T(" 거래소:")));
	text.append(selData->Exchange());
	text.append(TCHARtoUTF8(_T(",화폐이름:")));
	//VcExchangeManager* excMgr = VcExchangeManager::GetInstance();
	//std::string fullCode = selData->Exchange();
	//fullCode.append(selData->SymbolCode());
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
	switch (selData->ChartType())
	{
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
}

void VtChartFrm::SetSystem(VtSystemType sysType)
{
	if (!_Data || sysType == VtSystemType::None)
		return;

	VtSystemManager* sysFactory = VtSystemManager::GetInstance();
	if (_System)
	{
		delete _System;
		_System = nullptr;
	}
	switch (sysType)
	{
	case VtSystemType::MaSystem:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("high"), _Data->High.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
		break;
	case VtSystemType::StoSystem:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("high"), _Data->Close.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
		break;
	case VtSystemType::MacdSystem:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
		break;
	case VtSystemType::ChannelCross:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->AddDataSource(_T("high"), _Data->High.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("timestamp"), _Data->DateTime.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
	break;
	case VtSystemType::ChannelJump:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->AddDataSource(_T("high"), _Data->High.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("timestamp"), _Data->DateTime.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
	break;

	case VtSystemType::Osillator:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->AddDataSource(_T("high"), _Data->High.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("timestamp"), _Data->DateTime.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
	break;

	case VtSystemType::DailyHeight:
	{
		_System = sysFactory->CreateSystem(sysType);
		_System->DateTime(_Data->DateTime.data());
		_System->Time(_Data->Time.data());
		_System->Date(_Data->Date.data());
		_System->AddDataSource(_T("close"), _Data->Close.data());
		_System->AddDataSource(_T("high"), _Data->High.data());
		_System->AddDataSource(_T("low"), _Data->Low.data());
		_System->AddDataSource(_T("open"), _Data->Low.data());
		_System->AddDataSource(_T("timestamp"), _Data->DateTime.data());
		_System->CreateSignal(0, _Data->GetDataCount() - 1);
	}
	break;

	default:
		break;
	}
	
}

void VtChartFrm::LoadSystem()
{
	SetSystem(_CurSystemType);
}

void VtChartFrm::UpdateSystem(VtChartData* data, int count)
{
	if (_Data != data || count <= 0 || !_System)
		return;

	int start = data->DataCount() - count;
	int end = data->DataCount() - 1;
	VtPositionType pos = _System->UpdateSignal(start, end);
	VtPositionType lastSig = _System->OldSignal();
	if (pos != VtPositionType::None && pos != lastSig)
		PlaySignalSound(pos);
}

void VtChartFrm::UpdateIndex(VtBaseIndex* index)
{
	if (!index)
		return;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DrawChart(CChartViewer * a_pChartViewer, int mode)
{
	//PlaySignalSound(VtPositionType::Buy);

	if (nullptr == a_pChartViewer || _Data == nullptr || !_Data->Filled() || _Data->GetDataCount() == 0)
		return;

	_LayerList.clear();

	//int startIndex = 0;
	//int endIndex = 0;
	GetZoomRange(a_pChartViewer, startIndex, endIndex);
	int noOfPoints = endIndex - startIndex + 1;
	_NoOfPoints = noOfPoints;

	CString msg;
	msg.Format(_T("width = %d, height = %d, point count = %d\n"), _Width, _Height, noOfPoints);
	//TRACE(msg);
	
	DoubleArray timeStamps = DoubleArray(_Data->DateTime.data() + startIndex, noOfPoints);
	DoubleArray highData = DoubleArray(_Data->High.data() + startIndex, noOfPoints);
	DoubleArray lowData = DoubleArray(_Data->Low.data() + startIndex, noOfPoints);
	DoubleArray openData = DoubleArray(_Data->Open.data() + startIndex, noOfPoints);
	DoubleArray closeData = DoubleArray(_Data->Close.data() + startIndex, noOfPoints);
	DoubleArray volData = DoubleArray(_Data->Volume.data() + startIndex, noOfPoints);

	ArrayMath minArray(lowData);
	ArrayMath maxArray(highData);
	int minIndex = minArray.minIndex();
	int maxIndex = maxArray.maxIndex();
	
	_SourceChart = new FinanceChart(_Width);
	//_SourceChart->setMargins(0, 35, 0, 35);
	// Add a title to the chart
	TCHARtoUTF8 convert(ChartTile().c_str());
	//_SourceChart->addTitle(convert, _T("gulim.ttc"), 12);

	// Disable default legend box, as we are using dynamic legend
	_SourceChart->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);

	// Set the data into the finance chart object
	_SourceChart->setData(timeStamps, highData, lowData, openData, closeData, volData, _XtraPoints);

	_Height = CalcChartHeight();

	// Add the main chart with 240 pixels in height
	_MainChart = _SourceChart->addMainChart(_Height);

	_MainChart->setPlotArea(30, 35, _Width - 120, _Height, 0xffffff, 0xffffff, 0xffffff, _MainChart->dashLineColor(0xdddddd, Chart::DotLine), -1);
	_MainChart->yAxis()->setOffset(_YAxisGap, 0);
	//_MainChart->yAxis()->setColors(0xdddddd);
	_MainChart->xAxis()->setColors(0xdddddd);
	_MainChart->setNumberFormat(',');

	if (_ChartOrderManager)
		_ChartOrderManager->Chart(_MainChart);

	VtGlobal* global = VtGlobal::GetInstance();
	VtLayerInfo layerInfo;
	if (_MainChartType == VtMainChartType::ClosePrice)
	{
		_MainLayer = _SourceChart->addCloseLine(global->ColorTable[0]);
		_MainLayer->setLineWidth(2);
		layerInfo.layer = _MainLayer;
	}
	else if (_MainChartType == VtMainChartType::TypicalPrice)
	{
		_MainLayer = _SourceChart->addTypicalPrice(global->ColorTable[0]);
		_MainLayer->setLineWidth(2);
		layerInfo.layer = _MainLayer;
	}
	else if (_MainChartType == VtMainChartType::WeightedClose)
	{
		_MainLayer = _SourceChart->addWeightedClose(global->ColorTable[0]);
		_MainLayer->setLineWidth(2);
		layerInfo.layer = _MainLayer;
	}
	else if (_MainChartType == VtMainChartType::MedianPrice)
	{
		_MainLayer = _SourceChart->addMedianPrice(global->ColorTable[0]);
		_MainLayer->setLineWidth(2);
		layerInfo.layer = _MainLayer;
	}
	else if (_MainChartType == VtMainChartType::OHLC)
	{
		layerInfo.layer = _SourceChart->addHLOC(0x8000, 0x800000);
	}
	else
	{
		// Add candlestick symbols to the main chart, using green/red for up/down days
		CandleStickLayer* candle = _SourceChart->addCandleStick(0x00ff00, 0xff0000);
		candle->setColors(0xff0000, 0xff0000, 0x0000ff, 0x0000ff);
		layerInfo.layer = candle;
	}
	layerInfo.selected = false;
	layerInfo.chart = _MainChart;
	layerInfo.axis = _MainChart->yAxis();
	layerInfo.data = _Data;
	layerInfo.color = global->ColorTable[0];
	_LayerList.push_back(layerInfo);

	//layerInfo.axis->setColors(0x00dddddd, 0x00000000, 0x00000000);
	
	DrawIndex(_LayerList);
	DrawBand();
	DrawRefChart();

	
	DrawArea* d = _MainChart->makeChart();


	if (_shapesCollection.CheckChangeDimension(_ChartViewer, _Data->GetDataCount(), _MainChart))
		_shapesCollection.ValueToPixel(0, startIndex, _MainChart);

	if (mode == 0)
		DrawObjects(0, 0, d, _MainChart, _offset.x, _offset.y);


	DrawTitleValue(d);

	DrawStress(d);

	DrawSignal(timeStamps, highData, lowData, d);

	trackFinance(_SourceChart, (int)_MouseClickPoint.x);

	//DrawRealtimeValues(d);

	
	BaseChart* chart = a_pChartViewer->getChart();
	delete chart;
	chart = nullptr;
	a_pChartViewer->setChart(_SourceChart);
	

}

inline std::tuple<int, int, int, int, int, int> get_datetime(std::string a_format)
{
	std::string year, mon, day, hour, min, sec;
	try
	{

		year.assign(a_format.c_str(), 4);
		mon.assign(a_format.c_str() + 4, 2);
		day.assign(a_format.c_str() + 6, 2);
		hour.assign(a_format.c_str() + 8, 2);
		min.assign(a_format.c_str() + 11, 2);
		sec.assign(a_format.c_str() + 14, 2);
	}
	catch (std::exception& e)
	{
		AfxMessageBox(e.what());
	}
	return std::make_tuple(std::stoi(year), std::stoi(mon), std::stoi(day), std::stoi(hour), std::stoi(min), std::stoi(sec));
}


void VtChartFrm::SetChartData(VtChartData* a_ChartData)
{
	if (a_ChartData == nullptr)
		return;
	_Data = a_ChartData;
	_NoOfPoints = _Data->GetDataCount();
	_Data->AddChartWindow(_ParentChartWnd);
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	chartDataMgr->AddChartDataRequest(10, _Data);
}

void VtChartFrm::OnRButtonDown(UINT nFlags, CPoint point)
{

}

void VtChartFrm::OnLButtonDown(UINT nFlags, CPoint point)
{
	HitTest(point);

	if (!_Init)
		return;

	_MouseClickPoint.x = point.x;
	_MouseClickPoint.y = point.y;
	VtPoint mousePoint;
	MouseToChart(point, mousePoint);
	mousePoint.x -= _offset.x;
	mousePoint.y -= _offset.y;
	
	_start.x = mousePoint.x;
	_start.y = mousePoint.y;

	_temp.x = _start.x;
	_temp.y = _start.y;

	_isMouseLButtonDown = true;

	switch (_editOpt)
	{
	case EditOption::Select:

		// 객체를 먼저 찾아 본다.
		if (_editStatus != EditStatus::Redim)
		{
			// 현재 마우스를 클릭했을 때 조사를 한다.
			// 마우스 클릭으로 객체가 선택되면 선택된 객체를 설정하고 
			// 객체에 대한 트래커를 생성한다.
			_shapesCollection.Click(mousePoint.x, mousePoint.y);
		}

		// 객체가 선택된 경우 편집 모드가 된다.
		if (_shapesCollection.SelectedElement != nullptr)
		{
			//_shapesCollection.SelectedElement->ValueToPixel(_MainChart);
			// 선택된 객체에 대하여 트래커가 만들어진 경우
			if (_shapesCollection.Tracker != nullptr)
			{
				//_shapesCollection.ValueToPixel(_MainChart);
				//_shapesCollection.Tracker->ValueToPixel(_MainChart);
				//Console.WriteLine("mousedown");
				_shapesCollection.Tracker->EndMoveRedim();
				// 객체가 수정이나 이동 모드에 있는지 알아 본다.
				// 객체가 선택된 경우 반드시 수정이나 이동모드 중에 하나는 되어야 한다.
				// 선택 사각형 위에서 어떤 지점을 선택했는지 조사한다. 
				// 객체에 따라서 점이동, 이동, 크기 조절이 있다.
				// 하나의 객체에 여러개의 핸들 목록이 있다. 그 목록에서 그 점 위에 있는지 아니면 그 객체 영역에 있는지 조사한다.
				// 핸들이 점이고 점이 선택 범위 안에 있으면 점을 이동하거나 모양을 조절할 수 있게 된다.
				_redimStatus = _shapesCollection.Tracker->IsOver(mousePoint.x, mousePoint.y);
			}
			else
				return;

			SetEditStatus(EditStatus::Redim);
			_RedimStarted = true;
		}
		else // 객체가 선택되지 않으면 선택 모드가 된다.
		{
			// 객체가 없다면 기존에 선택된 모든 객체의 선택을 해제해 준다.
			_shapesCollection.DeSelect();
			// pressing mouse in an empty space means: starting a select rect
			SetEditStatus(EditStatus::SelectRect);
		}

		break;
	case EditOption::MultiLine:
		// 그리기 상태가 이미 그리기 시작한 상태가 아닐 때만 새로운 점을 시작한다.
		if (_multiLineStarted == false)
		{
			_penPoints.clear();
			_multiLineStarted = true;
			SetEditStatus(EditStatus::DrawRect);

			//VtPoint pt(_temp.x, _temp.y);
			//_penPoints.push_back(pt);
		}
		break;
	default:
		SetEditStatus(EditStatus::DrawRect);
		break;
	}
	if (!_multiLineStarted)
	{
		if (_ChartViewer != nullptr)
			DrawChart(_ChartViewer, 0);
	}
	else
	{
		/*
		if (_SourceChart != nullptr && _isMouseLButtonDown)
		{
			DrawArea *d = _SourceChart->initDynamicLayer();
			if (_editStatus == EditStatus::DrawRect)
				DrawCreatingObject(d);
		}
		*/
		if (_ChartViewer != nullptr)
			DrawChart(_ChartViewer, 0);
	}
	CMainFrame* mainFrm = (CMainFrame*)AfxGetMainWnd();
	//mainFrm->SendMessage(WM_CHART_ACTIVATED, (WPARAM)this, (LPARAM)_Data);
}

void VtChartFrm::OnMouseMove(UINT nFlags, CPoint point)
{
	VtPoint mousePoint;
	MouseToChart(point, mousePoint);
	mousePoint.x -= _offset.x;
	mousePoint.y -= _offset.y;

	_temp.x = mousePoint.x;
	_temp.y = mousePoint.y;
	//Console.WriteLine("before : tempX =  " + _temp.X.ToString());

	if (_editStatus == EditStatus::Redim)
	{
		int tmpX = _temp.x;
		int tmpY = _temp.y;
		int tmpstartX = _start.x;
		int tmpstartY = _start.y;
		//Console.WriteLine("Mouse move lbdown");
		switch (_redimStatus)
		{
			//Poly's point
		case RedimStatus::MovePoint:
			// Move selected
			if (_shapesCollection.SelectedElement != nullptr && _shapesCollection.Tracker != nullptr)
			{
				_shapesCollection.MovePoint(tmpstartX - tmpX, tmpstartY - tmpY);
			}
			break;
		case RedimStatus::MoveSelected:
			// Move selected
			if (_shapesCollection.SelectedElement != nullptr && _shapesCollection.Tracker != nullptr)
			{
				_shapesCollection.Move(tmpstartX - tmpX, tmpstartY - tmpY);
			}
			break;
		default:
			// redim selected
			if (_shapesCollection.SelectedElement != nullptr && _shapesCollection.Tracker != nullptr)
			{
				_shapesCollection.Redim(tmpX - tmpstartX, tmpY - tmpstartY, _redimStatus);
			}
			break;
		}

		//Console.WriteLine("Mouse move");

		//if (_SelSymbol == nullptr) return;
		//_shapesCollection.PixelToValue(_IDStart, _ZoomStartIndex, d, _MainChart, CanvasOriginX, CanvasOriginY, _SelSymbol);
	}
	else
	{
		if (_editOpt == EditOption::Select)
		{
			if (_shapesCollection.Tracker != nullptr)
			{
				RedimStatus redim = _shapesCollection.Tracker->IsOver(mousePoint.x, mousePoint.y);
				
				switch (redim)
				{
				case RedimStatus::MovePoint:
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
					break;
				case RedimStatus::MoveSelected:
					::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
					break;
				case RedimStatus::NW:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
					break;
				case RedimStatus::N:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
					break;
				case RedimStatus::NE:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
					break;
				case RedimStatus::E:
					::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
					break;
				case RedimStatus::SE:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
					break;
				case RedimStatus::S:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
					break;
				case RedimStatus::SW:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
					break;
				case RedimStatus::W:
					::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
					break;
				default:
					::SetCursor(::LoadCursor(NULL, IDC_ARROW));
					_redimStatus = RedimStatus::None;
					break;
				}
			}
			else
			{
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));
				_redimStatus = RedimStatus::None;
			}
		}
		else
		{
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			_redimStatus = RedimStatus::None;
		}
	}

	CString msg;
	msg.Format(_T("ChartFrame::Mouse x = %d, y = %d\n"), mousePoint.x, mousePoint.y);
	//TRACE(msg);
	/*
	if (_SourceChart != nullptr && _isMouseLButtonDown)
	{
		DrawArea *d = _SourceChart->initDynamicLayer();
		if (_editStatus == EditStatus::DrawRect)
			DrawCreatingObject(d);
		else if (_editStatus == EditStatus::SelectRect)
			DrawSelection(d);
		else if (_editStatus == EditStatus::Redim)
		{
			if (_RedimStarted == true)
			{
				DrawChart(_ChartViewer, 1);
				_RedimStarted = false;
			}
			DrawChart(_ChartViewer, 0);
		}
	}
	*/
}


void VtChartFrm::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (_SelectedSystem)
	{
		OnSystemProperty();
		return;
	}

	if (_SelectedDataSetName.length() > 0)
	{
		ShowProperty();
		return;
	}

	int sx = _start.x;
	int sy = _start.y;
	//CalcPixelSize();
	int tmpX = point.x - _offset.x;
	int tmpY = point.y - _offset.y;

	//VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);

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

	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}

void VtChartFrm::PixelToValue(VtPoint& location0, VtPoint& location1, bool byValue)
{
	
}

void VtChartFrm::PixelToValue()
{
	int startIndex = 0;
	int endIndex = 0;
	GetZoomRange(_ChartViewer, startIndex, endIndex);

	_shapesCollection.PixelToValue(_StartID, startIndex, _MainChart);
}

void VtChartFrm::initChartViewer(CChartViewer *viewer)
{
	if (_Init) return;

	if (_Data == nullptr)
		return;

	//InitMaList();

	//viewer->ChartFrm(this);

	auto time = Data()->DateTime;
	int dataSize = Data()->GetDataCount();
	// Set the full x range to be the duration of the data
	viewer->setFullRange("x", time[0], time[dataSize - 1]);

	// Initialize the view port to show the latest 20% of the time range
	viewer->setViewPortWidth(0.06);
	viewer->setViewPortLeft(1 - viewer->getViewPortWidth());
	viewer->setZoomInWidthLimit(0.05);

	// Set the maximum zoom to 10 points
	viewer->setZoomInWidthLimit((MinPoints() + XtraPoints()) / dataSize);

	// Initially set the mouse to drag to scroll mode.
	viewer->setMouseUsage(Chart::MouseUsageDefault);

	_Height = CalcChartHeight();

	_Init = true;
}

void VtChartFrm::CreateLineWithOrder(VtOrder* order)
{
	if (!order || !_MainChart)
		return;
	int yPos = _MainChart->getYCoor(order->orderPrice);
	VtElement* elem = _shapesCollection.AddHLineByOrder(_MainChart, yPos, _creationPenColor, _creationPenWidth);
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbolMaster* symMstr = symMgr->FindSymbolMasterByShortCode(order->shortCode);
	if (symMstr)
	{
		elem->SymbolMaster(symMstr);
		elem->Order(order);
	}
}

void VtChartFrm::PutOrder(POINT& pt, VtOrderCmd cmd, int qty)
{
	if (!_MainChart || !_Data)
		return;

	double price = _MainChart->getYValue(pt.y);
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbolMaster* symMstr = symMgr->FindSymbolMasterByFullCode(_Data->SymbolCode());
	if (_ChartOrderManager)
		_ChartOrderManager->PutOrder(Id(), cmd, qty, price, symMstr);
	_editOpt = EditOption::None;
}

void VtChartFrm::AddPreOrder(VtOrder* order)
{
	order->state = VtOrderState::Reserved;
	if (_ChartOrderManager)
		_ChartOrderManager->AddPreOrder(order);
	CreateLineWithOrder(order);
	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::OnAccepted(VtOrder* order)
{
	if (!order) return;
	if (order->state != VtOrderState::Accepted)
		return;

	if (_ChartOrderManager)
		_ChartOrderManager->OrderAccepted(order);
	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::OnFilled(VtOrder* order)
{
	if (!order) return;
	if (order->state != VtOrderState::Filled)
		return;

	if (_ChartOrderManager)
		_ChartOrderManager->OrderFilled(order);

	VtElement* ele = _shapesCollection.FindObjectByOrder(order);
	if (ele)
	{
		_shapesCollection.RemoveElement(ele);
	}

	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::OnConfirmModify(VtOrder* order)
{
	if (!order) return;
	if (order->state != VtOrderState::ConfirmModify)
		return;

	// 객체를 없애고 새로운 객체를 만드는 것이 나을듯.
	VtElement* ele = _shapesCollection.FindObjectByOrderNo(order->oriOrderNo);
	if (ele)
	{
		ele->Order(order);
	}

	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::OnConfirmCancel(VtOrder* order)
{
	if (!order) return;
	if (order->state != VtOrderState::ConfirmCancel)
		return;

	VtElement* ele = _shapesCollection.FindObjectByOrderNo(order->oriOrderNo);
	if (ele)
	{
		_shapesCollection.RemoveElement(ele);
	}

	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::OrderArrived(VtOrder* order)
{
	if (!order) return;
	if (order->state != VtOrderState::Accepted)
		return;
	if (_ChartOrderManager)
		_ChartOrderManager->OrderArrived(order);
	_ChartViewer->updateViewPort(true, true);
}

void VtChartFrm::ChangeOrder(int yPos, VtElement* selObj)
{
	if (!selObj)
		return;
	VtOrder* order = selObj->Order();
	if (order && (order->state == VtOrderState::Accepted || order->state == VtOrderState::ConfirmModify))
	{
		double newValue = _MainChart->getYValue(yPos);
		if (_ChartOrderManager)
			_ChartOrderManager->ChangeOrder(newValue, selObj->Order());
	}
}

void VtChartFrm::InitChartIndex()
{
	ChartIndexMap[VtChartIndex::MA] = false;
	ChartIndexMap[VtChartIndex::BolingerBand] = false;
	ChartIndexMap[VtChartIndex::Stocastic] = false;
	ChartIndexMap[VtChartIndex::MACD] = false;
	ChartIndexMap[VtChartIndex::RSI] = false;
	ChartIndexMap[VtChartIndex::TRIX] = false;
	ChartIndexMap[VtChartIndex::Parabolic] = false;
	ChartIndexMap[VtChartIndex::Volume] = false;
	ChartIndexMap[VtChartIndex::PriceChannel] = true;
}

void VtChartFrm::ShowChartIndex(int index)
{
	if (!_ParentChartWnd || !_ChartViewer)
		return;

	//bool show = ChartIndexMap[(VtChartIndex)index];
	//ChartIndexMap[(VtChartIndex)index] = !show;

	_Height = CalcChartHeight();
	
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DeleteAllChartIndex()
{
	int i = 0;
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		delete *it;
	}
	IndexVector.clear();
	_Height = CalcChartHeight();
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::OnReceiveChartData(VtChartData* data)
{
	if (!data)
		return;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DrawSelected(DoubleArray& xData, DoubleArray& yData, XYChart* chart)
{
	if (!chart)
		return;
	
}

void VtChartFrm::DrawSelected(DoubleArray& xData, DrawArea* d, int startIndex, int nofPoints)
{
	if (!d)
		return;
}

void VtChartFrm::ValueToPixel(VtValue& value0, VtValue& value1, bool byValue)
{

}

void VtChartFrm::ValueToPixel()
{
	int startIndex = 0;
	int endIndex = 0;
	GetZoomRange(_ChartViewer, startIndex, endIndex);
	_shapesCollection.ValueToPixel(_StartID, startIndex, _MainChart);
}

void VtChartFrm::DeleteSelectedObject()
{
	_shapesCollection.DeleteSelectedObject();
	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DeleteAllObjects()
{
	_shapesCollection.DeleteAllObjects();
	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}

void VtChartFrm::Undo()
{
	_shapesCollection.Undo();
	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}

void VtChartFrm::Redo()
{
	_shapesCollection.Redo();
	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}

void VtChartFrm::SetChartSize(int width, int height)
{
	_Width = width;
	_Height = height;

	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::AddCompareData(VtChartData* data)
{
	if (!data || !_Data)
		return;

	// 메인 차트와 같으면 반환
	if (data->Key().compare(_Data->Key()) == 0)
		return;
	VtGlobal* global = VtGlobal::GetInstance();
	VtReferenceChart* refChart = new VtReferenceChart();
	refChart->show = true;
	refChart->name = data->Key();
	refChart->data = data;
	refChart->embedded = false;
	refChart->shareAxisWithMain = false;
	refChart->height = 150;
	refChart->type = 0;
	refChart->color = _ColorMgr->GetColor(refChart->data->ColorIndex());

	RefChartVector.push_back(refChart);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	data->AddChartWindow(_ParentChartWnd);
	chartDataMgr->AddChartDataRequest(10, data);
}

void VtChartFrm::RemoveCompareData(std::string key)
{
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->data->Key().compare(key) == 0)
		{
			RefChartVector.erase(it);
			if (_ParentChartWnd)
			{
				_ParentChartWnd->UnRegisterData(chart->data);
			}
			break;
		}
	}
}

void VtChartFrm::RemoveCompareData(VtChartData* data)
{
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		if (chart->data == data)
		{
			RefChartVector.erase(it);
			// Return the used color
			_ColorMgr->ReturnColorIndex(chart->data->ColorIndex());
			// Remove the chart window used from the map.
			data->RemoveChartWindow(_ParentChartWnd);
			VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
			// Remove the request of chart data from the timer.
			chartDataMgr->RemoveChartDataRequest(data);
			delete chart;
			break;
		}
	}
}



/// <summary>
/// 그려진 차트를 선택하는 방법
/// 그 차트와 축을 저장해서 그 차트와 축을 기준으로 좌표를 찾아야 찾을 수 있다.
/// 굳이 for 문을 돌지 않아도 된다.
/// </summary>
/// <param name="point"></param>
/// <returns></returns>
bool VtChartFrm::HitTestSymbol(CPoint point)
{
	if (!_SourceChart)
		return false;

	// It is possible for a FinanceChart to be empty, so we need to check for it.
	if (_SourceChart->getChartCount() == 0)
		return false;

	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = _SourceChart->initDynamicLayer();

	int mouseX = point.x;
	int mouseY = point.y;
	_SelectedChart = false;
	_SelectedData = _Data;
	int i = 0;
	for (auto it = _LayerList.begin(); it != _LayerList.end(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		layerInfo.data->Selected(false);
		int xValue = (int)(layerInfo.chart->getNearestXValue(mouseX));
		int xIndex = layerInfo.layer->getXIndexOf(xValue);
		int dataSetCount = layerInfo.layer->getDataSetCount();
		CRect rcTest;
		double yValue = 0, yValue2 = 0;
		if (dataSetCount == 4)
		{
			yValue = layerInfo.layer->getDataSet(0)->getValue(xIndex);
			yValue2 = layerInfo.layer->getDataSet(1)->getValue(xIndex);
		}
		else
		{
			yValue = layerInfo.layer->getDataSet(0)->getValue(xIndex);
			yValue2 = layerInfo.layer->getDataSet(0)->getValue(xIndex + 1);
		}
		int yCoor = layerInfo.chart->getYCoor(yValue, layerInfo.axis);
		int yCoor2 = layerInfo.chart->getYCoor(yValue2, layerInfo.axis);
		int xCoor = layerInfo.chart->getXCoor(xValue);
		if (dataSetCount == 4)
		{
			rcTest.left = xCoor - 10;
			rcTest.top = yCoor - 5;
			rcTest.right = rcTest.left + 20;
			rcTest.bottom = yCoor2 + 5;
		}
		else
		{
			rcTest.left = xCoor - 10;
			rcTest.top = yCoor - 10;
			rcTest.right = rcTest.left + 20;
			rcTest.bottom = rcTest.top + 20;
		}
		//Console.WriteLine("mouseY = " + mouseY);
		//Console.WriteLine("offsetY = " + c.getAbsOffsetY());
		//TRACE(name.c_str());

		int curX = mouseX + layerInfo.chart->getAbsOffsetX();
		int curY = mouseY + layerInfo.chart->getAbsOffsetY();
		CPoint curPoint;
		curPoint.x = curX;
		curPoint.y = curY;
		if (rcTest.PtInRect(curPoint))
		{
			layerInfo.data->Selected(true);
			layerInfo.selected = true;
			_SelectedChart = true;
			_SelectedData = layerInfo.data;
		}
		else
		{
			layerInfo.data->Selected(false);
			layerInfo.selected = false;
		}
		i++;
	}

	return false;
}

void VtChartFrm::HitTest(CPoint point)
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
	for (int i = 0; i < _SourceChart->getChartCount(); ++i) 
	{
		c = (XYChart *)_SourceChart->getChart(i);
		plotArea = c->getPlotArea();
		plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
		plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
		plotHeight = plotArea->getHeight();
		for (int j = 0; j < c->getLayerCount(); ++j)
		{
			Layer *layer = c->getLayer(j);
			int xValue = (int)(c->getNearestXValue((double)point.x));
			int xIndex = layer->getXIndexOf(xValue);
			int yCoor = 0;
			int yCoor2 = 0;
			int xCoor = c->getXCoor(xValue);

			double yValue = 0;
			int dataSetCount = layer->getDataSetCount();
			if (dataSetCount == 4)
			{
				dataName = layer->getDataSet(0)->getDataName();
				double yValue2 = layer->getDataSet(1)->getValue(xIndex);
				if (dataName.find(_T("Reference")) != std::string::npos)
				{
					Axis* axis = FindAxis(dataName);
					yValue = layer->getDataSet(0)->getValue(xIndex);
					yCoor = c->getYCoor(yValue, axis);
					//yCoor += plotAreaTopY;
					yCoor2 = c->getYCoor(yValue2);
					//yCoor2 += plotAreaTopY;

					xCoor = plotAreaLeftX + xCoor - 30;
					yCoor = plotAreaTopY + yCoor - 35;
					yCoor2 = plotAreaTopY + yCoor2 - 35;
				}
				else
				{
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
				if (rcTest.PtInRect(point))
				{
					found = true;
					int delimiterPosition = (int)dataName.find(';');
					if (delimiterPosition != std::string::npos)
						_SelectedDataSetName = dataName.substr(0, delimiterPosition);
					//break;
				}
			}
			else
			{
				if (dataSetCount == 1)
				{
					dataName = layer->getDataSet(0)->getDataName();
					if (dataName.find(_T("Reference")) != std::string::npos)
					{
						Axis* axis = FindAxis(dataName);
						yValue = layer->getDataSet(0)->getValue(xIndex);
						yCoor = c->getYCoor(yValue, axis);
					}
					else
					{
						yValue = layer->getDataSet(0)->getValue(xIndex);
						yCoor = c->getYCoor(yValue);
					}

					xCoor = plotAreaLeftX + xCoor - 30;
					yCoor = plotAreaTopY + yCoor - 35;

					rcTest.left = xCoor - 10;
					rcTest.top = yCoor - 10;
					rcTest.right = rcTest.left + 20;
					rcTest.bottom = rcTest.top + 20;

					if (rcTest.PtInRect(point))
					{
						found = true;
						int delimiterPosition = (int)dataName.find(';');
						if (delimiterPosition != std::string::npos)
							_SelectedDataSetName = dataName.substr(0, delimiterPosition);
						//AfxMessageBox(_SelectedDataSetName.c_str());
						break;
					}
				}
				else if (dataSetCount > 0)
				{
					dataName = layer->getDataSet(0)->getDataName();
					for (int k = 0; k < dataSetCount; ++k)
					{
						yValue = layer->getDataSet(k)->getValue(xIndex);
						yCoor = c->getYCoor(yValue);

						if (dataName.find(_T("Vol")) != std::string::npos)
						{
							Axis* axis = _MainChart->yAxis2();
							yCoor = c->getYCoor(yValue, axis);
						}

						xCoor = plotAreaLeftX + xCoor - 30;
						yCoor = plotAreaTopY + yCoor - 35;

						rcTest.left = xCoor - 10;
						rcTest.top = yCoor - 10;
						rcTest.right = rcTest.left + 20;
						rcTest.bottom = rcTest.top + 20;

						if (rcTest.PtInRect(point))
						{
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

		if (!found)
		{
			rcTest.left = plotAreaLeftX;
			rcTest.top = plotAreaTopY;
			rcTest.right = plotAreaLeftX + 10;
			rcTest.bottom = plotAreaTopY + 10;
			int pos;
			if (rcTest.PtInRect(point))
			{
				dataName = c->getLayer(0)->getDataSet(0)->getDataName();
				pos = dataName.find(_T(";"));
				_SelectedDataSetName = dataName.substr(0, pos);
				if (_SelectedDataSetName.find(_T("MainChart")) != std::string::npos)
				{
					Reset();
					ReleaseResource();
					break;
				}
				else
				{
					OnDeleteKey();
				}
			}

			rcTest.left = plotAreaLeftX + 11;
			rcTest.top = plotAreaTopY + 1;
			rcTest.right = plotAreaLeftX + 19;
			rcTest.bottom = plotAreaTopY + 9;

			if (rcTest.PtInRect(point))
			{
				dataName = c->getLayer(0)->getDataSet(0)->getDataName();
				pos = dataName.find(_T(";"));
				_SelectedDataSetName = dataName.substr(0, pos);
				if (_SelectedDataSetName.find(_T("MainChart")) != std::string::npos)
				{
					_ShowAll = !_ShowAll;
					ShowAllReferenceChart(_ShowAll);
					ShowAllIndex(_ShowAll);
					_SelectedDataSetName = _T("");
					return;
				}
				if (_SelectedDataSetName.length() > 0)
				{
					int pos = 0;
					if (_SelectedDataSetName.find(_T("Reference")) != std::string::npos)
					{
						pos = _SelectedDataSetName.find(_T(">"));
						if (pos != std::string::npos)
						{
							std::string index = _SelectedDataSetName.substr(pos + 1, _SelectedDataSetName.length() - pos - 1);
							int num = std::stoi(index);
							if (RefChartVector.size() > 0)
							{
								VtReferenceChart* chart = RefChartVector[num];
								chart->show = false;
								DrawChart(_ChartViewer, 0);
							}
							_SelectedDataSetName = _T("");
						}
					}
					else
					{
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
					}
				}
			}
		}
	}

	
	//TRACE(_SelectedDataSetName.c_str());
}

bool VtChartFrm::HitTestSystem(CPoint point)
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

void VtChartFrm::DeleteRefChartData()
{
	int i = 0;
	for (auto it = _LayerList.begin(); it != _LayerList.end(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		if (i == 0)
		{
			i++;
			continue;
		}

		if (layerInfo.data->Selected())
		{
			layerInfo.data->Selected(false);
			RemoveCompareData(layerInfo.data);
			break;
		}
		i++;
	}
	_SelectedChart = false;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::InsertIndex(CMFCPropertyGridProperty* prop)
{
	if (!prop)
		return;

	VtIndexFactory* indexFac = VtIndexFactory::GetInstance();
	
	VtBaseIndex* index = indexFac->CreateIndex(prop, IndexVector, _SourceChart);
	if (index)
	{
		IndexVector.push_back(index);
		DrawChart(_ChartViewer, 0);
	}
}

void VtChartFrm::InsertIndex(VtIndexBase* indexBase)
{
	if (!indexBase)
		return;

	VtIndexFactory* indexFac = VtIndexFactory::GetInstance();

	VtBaseIndex* index = indexFac->CreateIndex(indexBase->Type, IndexVector, _SourceChart);
	if (index)
	{
		IndexVector.push_back(index);
		DrawChart(_ChartViewer, 0);
	}
}

void VtChartFrm::DeleteIndex(int num)
{
	if (IndexVector.size() == 0)
		return;
	if (num < 0 || num >= IndexVector.size())
		return;

	VtBaseIndex* index = IndexVector[num];
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		VtBaseIndex* curIndex = *it;
		if (curIndex == index)
		{
			IndexVector.erase(it);
			break;
		}
	}

	delete index;

	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DeleteSystem()
{
	if (_System)
	{
		delete _System;
		_System = nullptr;
		DrawChart(_ChartViewer, 0);
	}
}

void VtChartFrm::InsertBand(VtBandBase* band)
{
	if (!band)
		return;

	VtIndexFactory* indexFac = VtIndexFactory::GetInstance();

	VtBaseBand* index = indexFac->CreateBand(band->Type, BandVector, _SourceChart);
	if (index)
	{
		BandVector.push_back(index);
		DrawChart(_ChartViewer, 0);
	}
}

void VtChartFrm::UpdateBand(VtBaseBand* band)
{
	if (!band)
		return;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DeleteBand(int num)
{
	if (BandVector.size() == 0)
		return;
	if (num < 0 || num >= BandVector.size())
		return;

	VtBaseBand* index = BandVector[num];
	for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
	{
		VtBaseBand* curIndex = *it;
		if (curIndex == index)
		{
			BandVector.erase(it);
			break;
		}
	}

	delete index;

	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::InsertStress(VtStressBase* stress)
{
	if (!stress)
		return;

	VtIndexFactory* indexFac = VtIndexFactory::GetInstance();

	VtBaseStress* index = indexFac->CreateStress(stress->Type, StressVector, _SourceChart);
	if (index)
	{
		StressVector.push_back(index);
		DrawChart(_ChartViewer, 0);
	}
}

void VtChartFrm::UpdateStress(VtBaseStress* stress)
{
	if (!stress)
		return;
	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::DeleteStress(int num)
{
	if (StressVector.size() == 0)
		return;
	if (num < 0 || num >= StressVector.size())
		return;

	VtBaseStress* index = StressVector[num];
	for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
	{
		VtBaseStress* curIndex = *it;
		if (curIndex == index)
		{
			StressVector.erase(it);
			break;
		}
	}

	delete index;

	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::UpdateReferenceChart(VtReferenceChart* refChart)
{
	if (!refChart)
		return;

	DrawChart(_ChartViewer, 0);
}

void VtChartFrm::InsertSystem(int index)
{
	if (index < 0)
		return;
	VtGlobal* global = VtGlobal::GetInstance();
	int systemCnt = global->SystemList.size();
	if (systemCnt == 0 || index >= systemCnt)
		return;
	VtSystemTemplate sysTempl = global->SystemList[index];
	SetSystem(sysTempl.Type);
}

void VtChartFrm::OnLButtonUp(UINT nFlags, CPoint point)
{
	int sx = _start.x;
	int sy = _start.y;
	//CalcPixelSize();
	int tmpX = point.x - _offset.x;
	int tmpY = point.y - _offset.y;

	//VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);

	switch (_editOpt)
	{
	case EditOption::Select:
		VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);
		if (_editStatus == EditStatus::SelectRect)
		{
			if (((tmpX - sx) + (tmpY - sy)) > 12)
			{
				_shapesCollection.MultiSelect(sx, sy, tmpX, tmpY);
			}
		}
		SetEditStatus(EditStatus::None);
		break;
	case EditOption::Rect:
		VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);
		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddRect(sx, sy, tmpX, tmpY, _creationPenColor, _creationFillColor, _creationPenWidth, _isCreationFilled);
			ChangeDimension();
		}
		break;

	case EditOption::MultiLine:
		if (_multiLineStarted == true)
		{
			VtPoint pt(tmpX, tmpY);
			_penPoints.push_back(pt);
		}
		break;
	case EditOption::Ellipse: //DrawEllipse
		VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);
		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddEllipse(sx, sy, tmpX, tmpY, _creationPenColor, _creationFillColor, _creationPenWidth, _isCreationFilled);
			ChangeDimension();
		}
		break;
	case EditOption::Line: //Draw Line
		//VtShapesCollection::SanitizeRect(sx, sy, tmpX, tmpY);
		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddLine(sx, sy, tmpX, tmpY, _creationPenColor, _creationPenWidth);
			ChangeDimension();
		}
		break;

	case EditOption::HLine:
		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddHLine(tmpY, _creationPenColor, _creationPenWidth);
			double yval = _MainChart->getYValue(tmpY);

			double rndVal = ROUNDING(yval, 2);
			std::string val = NumberFormatter::format(rndVal, 20, 2);
			ChangeDimension();
		}
		break;
	case EditOption::VLine:
		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddVLine(tmpX, _creationPenColor, _creationPenWidth);
			ChangeDimension();
		}
		break;
	case EditOption::Fibo: //Draw Line

		if (_editStatus == EditStatus::DrawRect)
		{
			_shapesCollection.AddFibo(sx, sy, tmpX, tmpY, _creationPenColor, _creationPenWidth);
			ChangeDimension();
		}
		break;
	default:
		SetEditStatus(EditStatus::None);
		break;
	}

	
	// store start X,Y,X1,Y1 of selected item
	if (_shapesCollection.SelectedElement != nullptr)
	{
		// 객체의 위치를 마지막으로 정해준다.
		if (_redimStatus != RedimStatus::None)
		{
			_shapesCollection.EndMove();
			ChangeDimension();
		}

		// 선택사각형의 위치도 마지막으로 정해준다.
		if (_shapesCollection.Tracker != nullptr)
			_shapesCollection.Tracker->EndMoveRedim();
	}

	if (!_multiLineStarted)
	{
		// 객체를 그리고 난 다음에는 무조건 선택 상태로 되돌린다.
		_isMouseLButtonDown = false;
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		//pMainWnd->ResetDrawing();
	}

	if (_redimStatus != RedimStatus::None)
	{
		ChangeOrder(tmpY, _shapesCollection.SelectedElement);
	}

	if (_ChartViewer != nullptr)
		DrawChart(_ChartViewer, 0);
}


void VtChartFrm::ShowAllIndex(bool flag)
{
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		VtBaseIndex* index = *it;
		index->Show = flag;
	}
}

void VtChartFrm::ShowAllReferenceChart(bool flag)
{
	for (auto it = RefChartVector.begin(); it != RefChartVector.end(); ++it)
	{
		VtReferenceChart* chart = *it;
		chart->show = flag;
	}
}

VtBaseIndex* VtChartFrm::FindIndex(std::string indexName)
{
	for (auto it = IndexVector.begin(); it != IndexVector.end(); ++it)
	{
		VtBaseIndex* index = *it;
		std::string name = index->IndexBase.Name;
		if (indexName.compare(name) == 0)
		{
			return index;
		}
	}

	return nullptr;
}

VtBaseBand* VtChartFrm::FindBand(std::string bandName)
{
	for (auto it = BandVector.begin(); it != BandVector.end(); ++it)
	{
		VtBaseBand* band = *it;
		std::string name = band->BandBase.Name;
		if (bandName.compare(name) == 0)
		{
			return band;
		}
	}

	return nullptr;
}

VtBaseStress* VtChartFrm::FindStress(std::string stressName)
{
	for (auto it = StressVector.begin(); it != StressVector.end(); ++it)
	{
		VtBaseStress* stress = *it;
		std::string name = stress->StressBase.Name;
		if (stressName.compare(name) == 0)
		{
			return stress;
		}
	}

	return nullptr;
}

void VtChartFrm::MouseToChart(CPoint src, VtPoint & des)
{
	des.x = src.x;
	des.y = src.y;
}

void VtChartFrm::DrawInstantObject()
{
	if (_SourceChart == nullptr)
		return;

	DrawArea *d = _SourceChart->initDynamicLayer();
	int sx = _start.x + _offset.x;
	int sy = _start.y + _offset.y;
	int ex = (int)(_temp.x + _offset.x);
	int ey = (int)(_temp.y + _offset.y);

	// 새로운 객체가 생성되기 전에 임시를 객체의 모양을 그리는 과정.
	if (_editStatus == EditStatus::DrawRect)
	{
		int clrFill = VtElement::ColorToInt(_creationFillColor);
		int clrPen = VtElement::ColorToInt(_creationPenColor);
		clrPen = d->dashLineColor(clrPen, Chart::DotLine);
		if (_editOpt == EditOption::Line)
		{
			VtLine::DrawLine(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
		}
		else if (_editOpt == EditOption::HLine)
		{
			VtHLine::DrawHLine(_temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
		}
		else if (_editOpt == EditOption::VLine)
		{
			VtVLine::DrawVLine(_temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
		}
		else if (_editOpt == EditOption::Ellipse)
		{
			VtEllipse::DrawEllipse(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, clrFill);
		}
		else if (_editOpt == EditOption::Rect)
		{
			VtRect::DrawRect(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, clrFill);
		}
		else if (_editOpt == EditOption::Fibo)
		{
			VtFibo::MakePoints(_start, _temp, _fiboPoints);
			VtFibo::DrawFibo(_start, _temp, _fiboPoints, _MainChart, _offset.x, _offset.y, d, clrPen, _creationPenWidth);
		}
		else
		{
			VtShapesCollection::SanitizeRect(sx, sy, ex, ey);
			d->rect(
				sx,
				sy,
				ex,
				ey, clrPen, clrFill);
		}
	}
	else
	{
		if (_editStatus == EditStatus::SelectRect)
		{
			int clrFill = VtElement::ColorToInt(_creationFillColor);
			int clrPen = VtElement::ColorToInt(_creationPenColor);
			clrPen = d->dashLineColor(clrPen, Chart::DotLine);
			VtShapesCollection::SanitizeRect(sx, sy, ex, ey);
			d->rect(
				sx,
				sy,
				ex,
				ey, clrPen, clrFill);
		}
	}
}

void VtChartFrm::DrawObjects(int idStart, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy)
{
	_shapesCollection.DrawSelected(idStart, zoomStartIndex, d, chart, dx, dy);
	_shapesCollection.DrawUnselected(idStart, zoomStartIndex, d, chart, dx, dy);
}

void VtChartFrm::DrawObjects(XYChart * chart, DrawArea* d)
{
	if (chart == nullptr || d == nullptr)
		return;

	_shapesCollection.DrawSelected(0, 0, d, chart, _offset.x, _offset.y);
	_shapesCollection.DrawUnselected(0, 0, d, chart, _offset.x, _offset.y);
}

void VtChartFrm::DrawSelection(DrawArea* d)
{
	if (_SourceChart == nullptr)
		return;

	int sx = _start.x + _offset.x;
	int sy = _start.y + _offset.y;
	int ex = (int)(_temp.x + _offset.x);
	int ey = (int)(_temp.y + _offset.y);

	int clrFill = VtElement::ColorToInt(_creationFillColor);
	int clrPen = VtElement::ColorToInt(_creationPenColor);
	clrPen = d->dashLineColor(clrPen, Chart::DotLine);
	VtShapesCollection::SanitizeRect(sx, sy, ex, ey);
	d->rect(
		sx,
		sy,
		ex,
		ey, clrPen, clrFill);
}

void VtChartFrm::DrawCreatingObject(DrawArea* d)
{
	if (_SourceChart == nullptr)
		return;

	int clrFill = VtElement::ColorToInt(_creationFillColor);
	int clrPen = VtElement::ColorToInt(_creationPenColor);
	clrPen = d->dashLineColor(clrPen, Chart::DotLine);
	if (_editOpt == EditOption::Line)
	{
		VtLine::DrawLine(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
	}
	else if (_editOpt == EditOption::HLine)
	{
		VtHLine::DrawHLine(_temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
	}
	else if (_editOpt == EditOption::VLine)
	{
		VtVLine::DrawVLine(_temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
	}
	else if (_editOpt == EditOption::Ellipse)
	{
		VtEllipse::DrawEllipse(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, clrFill);
	}
	else if (_editOpt == EditOption::Rect)
	{
		VtRect::DrawRect(_start, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, clrFill);
	}
	else if (_editOpt == EditOption::Fibo)
	{
		VtFibo::MakePoints(_start, _temp, _fiboPoints);
		VtFibo::DrawFibo(_start, _temp, _fiboPoints, _MainChart, _offset.x, _offset.y, d, clrPen, _creationPenWidth);
	}
	else if (_editOpt == EditOption::MultiLine)
	{
		VtMultiLine::DrawMultiLine(_penPoints, _temp, d, _MainChart, _offset.x, _offset.y, clrPen, _creationPenWidth);
	}
}

void VtChartFrm::ChangeDimension()
{
	PixelToValue();
	_shapesCollection.SaveDimension(_ChartViewer, _Data->GetDataCount(), _MainChart);
}


//
// Draw finance chart track line with legend
//
void VtChartFrm::trackFinance(MultiChart *m, int mouseX)
{
	CString pos;
	pos.Format(_T("tracFinance :: x = %d, y = %d \n"), mouseX, _ChartViewer->getPlotAreaMouseY());
	//TRACE(pos);

	if (!_Data)
		return;


	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = m->initDynamicLayer();

	if (_SourceChart != nullptr && _isMouseLButtonDown)
	{
		//DrawArea *d = _SourceChart->initDynamicLayer();
		if (_editStatus == EditStatus::DrawRect)
			DrawCreatingObject(d);
		else if (_editStatus == EditStatus::SelectRect)
			DrawSelection(d);
		else if (_editStatus == EditStatus::Redim)
		{
			if (_RedimStarted == true)
			{
				//DrawChart(_ChartViewer, 1);
				_RedimStarted = false;
			}
			//DrawChart(_ChartViewer, 0);
		}
	}

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
				if (name.find(_T("Reference")) != std::string::npos)
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
							label.append(_T(",화폐이름:"));
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
					if (name.find(_T("Reference")) != std::string::npos)
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
								label.append(_T(",화폐이름:"));
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
		for (int i = 1; i < datetime.len; i++)
		{
			double newDate = Chart::getChartYMD(datetime[i]);
			int xpos = _MainChart->getXCoor(i);
			if (oldDate != newDate)
			{
				d->line(xpos, plotAreaTopY, xpos, plotAreaTopY + height, color, 1);
			}

			oldDate = newDate;
		}

		d->line(plotAreaLeftX + width + _YAxisGap , plotAreaTopY, plotAreaLeftX + width + _YAxisGap, plotAreaTopY + height, 0xc6cbcc, 1);

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
		d->line(0, entireHeight -1, entireWidth, entireHeight-1, 0x000000, 1);
		d->line(0, 0, 0, entireHeight, 0x000000, 1);
		d->line(entireWidth -1, 0, entireWidth -1, entireHeight, 0x000000, 1);
		DrawRealtimeValues(d);
	}
}

void VtChartFrm::TrackFinance(MultiChart *m, int mouseX)
{
	if (!_MainChart)
		return;

	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = m->initDynamicLayer();

	// Get the data x-value that is nearest to the mouse
	int xValue = (int)_MainChart->getNearestXValue(mouseX);

	// Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
	XYChart *c = 0;
	PlotArea *plotArea = nullptr;
	int plotAreaLeftX = 0;
	int plotAreaTopY = 0;
	for (auto it = _LayerList.begin(); it != _LayerList.end(); ++it) 
	{
		VtLayerInfo layerInfo = *it;
		c = layerInfo.chart;
		c->setDefaultFonts("gulim.ttc", "gulim.ttc");
		// Variables to hold the legend entries
		ostringstream ohlcLegend;
		vector<string> legendEntries;

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

				if (closeValue != Chart::NoValue) {

					// Build the OHLC legend
					ohlcLegend << "      <*block*>";
					//ohlcLegend << outName.c_str();
					//ohlcLegend.write(outName.c_str(), outName.length());
					//ohlcLegend << title;
					std::string msg;
					msg.append(_T("index = "));
					msg.append(std::to_string(xIndex));
					ohlcLegend << msg.c_str();
					ohlcLegend << TCHARtoUTF8(_T("시가:"));
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
			}
			else
			{
				// Iterate through all the data sets in the layer
				for (int k = 0; k < layer->getDataSetCount(); ++k)
				{
					DataSet *dataSet = layer->getDataSetByZ(k);

					string name = dataSet->getDataName();
					double value = dataSet->getValue(xIndex);
					if ((0 != name.size()) && (value != Chart::NoValue)) {

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
						else {
							// In a FinanceChart, only the layer for volume bars has 3 data sets for
							// up/down/flat days
							if (dataSetCount == 3) {
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
				plotArea = c->getPlotArea();
				plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
				plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();
				int setCount = layer->getDataSetCount();
				if (setCount > 0)
				{
					for (int k = 0; k < _NoOfPoints; k = k + 6)
					{
						double curVal = 0.0;
						for (int s = 0; s < setCount; ++s)
						{
							curVal = layer->getDataSet(s)->getValue(k);
							int yVal = c->getYCoor(curVal);
							int xVal = c->getXCoor(k);
							d->circle(plotAreaLeftX + xVal - 30, plotAreaTopY + yVal - 35, 4, 4, 0x000000, 0xff0000);
						}
					}
				}
			}
		}

		// The legend begins with the date label, then the ohlcLegend (if any), and then the
		// entries for the indicators.
		ostringstream legendText;
		legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
			<< "*><*font=arialbd.ttf*>[" << c->xAxis()->getFormattedLabel(xValue, "mm/dd hh:nn")
			<< "]<*/font*>" << ohlcLegend.str();
		for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i) {
			legendText << "      " << legendEntries[i];
		}
		legendText << "<*/*>";

		if (c != _MainChart)
		{
			d->rect(plotAreaLeftX, plotAreaTopY, plotAreaLeftX + 10, plotAreaTopY + 10, 0xffffff, 0x000000);
			d->line(plotAreaLeftX, plotAreaTopY, plotAreaLeftX + 10, plotAreaTopY + 10, 0xffffff, 1);
			d->line(plotAreaLeftX + 10, plotAreaTopY, plotAreaLeftX, plotAreaTopY + 10, 0xffffff, 1);

			d->rect(plotAreaLeftX + 11, plotAreaTopY + 1, plotAreaLeftX + 19, plotAreaTopY + 9, 0x000000, 0xffffff);
			d->line(plotAreaLeftX + 11, plotAreaTopY + 5, plotAreaLeftX + 19, plotAreaTopY + 5, 0x000000, 1);

		}

		// Display the legend on the top of the plot area
		TTFText *t = d->text(legendText.str().c_str(), "gulim.ttc", 8);
		t->draw(plotAreaLeftX + 23, plotAreaTopY + 3, 0x000000, Chart::TopLeft);
		t->destroy();
	}
}

void VtChartFrm::DrawTrackLine(MultiChart*m, int mouseX)
{
	if (!m)
		return;
	if (!_isMouseLButtonDown)
		return;
	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = m->initDynamicLayer();

	// It is possible for a FinanceChart to be empty, so we need to check for it.
	if (m->getChartCount() == 0)
		return;
	if (!_MainChart)
		return;

	// Get the data x-value that is nearest to the mouse
	int xValue = (int)_MainChart->getNearestXValue(mouseX);

	// Get the plot area position relative to the entire FinanceChart
	PlotArea *plotArea = _MainChart->getPlotArea();
	int plotAreaLeftX = plotArea->getLeftX() + _MainChart->getAbsOffsetX();
	int plotAreaTopY = plotArea->getTopY() + _MainChart->getAbsOffsetY();

	// Draw a vertical track line at the x-position
	d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), _MainChart->getXCoor(xValue) +
		_MainChart->getAbsOffsetX(), 0xff0000);
}

Axis* VtChartFrm::FindAxis(std::string dataName)
{
	for (auto it = _LayerList.begin(); it != _LayerList.end(); ++it)
	{
		VtLayerInfo layerInfo = *it;
		Layer* layer = layerInfo.layer;
		int datasetCount = layer->getDataSetCount();
		if (datasetCount > 0)
		{
			for (int i = 0; i < datasetCount; ++i)
			{
				std::string name = layer->getDataSet(i)->getDataName();
				if (name.find(dataName) != std::string::npos)
				{
					return layerInfo.axis;
				}
			}
		}
	}

	return nullptr;
}

#include "stdafx.h"
#include "VtIndexFactory.h"
#include "VtMaIndex.h"
#include "VtVolumeIndex.h"
#include "VtBollingerBandIndex.h"
#include "VtPriceChannelIndex.h"
#include "VtMacdIndex.h"
#include "VtStochasticIndex.h"
#include "VtRsiIndex.h"
#include "VtTrixIndex.h"
#include "VtBaseIndex.h"
#include "VtIndexDefine.h"
#include "FinanceChart.h"
#include "VtAtrIndex.h"
#include "VtGlobal.h"
#include "VtNDailyBand.h"
#include "VtDailyBand.h"
#include "VtEntireBand.h"
#include "VtOrbBand.h"
#include "VtSimpleMaStress.h"
#include "VtRobo1Stress.h"
#include "VtBaseStress.h"
#include "VtBaseBand.h"
#include "VtIndexDefine.h"
VtIndexFactory::VtIndexFactory()
{
}


VtIndexFactory::~VtIndexFactory()
{
}

VtBaseIndex* VtIndexFactory::CreateIndex(VtIndexType type, std::vector<VtBaseIndex*>& indexList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	int chartHeight = sourceChart->getHeight();
	int indexHeight = 0;

	VtBaseIndex* index = nullptr;
	VtGlobal* global = VtGlobal::GetInstance();
	int count = 0;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	switch (type)
	{
	case VtIndexType::SimpleMa:
		index = new VtMaIndex(VtIndexType::SimpleMa);
		count = GetIndexTypeCount(VtIndexType::SimpleMa, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("단순이동평균");
		else
		{
			multi.Format(_T("단순이동평균 %d"), count);
			index->IndexBase.Name = multi;
		}
		((VtMaIndex*)index)->Period = 5;
		((VtMaIndex*)index)->Color = global->ColorTable[count];
		return index;
	case VtIndexType::DataVolume:
		count = GetIndexTypeCount(VtIndexType::DataVolume, indexList);
		if (count > 0)
			return index;

		index = new VtVolumeIndex(VtIndexType::DataVolume);
		index->IndexBase.Name = _T("거래량");

		((VtVolumeIndex*)index)->Height = 75;

		((VtVolumeIndex*)index)->UpColor = 0xff0000;

		((VtVolumeIndex*)index)->DownColor = 0x0000ff;

		((VtVolumeIndex*)index)->FlatColor = 0x000000;
		return index;
	case VtIndexType::BollingerBand:
		count = GetIndexTypeCount(VtIndexType::BollingerBand, indexList);
		if (count > 0)
			return index;
		index = new VtBollingerBandIndex(VtIndexType::BollingerBand);
		index->IndexBase.Name = _T("볼린저밴드");

		((VtBollingerBandIndex*)index)->Period = 20;

		((VtBollingerBandIndex*)index)->Width = 2;

		((VtBollingerBandIndex*)index)->LineColor = 0x000000;

		((VtBollingerBandIndex*)index)->FillColor = 0xffffff;
		return index;
	case VtIndexType::PriceChannel:
		count = GetIndexTypeCount(VtIndexType::PriceChannel, indexList);
		if (count > 0)
			return index;
		index = new VtPriceChannelIndex(VtIndexType::PriceChannel);
		index->IndexBase.Name = _T("가격채널");

		((VtPriceChannelIndex*)index)->Period = 24;

		((VtPriceChannelIndex*)index)->LineColor = 0xff0000;

		((VtPriceChannelIndex*)index)->FillColor = 0xffffff;
		return index;
	case VtIndexType::Macd:
		index = new VtMacdIndex(VtIndexType::Macd);
		count = GetIndexTypeCount(VtIndexType::Macd, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("MACD");
		else
		{
			multi.Format(_T("MACD%d"), count);
			index->IndexBase.Name = multi;
		}

		indexHeight = 75;
		if (indexHeight != 0)
			index->Portion = (double)indexHeight / (double)chartHeight;
		((VtMacdIndex*)index)->Height = 75;

		((VtMacdIndex*)index)->ShortPeriod = 12;

		((VtMacdIndex*)index)->LongPeriod = 24;

		((VtMacdIndex*)index)->Period = 9;

		((VtMacdIndex*)index)->Color = 0xff0000;

		((VtMacdIndex*)index)->SignalColor = 0x0000ff;

		((VtMacdIndex*)index)->DivColor = 0xcccccc;
		return index;
	case VtIndexType::Rsi:
		index = new VtRsiIndex(VtIndexType::Rsi);
		count = GetIndexTypeCount(VtIndexType::Rsi, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("RSI");
		else
		{
			multi.Format(_T("RSI%d"), count);
			index->IndexBase.Name = multi;
		}

		indexHeight = 75;
		if (indexHeight != 0)
			index->Portion = (double)indexHeight / (double)chartHeight;
		((VtRsiIndex*)index)->Height = 75;

		((VtRsiIndex*)index)->Period = 10;

		((VtRsiIndex*)index)->Color = 0xff00ff;

		((VtRsiIndex*)index)->Range = 2;

		((VtRsiIndex*)index)->UpColor = 0xff0000;

		((VtRsiIndex*)index)->DownColor = 0x0000ff;
		return index;
	case VtIndexType::Stochastic:
		index = new VtStochasticIndex(VtIndexType::Stochastic);
		count = GetIndexTypeCount(VtIndexType::Stochastic, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("Stochastic");
		else
		{
			multi.Format(_T("Stochastic%d"), count);
			index->IndexBase.Name = multi;
		}

		indexHeight = 75;
		if (indexHeight != 0)
			index->Portion = (double)indexHeight / (double)chartHeight;
		((VtStochasticIndex*)index)->Height = 75;

		((VtStochasticIndex*)index)->FastK = 5;

		((VtStochasticIndex*)index)->SlowK = 10;

		((VtStochasticIndex*)index)->ColorSlowK = 0xff0000;

		((VtStochasticIndex*)index)->ColorSlowD = 0x0000ff;
		return index;
	case VtIndexType::Trix:
		index = new VtTrixIndex(VtIndexType::Trix);
		count = GetIndexTypeCount(VtIndexType::Trix, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("TRIX");
		else
		{
			multi.Format(_T("TRIX%d"), count);
			index->IndexBase.Name = multi;
		}

		indexHeight = 75;
		if (indexHeight != 0)
			index->Portion = (double)indexHeight / (double)chartHeight;
		((VtTrixIndex*)index)->Height = 75;

		((VtTrixIndex*)index)->Period = 10;

		((VtTrixIndex*)index)->Color = 5;
		return index;
	case VtIndexType::Atr:
		index = new VtAtrIndex(VtIndexType::Atr);
		count = GetIndexTypeCount(VtIndexType::Atr, indexList);
		if (count == 0)
			index->IndexBase.Name = _T("ATR");
		else
		{
			multi.Format(_T("ATR%d"), count);
			index->IndexBase.Name = multi;
		}

		indexHeight = 75;
		if (indexHeight != 0)
			index->Portion = (double)indexHeight / (double)chartHeight;
		((VtAtrIndex*)index)->Height = 75;

		((VtAtrIndex*)index)->Period1 = 20;

		((VtAtrIndex*)index)->Period2 = 9;

		((VtAtrIndex*)index)->Color1 = 0xff0000;

		((VtAtrIndex*)index)->Color2 = 0x0000ff;
	default:
		break;
	}

	return index;
}


VtBaseIndex* VtIndexFactory::CreateIndex(VtIndexType type)
{
	VtBaseIndex* index = nullptr;
	switch (type)
	{
	case VtIndexType::SimpleMa:
		index = new VtMaIndex(VtIndexType::SimpleMa);
		index->IndexBase.Name = _T("단순이동평균");
		((VtMaIndex*)index)->Color = 0xff0000;
		((VtMaIndex*)index)->Period = 5;
		return index;
	case VtIndexType::DataVolume:
		index = new VtVolumeIndex(VtIndexType::DataVolume);
		index->IndexBase.Name = _T("거래량");
		((VtVolumeIndex*)index)->Height = 75;
		((VtVolumeIndex*)index)->UpColor = 0xff0000;
		((VtVolumeIndex*)index)->DownColor = 0x0000ff;
		((VtVolumeIndex*)index)->FlatColor = 0x000000;
		return index;
	case VtIndexType::BollingerBand:
		index = new VtBollingerBandIndex(VtIndexType::BollingerBand);
		index->IndexBase.Name = _T("볼린저밴드");
		((VtBollingerBandIndex*)index)->Period = 20;
		((VtBollingerBandIndex*)index)->Width = 20;
		((VtBollingerBandIndex*)index)->LineColor = 0x0000ff;
		((VtBollingerBandIndex*)index)->FillColor = 0x000000;
		return index;
	case VtIndexType::PriceChannel:
		index = new VtPriceChannelIndex(VtIndexType::PriceChannel);
		index->IndexBase.Name = _T("가격채널");
		((VtPriceChannelIndex*)index)->Period = 20;
		((VtPriceChannelIndex*)index)->LineColor = 0xff0000;
		((VtPriceChannelIndex*)index)->FillColor = 0x0000ff;
		return index;
	case VtIndexType::Macd:
		index = new VtMacdIndex(VtIndexType::Macd);
		index->IndexBase.Name = _T("MACD");
		((VtMacdIndex*)index)->Height = 75;
		((VtMacdIndex*)index)->ShortPeriod = 12;
		((VtMacdIndex*)index)->LongPeriod = 6;
		((VtMacdIndex*)index)->Period = 5;
		((VtMacdIndex*)index)->Color = 0xff0000;
		((VtMacdIndex*)index)->SignalColor = 0x0000ff;
		((VtMacdIndex*)index)->DivColor = 0x000000;
		return index;
	case VtIndexType::Rsi:
		index = new VtRsiIndex(VtIndexType::Rsi);
		index->IndexBase.Name = _T("RSI");
		((VtRsiIndex*)index)->Height = 75;
		((VtRsiIndex*)index)->Period = 40;
		((VtRsiIndex*)index)->Color = 0x0000ff;
		((VtRsiIndex*)index)->Range = 20;
		((VtRsiIndex*)index)->UpColor = 0xff0000;
		((VtRsiIndex*)index)->DownColor = 0x0000ff;
		return index;
	case VtIndexType::Stochastic:
		index = new VtStochasticIndex(VtIndexType::Stochastic);
		index->IndexBase.Name = _T("Stochastic");
		((VtStochasticIndex*)index)->Height = 75;
		((VtStochasticIndex*)index)->FastK = 12;
		((VtStochasticIndex*)index)->SlowK = 5;
		((VtStochasticIndex*)index)->SlowD = 5;
		((VtStochasticIndex*)index)->ColorSlowK = 0x0000ff;
		((VtStochasticIndex*)index)->ColorSlowD = 0x000000;
		return index;
	case VtIndexType::Trix:
		index = new VtTrixIndex(VtIndexType::Trix);
		index->IndexBase.Name = _T("TRIX");
		((VtTrixIndex*)index)->Height = 75;
		((VtTrixIndex*)index)->Period = 10;
		((VtTrixIndex*)index)->Color = 0x0000ff;
		return index;
	case VtIndexType::Atr:
		index = new VtAtrIndex(VtIndexType::Atr);
		index->IndexBase.Name = _T("TRIX");
		((VtAtrIndex*)index)->Height = 200;
		((VtAtrIndex*)index)->Period1 = 10;
		((VtAtrIndex*)index)->Period2 = 30;
		((VtAtrIndex*)index)->Color1 = 0xff0000;
		((VtAtrIndex*)index)->Color2 = 0x0000ff;
		return index;
	default:
		break;
	}

	return index;
}

VtBaseIndex* VtIndexFactory::CreateIndex(CMFCPropertyGridProperty* prop)
{
	if (!prop)
		return nullptr;

	VtIndexBase* base = (VtIndexBase*)prop->GetData();
	VtBaseIndex* index = nullptr;
	CString val;
	CMFCPropertyGridProperty* sub = nullptr;
	if (base)
	{
		switch (base->Type)
		{
		case VtIndexType::SimpleMa:
			index = new VtMaIndex(VtIndexType::SimpleMa);
			index->IndexBase.Name = _T("단순이동평균");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtMaIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMaIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::DataVolume:
			index = new VtVolumeIndex(VtIndexType::DataVolume);
			index->IndexBase.Name = _T("거래량");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->DownColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->FlatColor = _ttoi(val);
			return index;
		case VtIndexType::BollingerBand:
			index = new VtBollingerBandIndex(VtIndexType::BollingerBand);
			index->IndexBase.Name = _T("볼린저밴드");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Width = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::PriceChannel:
			index = new VtPriceChannelIndex(VtIndexType::PriceChannel);
			index->IndexBase.Name = _T("가격채널");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::Macd:
			index = new VtMacdIndex(VtIndexType::Macd);
			index->IndexBase.Name = _T("MACD");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMacdIndex*)index)->ShortPeriod = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtMacdIndex*)index)->LongPeriod = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtMacdIndex*)index)->SignalColor = _ttoi(val);
			sub = prop->GetSubItem(6);
			val = sub->GetValue();
			((VtMacdIndex*)index)->DivColor = _ttoi(val);
			return index;
		case VtIndexType::Rsi:
			index = new VtRsiIndex(VtIndexType::Rsi);
			index->IndexBase.Name = _T("RSI");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Range = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtRsiIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtRsiIndex*)index)->DownColor = _ttoi(val);
			return index;
		case VtIndexType::Stochastic:
			index = new VtStochasticIndex(VtIndexType::Stochastic);
			index->IndexBase.Name = _T("Stochastic");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->FastK = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->SlowK = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowK = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowD = _ttoi(val);
			return index;
		case VtIndexType::Trix:
			index = new VtTrixIndex(VtIndexType::Trix);
			index->IndexBase.Name = _T("TRIX");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::Atr:
			index = new VtAtrIndex(VtIndexType::Atr);
			index->IndexBase.Name = _T("ATR");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period1 = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period2 = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color1 = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color2 = _ttoi(val);
			return index;
		default:
			break;
		}
	}

	return index;
}

VtBaseIndex* VtIndexFactory::CreateIndex(CMFCPropertyGridProperty* prop, std::vector<VtBaseIndex*>& indexList)
{
	if (!prop)
		return nullptr;

	VtIndexBase* base = (VtIndexBase*)prop->GetData();
	VtBaseIndex* index = nullptr;
	int count = 0;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	if (base)
	{
		switch (base->Type)
		{
		case VtIndexType::SimpleMa:
			index = new VtMaIndex(VtIndexType::SimpleMa);
			count = GetIndexTypeCount(VtIndexType::SimpleMa, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("단순이동평균");
			else
			{
				multi.Format(_T("단순이동평균 %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtMaIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMaIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::DataVolume:
			count = GetIndexTypeCount(VtIndexType::DataVolume, indexList);
			if (count > 0)
				return index;

			index = new VtVolumeIndex(VtIndexType::DataVolume);
			index->IndexBase.Name = _T("거래량");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->DownColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->FlatColor = _ttoi(val);
			return index;
		case VtIndexType::BollingerBand:
			count = GetIndexTypeCount(VtIndexType::BollingerBand, indexList);
			if (count > 0)
				return index;
			index = new VtBollingerBandIndex(VtIndexType::BollingerBand);
			index->IndexBase.Name = _T("볼린저밴드");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Width = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::PriceChannel:
			count = GetIndexTypeCount(VtIndexType::PriceChannel, indexList);
			if (count > 0)
				return index;
			index = new VtPriceChannelIndex(VtIndexType::PriceChannel);
			index->IndexBase.Name = _T("가격채널");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::Macd:
			index = new VtMacdIndex(VtIndexType::Macd);
			count = GetIndexTypeCount(VtIndexType::Macd, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("MACD");
			else
			{
				multi.Format(_T("MACD %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMacdIndex*)index)->ShortPeriod = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtMacdIndex*)index)->LongPeriod = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtMacdIndex*)index)->SignalColor = _ttoi(val);
			sub = prop->GetSubItem(6);
			val = sub->GetValue();
			((VtMacdIndex*)index)->DivColor = _ttoi(val);
			return index;
		case VtIndexType::Rsi:
			index = new VtRsiIndex(VtIndexType::Rsi);
			count = GetIndexTypeCount(VtIndexType::Rsi, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("RSI");
			else
			{
				multi.Format(_T("RSI %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Range = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtRsiIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtRsiIndex*)index)->DownColor = _ttoi(val);
			return index;
		case VtIndexType::Stochastic:
			index = new VtStochasticIndex(VtIndexType::Stochastic);
			count = GetIndexTypeCount(VtIndexType::Stochastic, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("Stochastic");
			else
			{
				multi.Format(_T("Stochastic %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->FastK = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->SlowK = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowK = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowD = _ttoi(val);
			return index;
		case VtIndexType::Trix:
			index = new VtTrixIndex(VtIndexType::Trix);
			count = GetIndexTypeCount(VtIndexType::Trix, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("TRIX");
			else
			{
				multi.Format(_T("TRIX %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::Atr:
			index = new VtAtrIndex(VtIndexType::Atr);
			count = GetIndexTypeCount(VtIndexType::Atr, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("ATR");
			else
			{
				multi.Format(_T("ATR %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period1 = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period2 = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color1 = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color2 = _ttoi(val);
		default:
			break;
		}
	}

	return index;
}

VtBaseIndex* VtIndexFactory::CreateIndex(CMFCPropertyGridProperty* prop, std::vector<VtBaseIndex*>& indexList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	if (!prop)
		return nullptr;
	int chartHeight = sourceChart->getHeight();
	int indexHeight = 0;

	CMFCPropertyGridProperty* parent = prop->GetParent();
	if (parent)
	{
		prop = prop->GetParent();
	}

	VtIndexBase* base = (VtIndexBase*)prop->GetData();
	VtBaseIndex* index = nullptr;
	int count = 0;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	if (base)
	{
		switch (base->Type)
		{
		case VtIndexType::SimpleMa:
			index = new VtMaIndex(VtIndexType::SimpleMa);
			count = GetIndexTypeCount(VtIndexType::SimpleMa, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("단순이동평균");
			else
			{
				multi.Format(_T("단순이동평균 %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtMaIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMaIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::DataVolume:
			count = GetIndexTypeCount(VtIndexType::DataVolume, indexList);
			if (count > 0)
				return index;

			index = new VtVolumeIndex(VtIndexType::DataVolume);
			index->IndexBase.Name = _T("거래량");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->DownColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtVolumeIndex*)index)->FlatColor = _ttoi(val);
			return index;
		case VtIndexType::BollingerBand:
			count = GetIndexTypeCount(VtIndexType::BollingerBand, indexList);
			if (count > 0)
				return index;
			index = new VtBollingerBandIndex(VtIndexType::BollingerBand);
			index->IndexBase.Name = _T("볼린저밴드");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->Width = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtBollingerBandIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::PriceChannel:
			count = GetIndexTypeCount(VtIndexType::PriceChannel, indexList);
			if (count > 0)
				return index;
			index = new VtPriceChannelIndex(VtIndexType::PriceChannel);
			index->IndexBase.Name = _T("가격채널");
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->LineColor = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtPriceChannelIndex*)index)->FillColor = _ttoi(val);
			return index;
		case VtIndexType::Macd:
			index = new VtMacdIndex(VtIndexType::Macd);
			count = GetIndexTypeCount(VtIndexType::Macd, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("MACD");
			else
			{
				multi.Format(_T("MACD %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			indexHeight = _ttoi(val);
			if (indexHeight != 0)
				index->Portion = (double)indexHeight / (double)chartHeight;
			((VtMacdIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtMacdIndex*)index)->ShortPeriod = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtMacdIndex*)index)->LongPeriod = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtMacdIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtMacdIndex*)index)->SignalColor = _ttoi(val);
			sub = prop->GetSubItem(6);
			val = sub->GetValue();
			((VtMacdIndex*)index)->DivColor = _ttoi(val);
			return index;
		case VtIndexType::Rsi:
			index = new VtRsiIndex(VtIndexType::Rsi);
			count = GetIndexTypeCount(VtIndexType::Rsi, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("RSI");
			else
			{
				multi.Format(_T("RSI %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			indexHeight = _ttoi(val);
			if (indexHeight != 0)
				index->Portion = (double)indexHeight / (double)chartHeight;
			((VtRsiIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Color = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtRsiIndex*)index)->Range = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtRsiIndex*)index)->UpColor = _ttoi(val);
			sub = prop->GetSubItem(5);
			val = sub->GetValue();
			((VtRsiIndex*)index)->DownColor = _ttoi(val);
			return index;
		case VtIndexType::Stochastic:
			index = new VtStochasticIndex(VtIndexType::Stochastic);
			count = GetIndexTypeCount(VtIndexType::Stochastic, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("Stochastic");
			else
			{
				multi.Format(_T("Stochastic %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			indexHeight = _ttoi(val);
			if (indexHeight != 0)
				index->Portion = (double)indexHeight / (double)chartHeight;
			((VtStochasticIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->FastK = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->SlowK = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowK = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtStochasticIndex*)index)->ColorSlowD = _ttoi(val);
			return index;
		case VtIndexType::Trix:
			index = new VtTrixIndex(VtIndexType::Trix);
			count = GetIndexTypeCount(VtIndexType::Trix, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("TRIX");
			else
			{
				multi.Format(_T("TRIX %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			indexHeight = _ttoi(val);
			if (indexHeight != 0)
				index->Portion = (double)indexHeight / (double)chartHeight;
			((VtTrixIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Period = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtTrixIndex*)index)->Color = _ttoi(val);
			return index;
		case VtIndexType::Atr:
			index = new VtAtrIndex(VtIndexType::Atr);
			count = GetIndexTypeCount(VtIndexType::Atr, indexList);
			if (count == 0)
				index->IndexBase.Name = _T("ATR");
			else
			{
				multi.Format(_T("ATR %d"), count);
				index->IndexBase.Name = multi;
			}
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			indexHeight = _ttoi(val);
			if (indexHeight != 0)
				index->Portion = (double)indexHeight / (double)chartHeight;
			((VtAtrIndex*)index)->Height = _ttoi(val);
			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period1 = _ttoi(val);
			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Period2 = _ttoi(val);
			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color1 = _ttoi(val);
			sub = prop->GetSubItem(4);
			val = sub->GetValue();
			((VtAtrIndex*)index)->Color2 = _ttoi(val);
		default:
			break;
		}
	}

	return index;
}

VtBaseBand* VtIndexFactory::CreateBand(VtBandType type, std::vector<VtBaseBand*>& bandList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	int chartHeight = sourceChart->getHeight();
	int indexHeight = 0;

	VtBaseBand* band = nullptr;
	VtGlobal* global = VtGlobal::GetInstance();
	int count = 0;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	switch (type)
	{
	case VtBandType::Ndaily:
		band = new VtNDailyBand(VtBandType::Ndaily);
		band->BandBase.Name = global->BandList[0]->Name;
		((VtNDailyBand*)band)->BackLen = 1;
		((VtNDailyBand*)band)->HighColor = RGB(255, 0, 0);
		((VtNDailyBand*)band)->LowColor = RGB(0, 0, 255);
		return band;
	case VtBandType::Daily:
		band = new VtDailyBand(VtBandType::Daily);
		band->BandBase.Name = global->BandList[1]->Name;
		((VtDailyBand*)band)->HighColor = RGB(255, 0,0);
		((VtDailyBand*)band)->LowColor = RGB(0, 0, 255);
		return band;
	case VtBandType::Entire:
		band = new VtEntireBand(VtBandType::Entire);
		((VtEntireBand*)band)->HighColor = RGB(255, 0, 0);
		((VtEntireBand*)band)->LowColor = RGB(0, 0, 255);
		band->BandBase.Name = global->BandList[2]->Name;
		return band;
	case VtBandType::ORB:
		band = new VtOrbBand(VtBandType::ORB);
		band->BandBase.Name = global->BandList[3]->Name;
		((VtOrbBand*)band)->HighColor = RGB(255, 0, 0);
		((VtOrbBand*)band)->LowColor = RGB(0, 0, 255);
		((VtOrbBand*)band)->Dividance = 4;
		((VtOrbBand*)band)->BackLen = 1;
		return band;
	default:
		break;
	}

	return band;
}

VtBaseBand* VtIndexFactory::CreateBand(CMFCPropertyGridProperty* prop, std::vector<VtBaseBand*>& bandList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	if (!prop)
		return nullptr;
	
	CMFCPropertyGridProperty* parent = prop->GetParent();
	if (parent)
	{
		prop = prop->GetParent();
	}

	VtBandBase* base = (VtBandBase*)prop->GetData();
	VtBaseBand* band = nullptr;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	VtGlobal* global = VtGlobal::GetInstance();
	if (base)
	{
		switch (base->Type)
		{
		case VtBandType::Ndaily:
			band = new VtNDailyBand(VtBandType::Ndaily);
			band->BandBase.Name = global->BandList[0]->Name;
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtNDailyBand*)band)->BackLen = _ttoi(val);

			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtNDailyBand*)band)->HighColor = _ttoi(val);

			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtNDailyBand*)band)->LowColor = _ttoi(val);

			return band;
		case VtBandType::Daily:
			band = new VtDailyBand(VtBandType::Daily);
			band->BandBase.Name = global->BandList[1]->Name;
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtDailyBand*)band)->HighColor = _ttoi(val);

			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtDailyBand*)band)->LowColor = _ttoi(val);

			return band;
		case VtBandType::Entire:
			band = new VtEntireBand(VtBandType::Entire);
			band->BandBase.Name = global->BandList[2]->Name;
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtEntireBand*)band)->HighColor = _ttoi(val);

			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtEntireBand*)band)->LowColor = _ttoi(val);


			return band;
		case VtBandType::ORB:
			band = new VtOrbBand(VtBandType::ORB);
			band->BandBase.Name = global->BandList[3]->Name;
			sub = prop->GetSubItem(0);
			val = sub->GetValue();
			((VtOrbBand*)band)->BackLen = _ttoi(val);

			sub = prop->GetSubItem(1);
			val = sub->GetValue();
			((VtOrbBand*)band)->Dividance = _ttoi(val);


			sub = prop->GetSubItem(2);
			val = sub->GetValue();
			((VtOrbBand*)band)->HighColor = _ttoi(val);


			sub = prop->GetSubItem(3);
			val = sub->GetValue();
			((VtOrbBand*)band)->LowColor = _ttoi(val);
			return band;
		default:
			break;
		}
	}

	return band;
}

VtBaseStress* VtIndexFactory::CreateStress(VtStressType type, std::vector<VtBaseStress*>& stressList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	int chartHeight = sourceChart->getHeight();
	int indexHeight = 0;

	VtBaseStress* stress = nullptr;
	VtGlobal* global = VtGlobal::GetInstance();
	int count = 0;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	switch (type)
	{
	case VtStressType::VRobo1:
		stress = new VtSimpleMaStress(VtStressType::VRobo1);
		stress->StressBase.Name = global->StressList[0]->Name;
		return stress;
	case VtStressType::VRobo2:
		stress = new VtRobo1Stress(VtStressType::VRobo2);
		stress->StressBase.Name = global->StressList[1]->Name;
		return stress;
	default:
		break;
	}

	return stress;
}

VtBaseStress* VtIndexFactory::CreateStess(CMFCPropertyGridProperty* prop, std::vector<VtBaseStress*>& stressList, FinanceChart* sourceChart)
{
	if (!sourceChart)
		return nullptr;
	if (!prop)
		return nullptr;

	CMFCPropertyGridProperty* parent = prop->GetParent();
	if (parent)
	{
		prop = prop->GetParent();
	}

	VtStressBase* base = (VtStressBase*)prop->GetData();
	VtBaseStress* stress = nullptr;
	CString val;
	CString multi;
	CMFCPropertyGridProperty* sub = nullptr;
	VtGlobal* global = VtGlobal::GetInstance();
	if (base)
	{
		switch (base->Type)
		{
		case VtStressType::VRobo1:
			stress = new VtSimpleMaStress(VtStressType::VRobo1);
			stress->StressBase.Name = global->StressList[0]->Name;
			return stress;
		case VtStressType::VRobo2:
			stress = new VtRobo1Stress(VtStressType::VRobo2);
			stress->StressBase.Name = global->StressList[1]->Name;
			return stress;
		default:
			break;
		}
	}
	return stress;
}

int VtIndexFactory::GetIndexTypeCount(VtIndexType type, std::vector<VtBaseIndex*>& indexList)
{
	int count = 0;
	for (auto it = indexList.begin(); it != indexList.end(); ++it)
	{
		VtBaseIndex* index = *it;
		if (index->IndexBase.Type == type)
		{
			count++;
		}
	}

	return count;
}

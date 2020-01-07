#include "stdafx.h"
#include "VtSymbol.h"
#include "HdTaskArg.h"
#include "HdScheduler.h"
#include "Chart/SmChartData.h"
#include "Log/loguru.hpp"

int VtSymbol::count = 0;

VtSymbol::VtSymbol()
{
	_RecentMonth = false;
	_RequestedSymbolMaster = false;
	Begin();
	count++;
	//LOG_F(INFO, "constructor :: symbol = count = %d", count);
}


VtSymbol::~VtSymbol()
{
	//LOG_F(INFO, "destructor :: symbol = count = %d", count);
}

void VtSymbol::Begin()
{
	UserDefinedName = _T("");
	for (int i = 0; i < 5; i++)
	{
		Hoga.Ary[i].BuyNo = 0;
		Hoga.Ary[i].BuyPrice = 0;
		Hoga.Ary[i].BuyQty = 0;
		Hoga.Ary[i].IntBuyPrice = 0;

		Hoga.Ary[i].SellNo = 0;
		Hoga.Ary[i].SellPrice = 0;
		Hoga.Ary[i].SellQty = 0;
		Hoga.Ary[i].IntSellPrice = 0;
	}

	Hoga.TotBuyNo = 0;
	Hoga.TotSellNo = 0;
	Hoga.TotBuyQty = 0;
	Hoga.TotSellQty = 0;

	Quote.close = 0;
	Quote.high = 0;
	Quote.low = 0;
	Quote.open = 0;
	Quote.intClose = 0;
	Quote.intHigh = 0;
	Quote.intOpen = 0;
	Quote.intLow = 0;

	intTickSize = 5;
	Decimal = 2;
	TickValue = 12500;
	Seungsu = 250000;
}

void VtSymbol::End()
{

}

void VtSymbol::GetSymbolMaster()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdSymbolMaster;
	arg.AddArg(_T("SymbolCode"), ShortCode);
	scheduler->Available(true);
	scheduler->AddTask(std::move(arg));
}

void VtSymbol::UpdateChartValue()
{
	for (auto it = _ChartDataMap.begin(); it != _ChartDataMap.end(); ++it) {
		SmChartData* chart_data = it->second;
		if (chart_data->Received()) {
			chart_data->UpdateLastValue(Quote.intClose);
		}
	}
}

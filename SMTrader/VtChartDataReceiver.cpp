#include "stdafx.h"
#include "VtChartDataReceiver.h"
#include "ESApiLayout.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtChartData.h"
#include "VtChartDataManager.h"
#include "chartdir.h"


#include <Poco/String.h>
#include <Poco/NumberParser.h>
#include "Poco/Delegate.h"

#include "MainFrm.h"


using Poco::trim;
using Poco::trimLeft;
using Poco::trimRight;
using Poco::trimRightInPlace;

using Poco::NumberParser;
using Poco::Delegate;

VtChartDataReceiver::VtChartDataReceiver()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//_ChartDataReceivedEvent += delegate(pMainFrm, &CMainFrame::OnChartDataReceived);
}


VtChartDataReceiver::~VtChartDataReceiver()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//_ChartDataReceivedEvent -= delegate(pMainFrm, &CMainFrame::OnChartDataReceived);
}

void VtChartDataReceiver::ReceivedPacket(LPCTSTR param)
{
	CString msg;
	msg.Format(_T("param len = %d"), SZ_Es5522O);
	TRACE(msg);

	lpEs5522O_st pChartStruct = (lpEs5522O_st)param;

	int errorCode = 0;
	char buffer[256];
	memset(buffer, 0x00, sizeof(buffer));
	memcpy(buffer, pChartStruct->Head.ErrCode, 4);
	std::string err = buffer;
	if (!NumberParser::tryParse(err, errorCode))
	{
		return;
	}

	if (errorCode != 0)
	{
		return;
	}

	std::string msgid, next;
	msgid.assign(pChartStruct->Head.WinId, sizeof(pChartStruct->Head.WinId));
	next.assign(pChartStruct->Head.NextKind, sizeof(pChartStruct->Head.NextKind));

	fullCode.assign(pChartStruct->FullCode, sizeof(pChartStruct->FullCode));
	fullCode = trim(fullCode);

	maxDataCnt.assign(pChartStruct->MaxDataCnt, sizeof(pChartStruct->MaxDataCnt));
	dataCnt.assign(pChartStruct->DataCnt, sizeof(pChartStruct->DataCnt));
	tickCnt.assign(pChartStruct->TickCnt, sizeof(pChartStruct->TickCnt));
	toDay.assign(pChartStruct->Today, sizeof(pChartStruct->Today));
	preClose.assign(pChartStruct->nonedata, sizeof(pChartStruct->nonedata));
	dataGb.assign(pChartStruct->DataGb, sizeof(pChartStruct->DataGb));
	dataCnt.assign(pChartStruct->DataCnt, sizeof(pChartStruct->DataCnt));
	summary.assign(pChartStruct->Summary, sizeof(pChartStruct->Summary));
	preLast.assign(pChartStruct->PrevLast, sizeof(pChartStruct->PrevLast));

	VtChartType type = VtChartType::MIN;
	if (dataGb.compare(_T("1")) == 0)
		type = VtChartType::DAY;
	else if (dataGb.compare(_T("2")) == 0)
		type = VtChartType::WEEK;
	else if (dataGb.compare(_T("3")) == 0)
		type = VtChartType::MONTH;
	else if (dataGb.compare(_T("4")) == 0)
		type = VtChartType::TICK;
	int cycle = 0;
	if (!NumberParser::tryParse(summary, cycle))
	{
		return;
	}
	VtChartData* data = nullptr;
	int count = 0;
	if (!NumberParser::tryParse(dataCnt, count))
	{
		return;
	}
	int index = 0;

	std::string dataID = fullCode;
	dataID.append(dataGb);
	dataID.append(summary);

	auto pos = _PacketMap.find(dataID);
	if (pos != _PacketMap.end())
	{
		data = _PacketMap[dataID];
		index = data->CurIndex();
	}
	else
	{
		data = VtChartDataManager::GetInstance()->FindNAdd(fullCode, dataGb, summary);
		data->CurIndex(0);
		_PacketMap[dataID] = data;
	}

	try
	{
		for (int i = 0; i < count; i++)
		{
			date = VtChartData::GetDate(pChartStruct->Ary[i].Date);
			time = VtChartData::GetTime(pChartStruct->Ary[i].Time);
			openPrice.assign(pChartStruct->Ary[i].OpenPrice, sizeof(pChartStruct->Ary[i].OpenPrice));
			highPrice.assign(pChartStruct->Ary[i].HighPrice, sizeof(pChartStruct->Ary[i].HighPrice));
			lowPrice.assign(pChartStruct->Ary[i].LowPrice, sizeof(pChartStruct->Ary[i].LowPrice));
			closePrice.assign(pChartStruct->Ary[i].ClosePrice, sizeof(pChartStruct->Ary[i].ClosePrice));
			volume.assign(pChartStruct->Ary[i].Volume, sizeof(pChartStruct->Ary[i].Volume));
			data->Date[index] = date;
			data->Time[index] = time;
			data->DateTime[index] = Chart::chartTime(date.year, date.month, date.day, time.hour, time.min, time.sec);

			data->High[index] = std::stod(highPrice);
			data->Low[index] = std::stod(lowPrice);
			data->Open[index] = std::stod(openPrice);
			data->Close[index] = std::stod(closePrice);
			data->Volume[index] = std::stod(volume);
			data->Id[index] = index;
			index++;
		}
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		AfxMessageBox(msg.c_str());
	}
	data->CurIndex(index);
	if (next.compare(_T("0")) == 0)
	{
		VtChartDataEventArgs arg;
		arg.pChart = data;
		FireChartDataReceived(std::move(arg));
		_PacketMap.erase(dataID);
	}
}

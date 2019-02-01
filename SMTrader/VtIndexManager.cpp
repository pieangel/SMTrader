#include "stdafx.h"
#include "VtIndexManager.h"
#include "ta_func.h"
#include "VtChartData.h"
#include <array>
#include <cstring>
#include "VtIndex.h"
#include "VtChartDataManager.h"
VtIndexManager::VtIndexManager()
{
}


VtIndexManager::~VtIndexManager()
{
	for (auto it = IndexMap.begin(); it != IndexMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtIndexManager::AddIndex(std::string key, VtIndex* index)
{
	if (!index)
		return;

	IndexMap[key] = index;
}

void VtIndexManager::MakeMA(VtChartData* data, TA_MAType type, int period)
{
	if (!data)
		return;
	std::string key;
	key = VtChartDataManager::MakeChartDataKey(data->SymbolCode(), data->ChartType(), data->Cycle());
	key.append(_T(":"));
	key.append(std::to_string(type));
	key.append(_T(":"));
	key.append(std::to_string(period));

	VtIndex* index = FindMA(key);
	if (!index)
		CreateMA(key, data, type, period);
	else
		UpdateMA(index, key, data, type, period);
}

VtIndex* VtIndexManager::FindMA(std::string key)
{
	auto it = IndexMap.find(key);
	if (it != IndexMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtIndexManager::CreateMA(std::string key, VtChartData* data, TA_MAType type, int period)
{
	if (!data)
		return;

	int startIdx = 0, endIdx = data->GetDataCount() - 1;
	int outBegIdx = 0, outNbElement = 0;
	double temp[ChartDataSize];
	std::memset(temp, 0x00, sizeof(temp));

	double* OutPut = new double[ChartDataSize];
	int retCode = TA_MA(startIdx,
		endIdx,
		data->Close.data(),
		period,
		type,
		&outBegIdx,
		&outNbElement,
		temp);

	std::memcpy(OutPut + outBegIdx, temp, outNbElement * sizeof(temp[0]));
	VtIndex* index = new VtIndex();
	index->DataCount(data->GetDataCount());
	index->OutDataMap[key] = OutPut;
	std::memcpy(index->Id, data->Id.data(), sizeof(data->Id.data()));
	AddIndex(key, index);
}

void VtIndexManager::UpdateMA(VtIndex* index, std::string key, VtChartData* data, TA_MAType type, int period)
{
	if (!data || !index)
		return;

	int startIdx = data->GetDataCount() - 3, endIdx = data->GetDataCount() - 1;
	int outBegIdx = 0, outNbElement = 0;
	double temp[5];
	std::memset(temp, 0x00, sizeof(temp));

	int retCode = TA_MA(startIdx,
		endIdx,
		data->Close.data(),
		period,
		type,
		&outBegIdx,
		&outNbElement,
		temp);

	double* output = index->OutDataMap[key];
	if (!output)
		return;

	int dif = data->GetLastIndex() - index->GetLastIndex();
	if (dif > 0)
	{
		std::memmove(index->Id, index->Id + dif, sizeof(index->Id) - sizeof(index->Id[0]) * dif);
		std::memmove(output, output + dif, sizeof(double)*index->DataCount() - sizeof(double) * dif);
	}

	for (int i = outNbElement - 1, j = index->DataCount() - 1; i >= 0; --i, --j)
	{
		output[j] = temp[i];
	}
}

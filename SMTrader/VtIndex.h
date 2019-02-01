#pragma once
#include <array>
#include <cstring>
#include <map>
#include "Global/VtDefine.h"
class VtChartData;
class VtIndex
{
public:
	VtIndex();
	~VtIndex();

	VtChartData* SrcData() const { return _SrcData; }
	void SrcData(VtChartData* val) { _SrcData = val; }
	std::map<std::string, double*> OutDataMap;
	int DataCount() const { return _DataCount; }
	void DataCount(int val) { _DataCount = val; }
	int GetLastIndex()
	{
		return Id[DataCount() - 1];
	}
	int Id[ChartDataSize];
private:
	VtChartData* _SrcData = nullptr;
	int _DataCount = 0;
};


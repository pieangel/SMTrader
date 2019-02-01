#pragma once
#include <string>
#include <map>
#include "ta_func.h"
#include "Global/TemplateSingleton.h"
class VtIndex;
class VtChartData;
class VtIndexManager : public TemplateSingleton<VtIndexManager>
{
public:
	VtIndexManager();
	~VtIndexManager();
	std::map<std::string, VtIndex*> IndexMap;
	void AddIndex(std::string key, VtIndex* index);
	void MakeMA(VtChartData* data, TA_MAType type, int period);
	VtIndex* FindMA(std::string key);
private:
	void CreateMA(std::string key, VtChartData* data, TA_MAType type, int period);
	void UpdateMA(VtIndex*index, std::string key, VtChartData* data, TA_MAType type, int period);
};


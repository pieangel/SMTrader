#pragma once
#include "Global/TemplateSingleton.h"
#include "VtIndexDefine.h"
#include <vector>
class VtBaseIndex;
class VtBaseStress;
class VtBaseBand;
class CMFCPropertyGridProperty;
class FinanceChart;
class VtIndexFactory : public TemplateSingleton<VtIndexFactory>
{
public:
	VtIndexFactory();
	~VtIndexFactory();
	VtBaseIndex* CreateIndex(VtIndexType type, std::vector<VtBaseIndex*>& indexList, FinanceChart* sourceChart);
	VtBaseIndex* CreateIndex(VtIndexType type);
	VtBaseIndex* CreateIndex(CMFCPropertyGridProperty* prop);
	VtBaseIndex* CreateIndex(CMFCPropertyGridProperty* prop, std::vector<VtBaseIndex*>& indexList);
	VtBaseIndex* CreateIndex(CMFCPropertyGridProperty* prop, std::vector<VtBaseIndex*>& indexList, FinanceChart* sourceChart);

	VtBaseBand* CreateBand(VtBandType type, std::vector<VtBaseBand*>& bandList, FinanceChart* sourceChart);
	VtBaseBand* CreateBand(CMFCPropertyGridProperty* prop, std::vector<VtBaseBand*>& bandList, FinanceChart* sourceChart);

	VtBaseStress* CreateStress(VtStressType type, std::vector<VtBaseStress*>& stressList, FinanceChart* sourceChart);
	VtBaseStress* CreateStess(CMFCPropertyGridProperty* prop, std::vector<VtBaseStress*>& stressList, FinanceChart* sourceChart);

	int GetIndexTypeCount(VtIndexType type, std::vector<VtBaseIndex*>& indexList);
};


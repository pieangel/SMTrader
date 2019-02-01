
#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>

class VtFundOrderManager;
class VtFund;
class VtFundOrderManagerSelecter : public TemplateSingleton<VtFundOrderManagerSelecter>
{
public:
	VtFundOrderManagerSelecter();
	~VtFundOrderManagerSelecter();

	VtFundOrderManager* ActiveOrderManager() const { return _ActiveOrderManager; }
	void ActiveOrderManager(VtFundOrderManager* val) { _ActiveOrderManager = val; }
	VtFundOrderManager* FindAddOrderManager(std::string fundName);
	void AddOrderManager(VtFund* fund);
	void SetActiveOrderManager(VtFund* fund);
	VtFundOrderManager* FindOrderManager(std::string fundName);
private:
	VtFundOrderManager* _ActiveOrderManager = nullptr;
public:
	std::map<std::string, VtFundOrderManager*> _OrderManagerMap;
};



#pragma once
#include "VtOrderManager.h"
#include <string>
#include <vector>
#include <map>

class VtFund;
class VtFundOrderManager : public VtOrderManager
{
public:
	VtFundOrderManager();
	virtual ~VtFundOrderManager();

	std::string FundName;

	VtFund* CurFund() const { return _CurFund; }
	void CurFund(VtFund* val);
	virtual std::vector<VtOrder*> GetAcceptedOrders(std::string symbolCode);
	std::vector<VtAccount*> GetParentAccountList();
	void UpdateOrderManagers();
	std::map<std::string, VtOrderManager*>& GetOrderManagerMap()
	{
		return _OrderMgrMap;
	}
	virtual std::map<std::string, VtOrder*> GetTotalRemain(std::string symbolCode);
	virtual void ClearRemainOrderMap(std::string symbolCode);
private:
	VtFund* _CurFund;
	std::map<std::string, VtOrderManager*> _OrderMgrMap;
	void MakeOrderManagers();
};


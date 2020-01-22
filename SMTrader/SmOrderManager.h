#pragma once
#include <map>
#include <string>
struct VtOrder;
class SmOrderManager
{
public:
	SmOrderManager();
	~SmOrderManager();
	virtual void AddOrder(VtOrder* order);
	virtual void AddAcceptedOrder(VtOrder* order);
	virtual void AddFilledOrder(VtOrder* order);
private:
	// 전체 주문 목록
	std::map<std::string, VtOrder*> _TotalOrderMap;
	// 접수확인 주문 목록
	std::map<std::string, VtOrder*> _AcceptedOrderMap;
	// 체결된 주문 목록
	std::map<std::string, VtOrder*> _FilledOrderMap;
	// 청산된 주문 목록
	std::map<std::string, VtOrder*> _SettledOrderMap;
};


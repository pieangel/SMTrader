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
	// ��ü �ֹ� ���
	std::map<std::string, VtOrder*> _TotalOrderMap;
	// ����Ȯ�� �ֹ� ���
	std::map<std::string, VtOrder*> _AcceptedOrderMap;
	// ü��� �ֹ� ���
	std::map<std::string, VtOrder*> _FilledOrderMap;
	// û��� �ֹ� ���
	std::map<std::string, VtOrder*> _SettledOrderMap;
};


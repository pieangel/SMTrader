#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include <tuple>
#include <vector>
struct VtOrder;
struct VtPosition;
class VtTotalOrderManager : public TemplateSingleton<VtTotalOrderManager>
{
public:
	VtTotalOrderManager();
	~VtTotalOrderManager();

	void AddOrder(VtOrder* order);
	void AddAcceptedOrder(VtOrder* order);
	void RemoveAcceptedOrder(VtOrder* order);
	void RemoveAcceptedOrder(int orderNo);
	void AddFilledOrder(VtOrder* order);
	void AddPosition(int type, std::string acntNo, std::string symCode, VtPosition* posi);
	void RemovePosition(int type, std::string acntNo, std::string symCode);
	std::vector<std::string> GetUnsettledList();
	size_t GetUnsettledCount();
	std::map<int, VtOrder*>& GetOrderMap()
	{
		return _OrderMap;
	}
	int GetOrderCount() {
		return _OrderMap.size();
	}
private:
	// key : order no, value : order object
	std::map<int, VtOrder*> _OrderMap;
	std::map<int, VtOrder*> _AcceptedMap;
	std::map<int, VtOrder*> _FilledMap;
	std::map<std::tuple<int, std::string, std::string>, VtPosition*> _PositionMap;
};


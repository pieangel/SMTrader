#pragma once
#include <map>
#include "Global/VtDefine.h"
struct VtOrder;
class XYChart;
class VtSymbolMaster;
class VtChartOrderManager
{
public:
	VtChartOrderManager();
	~VtChartOrderManager();
	
	std::map<int, VtOrder*> PreOrderMap;
	std::map<std::string, VtOrder*> AcceptedMap;
	std::map<std::string, VtOrder*> FilledMap;

	void OrderArrived(VtOrder* order);
	void AddPreOrder(VtOrder* order);
	void OrderAccepted(VtOrder* order);
	void OrderFilled(VtOrder* order);

	void RemovePreOrder(int orderNo);
	void RemoveAccepted(std::string orderNo);
	void RemoveFilled(std::string orderNo);
	XYChart* Chart() const { return _Chart; }
	void Chart(XYChart* val) { _Chart = val; }
	int DefaultOrderQty() const { return _DefaultOrderQty; }
	void DefaultOrderQty(int val) { _DefaultOrderQty = val; }
	void PutOrder(int id, VtOrderCmd cmd, int Qty, double price, VtSymbolMaster* symMstr);
	void ChangeOrder(double newValue, VtOrder* oldOrder);
private:
	XYChart* _Chart = nullptr;
	int _DefaultOrderQty = 1;
};


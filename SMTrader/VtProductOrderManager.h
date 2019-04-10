#pragma once
#include <map>
#include <list>
#include "Global/VtDefine.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
struct VtOrder;
class VtSymbolMaster;
struct VtPosition;
class VtAccount;
class VtOrderManager;
class VtSymbol;
class VtProductOrderManager
{
public:
	VtProductOrderManager();
	~VtProductOrderManager();

	void ClearAccepted();
	void ClearFilled();
	std::map<int, VtOrder*> AcceptedMap;
	std::list<VtOrder*> FilledList;

	void OnRemain(VtPosition&& curPosi);
	void AddAccepted(VtOrder* order);
	void OrderArrived(VtOrder* order);
	void OnAccepted(VtOrder* order);
	void OnFilled(VtOrder* order);
	//void OnFilled(VtOrder* order, VtPosition* posi);
	void OnConfirmModify(VtOrder* order);
	void OnConfirmCancel(VtOrder* order);

	void PutOrder(int id, VtOrderCmd cmd, int Qty, double price, VtSymbolMaster* symMstr);
	void ChangeOrder(double newValue, VtOrder* order);
	void CancelOrder(VtOrder* order);
	std::string ProductCode() const { return _ProductCode; }
	void ProductCode(std::string val) { _ProductCode = val; }
	VtPosition* Position() const { return _Position; }
	void Position(VtPosition* val);
	VtSymbolMaster* SymbolMaster() const { return _SymbolMaster; }
	void SymbolMaster(VtSymbolMaster* val) { _SymbolMaster = val; }

	void CancelAllBuyAccepted();
	void CancelAllSellAccepted();
	void CancelAllAccepted();
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtOrderManager* OrderMgr() const { return _OrderMgr; }
	void OrderMgr(VtOrderManager* val) { _OrderMgr = val; }

	void RemoveAcceptedOrder(int oldOrderNo);
	void OnReceiveQuoteHd(VtSymbol* sym);
	int GetAcceptedOrderCount();
	bool Init() const { return _Init; }
	void Init(bool val) { _Init = val; }
	void AddToRemain(VtOrder* order);
	void RemoveFromRemain(int orderNo);
	void AddToLiqud(VtOrder* order);
	void RemoveFromLiqud(int orderNo);
	void RegisterOrderPosition(VtOrder* order, VtPosition* posi);
	std::map<int, VtOrder*> GetRemainMap() {
		return RemainMap;
	}

	void RefreshAcceptedOrders();
	void RefreshAcceptedOrder(int orderNo);
private:
	// �ܰ� �ֹ� ��
	std::map<int, VtOrder*> RemainMap;
	// û�� ��û ���� �� - û���� ������ �������.
	// û���� ���� �ܰ�� �ִ� �ֹ��� �ֹ� ��ȣ�� û���û�� �Բ� ������.
	// û�� ��û���� ���� �ֹ���ȣ�� ã�Ƽ� û���� �ش�.
	std::map<int, VtOrder*> LiqudMap;
	VtOrderManager* _OrderMgr = nullptr;
	std::string _ProductCode;
	VtPosition* _Position = nullptr;
	VtSymbolMaster* _SymbolMaster = nullptr;
	VtAccount* _Account = nullptr;
	VtOrder* FindAccepted(int orderNo);

	void ResetPositionValue(VtPosition* posi);
	double CalcOpenPL(VtOrder* order, double avgPrice, int count);
	double CalcOpenPL(VtOrder* order, int avgPrice, int count);
	double CalcTradePL(double avgPrice, std::string filledPrice, int count);

	double CalcTradePL(double avgPrice, double filledPrice, int count);
	double CalcOpenPLHd(double close, VtPosition* posi, VtSymbol* sym);

	// �ѹ��̶� ���� �ֹ��� �Ǿ��� ��� ������ �ȴ�.
	bool _Init = false;
	void AdjustRemainQ(int newRemainQty);
	void RemoveAllRemain();
	void RemoveByOrderNo(int orderNo);
public:
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};


#pragma once

/// <summary>
/// �ֹ� ���� ���
/// ��� �ֹ��� �����ֹ����� ���۵ȴ�.
/// �ֹ��� �����Ǹ� �����ֹ����� ���ŵǰ� 
/// �ֹ�������� �Ű�����. 
/// �׸��� ��ü�� �Ǵ� ü���Ͽ� ��ϵȴ�.
/// �׸��� �ֹ���ü�� ���� �޸� ������ 
/// ���ֹ� ��Ͽ����� ������ �ȴ�.
/// �ٸ� ���� �ֹ��� ������ ���� �ʾ��� ��� 
/// �״�� ���� �ֱ� ������ 
/// ó������ ���� ���� �ֹ��� ���ؼ��� ���� �ֹ� ����� ó���Ѵ�.
/// </summary>

#include <list>
#include <map>
#include "VtOrder.h"
#include "Global/VtDefine.h"
#include "VtUniqueOrderID.h"
#include <mutex>
#include <vector>
#include "VtOrderEvent.h"
#include "Poco/BasicEvent.h"
#include "VtPosition.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
using Poco::BasicEvent;
class VtAccount;
class VtSymbolMaster;
class VtTaskOrderFilledArg;
class VtTaskAccountFilledArg;
class VtSymbol;
class VtProductOrderManagerSelector;
class VtProductOrderManager;
/// <summary>
/// �ֹ��� �����ϴ� Ŭ����
/// �ֹ��� ũ�� �����ֹ��� �Ϲ��ֹ����� ������.
/// ���� �ֹ��� �ֹ� ��ü�� ����� ���� �ǽð� �ü��� ���� �ֹ� ���ݿ� ��ġ�ϸ� 
/// ������ Ȥ�� ���尡�� �ֹ��� ����.
/// �Ϲ��ֹ��� �ֹ��� ����� ���� �ֹ��� �־� ���� 
/// �������� �ֹ��� ��������� ���� �ֹ� ��Ͽ��� ã�Ƽ� �����ְ�
/// �ֹ���Ͽ� �־��ش�.
/// �ֹ������ ������ ���� ������ �̷������.
/// 1. Ŭ���̾�Ʈ �ֹ� ���� 
/// 2. Ŭ���̾�Ʈ ���� �ֹ� ��Ͽ� �߰�
/// 3. ������ �ֹ�����
/// 4. �����κ��� �ֹ���ȣ�� ������ �ֹ� ���� ����
/// 5. ���� �ֹ� ��Ͽ��� ã�Ƽ� �ֹ��� �����ְ� �ֹ� ��Ͽ� �߰�
/// 6. ������ ���� ���� ��ϸ޽��� ����
/// 7. �����κ��� ����Ȯ�� �޽��� ����
/// 8. �����κ��� ü�� Ȯ�� �޽��� ����.
/// �����̳� ��� �ֹ��� �����ϰ� �� ������ ��ģ��.
/// �ٸ� 8������ ����Ȯ��, ���Ȯ��, Ȥ�� �����ź�, ��Ұź� �޽����� �����Ѵ�.
/// </summary>
/// 
/// �߿� : �ֹ��������� ü���� �������� ��찡 ���� - �� ��쿡 ����Ͽ� ���α׷��� �����ؾ� ��.
class VtOrderManager 
{
public:
	/// <summary>
	/// �ֹ���û ������ �׸���� ä���.
	/// </summary>
	/// <param name="acnt"></param>
	void RequestFilledOrder(VtAccount* acnt);
	void RequestFilledOrder(std::string acntNo);
	void RequestFilledOrder(std::string accountNo, int Range, int OrderBy);
	void RequestOrderList(VtTaskAccountFilledArg* arg);
	void ReceiveFilledOrder();
	/// <summary>
	/// �ֹ���û ���̵� ����� ����.
	/// </summary>
	/// <returns></returns>
	int GetOrderRequestID()
	{
		std::lock_guard<std::mutex> lock(m_);
		return VtUniqueOrderID::GetID();
	}
	/// <summary>
	/// �ֹ���û ���̵� ������
	/// </summary>
	VtOrderManager();
	virtual ~VtOrderManager();
	/// <summary>
	/// �ż��ֹ�
	/// </summary>
	/// <param name="amount"></param>
	void Buy(int amount);
	/// <summary>
	/// �ŵ� �ֹ�
	/// </summary>
	/// <param name="amount"></param>
	void Sell(int amount);
	/// <summary>
	/// �ֹ���û��ü�� �Բ� �ű� �ֹ� ��û
	/// </summary>
	/// <param name="request"></param>
	void PutOrder(VtOrderRequest&& request);
	/// <summary>
	/// �����ֹ����� �ֹ� ���� ��û
	/// </summary>
	/// <param name="oldOrder"></param>
	/// <param name="newValue"></param>
	/// <param name="newQty"></param>
	void ChangeOrder(VtOrder* oldOrder, double newValue);
	/// <summary>
	/// ���� �ֹ����� �ֹ� ��� ��û
	/// </summary>
	/// <param name="oldOrder"></param>
	void CancelOrder(VtOrder* oldOrder);
	/// <summary>
	/// ��Ʈ���� �ֹ� ��û
	/// </summary>
	/// <param name="chartId"></param>
	/// <param name="cmd"></param>
	/// <param name="qty"></param>
	/// <param name="price"></param>
	/// <param name="symMstr"></param>
	void PutOrder(int chartId, VtOrderCmd cmd, int qty, double price, VtSymbolMaster* symMstr);

	/// <summary>
	/// ��ü �ֹ� �� : Ű�� �ֹ���ȣ
	/// </summary>
	std::map<int, VtOrder*> OrderMap;
	/// <summary>
	/// ���� �ֹ� ���
	/// </summary>
	std::list<VtOrder*> PreOrderList;
	/// <summary>
	/// ����Ȯ�� �ֹ� ���
	/// </summary>
	//std::list<VtOrder*> AcceptedList;
	std::map<int, VtOrder*> AcceptedMap;
	/// <summary>
	/// ���¿��� ������ ���� ü��� ��ü�� ���
	/// ���������� ���ȴ�.
	/// �ý����� �ٽ� ������ �� �̰��� �ҷ��� ������ ��� �Ѵ�.
	/// �̸���� �ҷ��� �� FilledMap�� ü��� ���� �־���� �Ѵ�.
	/// �׷��� �ֹ��� �ùٸ��� ó���ȴ�.
	/// �� �ֹ��� ������ �ƴ϶� ����Ǿ�� �Ѵ�.
	/// </summary>
	std::list<VtOrder*> FilledList;

	/// <summary>
	/// ����ڰ� �ֹ��� ����� ���� ���
	/// �ֹ� ó���� ���ؼ� ���ȴ�.
	/// </summary>
	std::map<std::string, std::list<VtOrder*>*> FilledMap;
	/// <summary>
	/// �ֹ���Ͽ� �ֹ� �߰�
	/// </summary>
	/// <param name="order"></param>
	void AddOrder(VtOrder* order);
	/// <summary>
	/// ���� �ֹ� ��Ͽ� �ֹ� �߰�
	/// </summary>
	/// <param name="order"></param>
	void AddPreOrder(VtOrder* order);
	/// <summary>
	/// ����Ȯ�� �ֹ� ��Ͽ� �ֹ� �߰�
	/// </summary>
	/// <param name="order"></param>
	void AddAccepted(VtOrder* order);
	/// <summary>
	/// ü�� �ֹ� ��Ͽ� �ֹ� �߰�
	/// </summary>
	/// <param name="order"></param>
	void AddFilled(VtOrder* order);
	/// <summary>
	/// ���� �ֹ� ��Ͽ��� �ֹ�����
	/// </summary>
	/// <param name="orderRequestID"></param>
	/// <returns></returns>
	VtOrder* RemovePreOrder(int orderRequestID);
	/// <summary>
	/// ����Ȯ�� �ֹ���Ͽ��� �ֹ� ����
	/// </summary>
	/// <param name="orderNo"></param>
	void RemoveAccepted(int orderNo);
	/// <summary>
	/// ü�� �ֹ� ��Ͽ��� �ֹ� ����
	/// </summary>
	/// <param name="orderNo"></param>
	void RemoveFilled(int orderNo);
	/// <summary>
	/// �ֹ� ��û ��ȣ�� �ֹ� ��ü �˻�
	/// </summary>
	/// <param name="requestID"></param>
	/// <returns></returns>
	VtOrder* FindPreOrderByRequestID(int requestID);
	/// <summary>
	/// ����Ȯ�� ��Ͽ��� �ֹ� �˻�
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindAccepted(int orderNo);
	/// <summary>
	/// ü�� �ֹ����� �ֹ� �˻�
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindFilled(int orderNo);
	/// <summary>
	/// �ֹ� ��Ͽ��� �ֹ� �˻�
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindOrder(int orderNo);
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	VtOrder* CloneOrder(VtOrder* src);
	void CopyOrder(VtOrder* src, VtOrder* dst);
	/// <summary>
	/// ���� �ֹ��� �̿��Ͽ� �ֹ���û ���� ����
	/// </summary>
	/// <param name="request"></param>
	/// <param name="order"></param>
	void FillRequest(VtOrderRequest&& request, VtOrder* order);
	/// <summary>
	/// �ֹ� ���� ����
	/// </summary>
	/// <param name="order"></param>
	/// <param name="real"></param>
	void SetOrderInfo(VtOrder* order, VtRealtimeOrder&& real);
	/// <summary>
	/// ���� �ֹ��������� ���� ��ü
	/// </summary>
	/// <returns></returns>
	VtAccount* CurAccount() const { return _CurAccount; }
	void CurAccount(VtAccount* val) { _CurAccount = val; }
	/// <summary>
	/// �������� ���ʷ� ��ü�� ����� ���� �� �ֹ��� �ʿ� �߰��ϴ� �Լ�
	/// </summary>
	/// <param name="order"></param>
	void AddAcceptedFromList(VtOrder* order);
	void PutOrder(HdOrderRequest&& request);
	void ChangeOrder(HdOrderRequest&& request);
	void CancelOrder(HdOrderRequest&& request);
	virtual std::vector<VtOrder*> GetAcceptedOrders(std::string symbolCode);
	void GetAcceptedOrders(std::string symbolCode, std::vector<VtOrder*>& orderVector);
private:
	/// <summary>
	/// ���� �ֹ��������� ���� ��ü
	/// �ֹ� �����ڴ� ���� �ϳ��� ���¸��� �ٷ��.
	/// �� �ٸ� ���°� ���� ��� 
	/// �� ���´� �ٸ� �ֹ� �����ڸ� ������.
	/// ���¸��� ������ �ֹ� �����ڸ� ������ �ȴ�.
	/// </summary>
	VtAccount* _CurAccount = nullptr;
	std::mutex m_;
	std::mutex _OrderMutex;
	/// <summary>
	/// ü��� �ֹ��� ó����.
	/// </summary>
	/// <param name="filledList"></param>
	/// <param name="curOrder"></param>
	void HandleFilledOrder(std::list<VtOrder*>* filledList, VtOrder* curOrder);
	/// <summary>
	/// �ܰ� ������.
	/// </summary>
	/// <param name="curOrder"></param>
	void UpdateAccountRemain(VtOrder* curOrder);
	/// <summary>
	/// ���� �ֹ��� ��Ʈ�� �߰���.
	/// </summary>
	/// <param name="order"></param>
	void AddPreOrderToChart(VtOrder* order);
	/// <summary>
	/// ����Ȯ�ε� �ֹ��� ��Ʈ�� �߰���.
	/// </summary>
	/// <param name="order"></param>
	void AddAcceptedOrderToChart(VtOrder* order);
	/// <summary>
	/// ü�� Ȯ�ε� �ֹ��� ��Ʈ�� �߰���.
	/// </summary>
	/// <param name="order"></param>
	void AddFilledOrderToChart(VtOrder* order);

	void FillRequest(VtOrderDirectRequest&& request, VtOrder* order);
	void RemoveOriginalOrder(int oldOrderNo);
public:
	void DirectPutOrder(VtOrderDirectRequest&& request);
	void DirectChangeOrder(VtOrder* oldOrder, std::string newValue);
	void DirectCancelOrder(VtOrder* oldOrder);

	/// <summary>
	/// �ֹ��ʿ��� ��� �ֹ��� �����Ѵ�.
	/// �޸� ������ �Ͼ��.
	/// </summary>
	void ClearOrderMap();
	/// <summary>
	/// ü�� ��Ͽ��� ��� �ֹ��� �����Ѵ�.
	/// �޸� ������ �Ͼ��.
	/// </summary>
	void ClearFilledList();
	void ClearAcceptedList();
	/// <summary>
	/// ���� �ֹ��� ��������ٴ� �̺�Ʈ
	/// </summary>
	BasicEvent<VtOrderEventArgs> _PreOrderCreatedEvent;
	void OnPreOrderCreated(VtOrderEventArgs&& arg)
	{
		_PreOrderCreatedEvent(this, arg);
	}
	/// <summary>
	/// ���� �ֹ��� ��������ٴ� �̺�Ʈ�� ������.
	/// </summary>
	/// <param name="pOrder"></param>
	void SendPreOrderEvent(VtOrder* pOrder);
	/// <summary>
	/// ���� �������� �ֹ� ������ �޾��� �� ó��
	/// </summary>
	/// <param name="rcvdOrder"></param>
	void OnOrderReceived(VtOrder* rcvdOrder);
	/// <summary>
	/// �������� ���Ž� ���� �޽��� ó��
	/// </summary>
	/// <param name="errorMsg"></param>
	void OnError(std::string errorMsg);
	/// <summary>
	/// ���� ���� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnLedger(VtRealtimeOrder&& real);
	/// <summary>
	/// ����Ȯ�� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnAccepted(VtRealtimeOrder&& real);
	/// <summary>
	/// ü�� Ȯ�� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnFilled(VtRealtimeOrder&& real);
	/// <summary>
	/// ����Ȯ�� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmModify(VtRealtimeOrder&& real);
	/// <summary>
	/// ���Ȯ�� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmCancel(VtRealtimeOrder&& real);
	/// <summary>
	/// �ű� �ֹ� �ź� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnRejectNew(VtRealtimeOrder&& real);
	/// <summary>
	/// ���� �Ź� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnRejectModify(VtRealtimeOrder&& real);
	/// <summary>
	/// ��� �ź� �޽��� ó��
	/// </summary>
	/// <param name="real"></param>
	void OnRejectCancel(VtRealtimeOrder&& real);

	void OnOrderReceivedHd(VtOrder* order);
	void OnOrderAcceptedHd(VtOrder* order);
	void OnOrderUnfilledHd(VtOrder* order);
	void OnOrderFilledHd(VtOrder* order);

	void OnReceiveQuoteHd(VtSymbol* sym);
	std::string AccountNo;
	int AccountLevel;
	void OnRemain(VtPosition&& curPosi);
	void CalcTotalProfitLoss(VtOrder* order);
	void CalcTotalProfitLoss(VtSymbol* symbol);
	int GetAcceptedCount();
	bool GetInit();
public:
	//VtProductOrderManagerSelector* ProductOrderManagerSelector() const { return _ProductOrderManagerSelector; }
	//void ProductOrderManagerSelector(VtProductOrderManagerSelector* val) { _ProductOrderManagerSelector = val; }
	VtProductOrderManager* GetProductOrderManager(std::string symbolCode);
	VtProductOrderManager* FindAddProductOrderManager(std::string symbolCode);
	virtual std::map<int, VtOrder*> GetTotalRemain(std::string symbolCode);
	virtual void ClearRemainOrderMap(std::string symbolCode);
	int GetAvg(std::string symbolCode);
	void GetRemainMap(std::string symbolCode, std::map<int, VtOrder*>& remainMap);
private:
	VtProductOrderManagerSelector* _ProductOrderManagerSelector = nullptr;
	void HandleReverseConfirmModify(VtRealtimeOrder&& real);

public:
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
//private:
//	std::map<std::string, VtProductOrderManager*> _PrdtOrderMgrMap;
};


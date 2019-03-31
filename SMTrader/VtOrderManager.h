#pragma once

/// <summary>
/// 주문 동작 방식
/// 모든 주문은 예약주문에서 시작된다.
/// 주문이 접수되면 예약주문에서 제거되고 
/// 주문목록으로 옮겨진다. 
/// 그리고 미체결 또는 체결목록에 등록된다.
/// 그리고 주문객체에 대한 메모리 해제는 
/// 본주문 목록에서만 실행이 된다.
/// 다만 예약 주문은 응답이 오지 않았을 경우 
/// 그대로 남아 있기 때문에 
/// 처리되지 않은 예약 주문에 대해서는 예약 주문 목록이 처리한다.
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
/// 주문을 관리하는 클래스
/// 주문은 크게 예약주문과 일반주문으로 나뉜다.
/// 예약 주문은 주문 객체를 만들어 놓고 실시간 시세가 예약 주문 가격에 일치하면 
/// 지정가 혹은 시장가로 주문을 낸다.
/// 일반주문을 주문을 만들어 예약 주문에 넣어 놓고 
/// 서버에서 주문이 만들어지면 예약 주문 목록에서 찾아서 없애주고
/// 주문목록에 넣어준다.
/// 주문방식은 다음과 같은 순서로 이루어진다.
/// 1. 클라이언트 주문 생성 
/// 2. 클라이언트 예약 주문 목록에 추가
/// 3. 서버로 주문전송
/// 4. 서버로부터 주문번호를 포함함 주문 정보 수신
/// 5. 예약 주문 목록에서 찾아서 주문을 없애주고 주문 목록에 추가
/// 6. 서버로 부터 원장 등록메시지 수신
/// 7. 서버로부터 접수확인 메시지 수신
/// 8. 서버로부터 체결 확인 메시지 수신.
/// 수정이나 취소 주문도 동일하게 위 과정을 거친다.
/// 다만 8번에서 수정확인, 취소확인, 혹은 수정거부, 취소거부 메시지를 수신한다.
/// </summary>
/// 
/// 중요 : 주문정보보다 체결이 먼저오는 경우가 생김 - 이 경우에 대비하여 프로그램을 수정해야 함.
class VtOrderManager 
{
public:
	/// <summary>
	/// 주문요청 사항의 항목들을 채운다.
	/// </summary>
	/// <param name="acnt"></param>
	void RequestFilledOrder(VtAccount* acnt);
	void RequestFilledOrder(std::string acntNo);
	void RequestFilledOrder(std::string accountNo, int Range, int OrderBy);
	void RequestOrderList(VtTaskAccountFilledArg* arg);
	void ReceiveFilledOrder();
	/// <summary>
	/// 주문요청 아이디를 만들어 낸다.
	/// </summary>
	/// <returns></returns>
	int GetOrderRequestID()
	{
		std::lock_guard<std::mutex> lock(m_);
		return VtUniqueOrderID::GetID();
	}
	/// <summary>
	/// 주문요청 아이디 생성기
	/// </summary>
	VtOrderManager();
	virtual ~VtOrderManager();
	/// <summary>
	/// 매수주문
	/// </summary>
	/// <param name="amount"></param>
	void Buy(int amount);
	/// <summary>
	/// 매도 주문
	/// </summary>
	/// <param name="amount"></param>
	void Sell(int amount);
	/// <summary>
	/// 주문요청객체와 함께 신규 주문 요청
	/// </summary>
	/// <param name="request"></param>
	void PutOrder(VtOrderRequest&& request);
	/// <summary>
	/// 기존주문으로 주문 수정 요청
	/// </summary>
	/// <param name="oldOrder"></param>
	/// <param name="newValue"></param>
	/// <param name="newQty"></param>
	void ChangeOrder(VtOrder* oldOrder, double newValue);
	/// <summary>
	/// 기존 주문으로 주문 취소 요청
	/// </summary>
	/// <param name="oldOrder"></param>
	void CancelOrder(VtOrder* oldOrder);
	/// <summary>
	/// 차트에서 주문 요청
	/// </summary>
	/// <param name="chartId"></param>
	/// <param name="cmd"></param>
	/// <param name="qty"></param>
	/// <param name="price"></param>
	/// <param name="symMstr"></param>
	void PutOrder(int chartId, VtOrderCmd cmd, int qty, double price, VtSymbolMaster* symMstr);

	/// <summary>
	/// 전체 주문 맵 : 키는 주문번호
	/// </summary>
	std::map<int, VtOrder*> OrderMap;
	/// <summary>
	/// 예약 주문 목록
	/// </summary>
	std::list<VtOrder*> PreOrderList;
	/// <summary>
	/// 접수확인 주문 목록
	/// </summary>
	//std::list<VtOrder*> AcceptedList;
	std::map<int, VtOrder*> AcceptedMap;
	/// <summary>
	/// 계좌에서 가져온 현재 체결과 미체결 목록
	/// 참조용으로 사용된다.
	/// 시스템이 다시 시작할 때 이것을 불러와 설정해 줘야 한다.
	/// 이목록을 불러올 때 FilledMap에 체결된 것은 넣어줘야 한다.
	/// 그래야 주문이 올바르게 처리된다.
	/// 그 주문은 참조가 아니라 복사되어야 한다.
	/// </summary>
	std::list<VtOrder*> FilledList;

	/// <summary>
	/// 사용자가 주문한 결과로 들어온 목록
	/// 주문 처리를 위해서 사용된다.
	/// </summary>
	std::map<std::string, std::list<VtOrder*>*> FilledMap;
	/// <summary>
	/// 주문목록에 주문 추가
	/// </summary>
	/// <param name="order"></param>
	void AddOrder(VtOrder* order);
	/// <summary>
	/// 예약 주문 목록에 주문 추가
	/// </summary>
	/// <param name="order"></param>
	void AddPreOrder(VtOrder* order);
	/// <summary>
	/// 접수확인 주문 목록에 주문 추가
	/// </summary>
	/// <param name="order"></param>
	void AddAccepted(VtOrder* order);
	/// <summary>
	/// 체결 주문 목록에 주문 추가
	/// </summary>
	/// <param name="order"></param>
	void AddFilled(VtOrder* order);
	/// <summary>
	/// 예약 주문 목록에서 주문삭제
	/// </summary>
	/// <param name="orderRequestID"></param>
	/// <returns></returns>
	VtOrder* RemovePreOrder(int orderRequestID);
	/// <summary>
	/// 접수확인 주문목록에서 주문 삭제
	/// </summary>
	/// <param name="orderNo"></param>
	void RemoveAccepted(int orderNo);
	/// <summary>
	/// 체결 주문 목록에서 주문 삭제
	/// </summary>
	/// <param name="orderNo"></param>
	void RemoveFilled(int orderNo);
	/// <summary>
	/// 주문 요청 번호로 주문 객체 검색
	/// </summary>
	/// <param name="requestID"></param>
	/// <returns></returns>
	VtOrder* FindPreOrderByRequestID(int requestID);
	/// <summary>
	/// 접수확인 목록에서 주문 검색
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindAccepted(int orderNo);
	/// <summary>
	/// 체결 주문에서 주문 검색
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindFilled(int orderNo);
	/// <summary>
	/// 주문 목록에서 주문 검색
	/// </summary>
	/// <param name="orderNo"></param>
	/// <returns></returns>
	VtOrder* FindOrder(int orderNo);
	/// <summary>
	/// 주문 복사
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	VtOrder* CloneOrder(VtOrder* src);
	void CopyOrder(VtOrder* src, VtOrder* dst);
	/// <summary>
	/// 기존 주문을 이용하여 주문요청 정보 설정
	/// </summary>
	/// <param name="request"></param>
	/// <param name="order"></param>
	void FillRequest(VtOrderRequest&& request, VtOrder* order);
	/// <summary>
	/// 주문 정보 전송
	/// </summary>
	/// <param name="order"></param>
	/// <param name="real"></param>
	void SetOrderInfo(VtOrder* order, VtRealtimeOrder&& real);
	/// <summary>
	/// 현재 주문관리자의 계좌 객체
	/// </summary>
	/// <returns></returns>
	VtAccount* CurAccount() const { return _CurAccount; }
	void CurAccount(VtAccount* val) { _CurAccount = val; }
	/// <summary>
	/// 서버에서 최초로 미체결 목록을 받을 때 주문을 맵에 추가하는 함수
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
	/// 현재 주문관리자의 계좌 객체
	/// 주문 관리자는 오직 하나의 계좌만을 다룬다.
	/// 또 다른 계좌가 있을 경우 
	/// 그 계좌는 다른 주문 관리자를 가진다.
	/// 계좌마다 고유의 주문 관리자를 가지게 된다.
	/// </summary>
	VtAccount* _CurAccount = nullptr;
	std::mutex m_;
	std::mutex _OrderMutex;
	/// <summary>
	/// 체결된 주문을 처리함.
	/// </summary>
	/// <param name="filledList"></param>
	/// <param name="curOrder"></param>
	void HandleFilledOrder(std::list<VtOrder*>* filledList, VtOrder* curOrder);
	/// <summary>
	/// 잔고를 갱신함.
	/// </summary>
	/// <param name="curOrder"></param>
	void UpdateAccountRemain(VtOrder* curOrder);
	/// <summary>
	/// 예약 주문을 차트에 추가함.
	/// </summary>
	/// <param name="order"></param>
	void AddPreOrderToChart(VtOrder* order);
	/// <summary>
	/// 접수확인된 주문을 차트에 추가함.
	/// </summary>
	/// <param name="order"></param>
	void AddAcceptedOrderToChart(VtOrder* order);
	/// <summary>
	/// 체결 확인된 주문을 차트에 추가함.
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
	/// 주문맵에서 모든 주문을 삭제한다.
	/// 메모리 해제가 일어난다.
	/// </summary>
	void ClearOrderMap();
	/// <summary>
	/// 체결 목록에서 모든 주문을 삭제한다.
	/// 메모리 해제가 일어난다.
	/// </summary>
	void ClearFilledList();
	void ClearAcceptedList();
	/// <summary>
	/// 예약 주문이 만들어졌다는 이벤트
	/// </summary>
	BasicEvent<VtOrderEventArgs> _PreOrderCreatedEvent;
	void OnPreOrderCreated(VtOrderEventArgs&& arg)
	{
		_PreOrderCreatedEvent(this, arg);
	}
	/// <summary>
	/// 예약 주문이 만들어졌다는 이벤트를 보낸다.
	/// </summary>
	/// <param name="pOrder"></param>
	void SendPreOrderEvent(VtOrder* pOrder);
	/// <summary>
	/// 최초 서버에서 주문 정보를 받았을 때 처리
	/// </summary>
	/// <param name="rcvdOrder"></param>
	void OnOrderReceived(VtOrder* rcvdOrder);
	/// <summary>
	/// 서버에서 수신시 오류 메시지 처리
	/// </summary>
	/// <param name="errorMsg"></param>
	void OnError(std::string errorMsg);
	/// <summary>
	/// 원장 접수 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnLedger(VtRealtimeOrder&& real);
	/// <summary>
	/// 접수확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnAccepted(VtRealtimeOrder&& real);
	/// <summary>
	/// 체결 확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnFilled(VtRealtimeOrder&& real);
	/// <summary>
	/// 수정확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmModify(VtRealtimeOrder&& real);
	/// <summary>
	/// 취소확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmCancel(VtRealtimeOrder&& real);
	/// <summary>
	/// 신규 주문 거부 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnRejectNew(VtRealtimeOrder&& real);
	/// <summary>
	/// 수정 거버 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnRejectModify(VtRealtimeOrder&& real);
	/// <summary>
	/// 취소 거부 메시지 처리
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


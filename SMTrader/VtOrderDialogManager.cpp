#include "stdafx.h"
#include "VtOrderDialogManager.h"
#include "VtOrderWnd.h"
#include "VtQuote.h"
#include "VtHoga.h"
#include "VtOrder.h"
#include "Poco/String.h"
#include "VtSymbolMaster.h"
#include "VtAccount.h"
#include "VtSymbol.h"
#include "VtOrderWndHd.h"
#include "MainFrm.h"
#include "resource.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "VtSubAccountManager.h"
#include "VtAccountManager.h"
#include "VtGlobal.h"

using Poco::trim;

/*
2017년 11월 15일
1. 주문창 관리자 추가
2. 실시간 시세 이벤트 객체 추가
3. 시세관리자와 주문창 관리자를 이벤트로 연결 시킴
*/

VtOrderDialogManager::VtOrderDialogManager()
{
	_MainFrm = nullptr;
	Begin();
}


VtOrderDialogManager::~VtOrderDialogManager()
{
	End();
}

void VtOrderDialogManager::OnRealtimeQuoteReceived(VtRealtimeQuoteEventArgs& arg)
{
	VtRealtimeQuote* pQuote = arg.pQuote;
	if (!pQuote) return;

	std::string fullCode;
	fullCode.assign(pQuote->FullCode, sizeof(pQuote->FullCode));
	fullCode = trim(fullCode);
}

void VtOrderDialogManager::OnOrderWindowEventReceived(VtOrderWndEventArgs& arg)
{
	if (arg.type == VtOrderWindowEventType::Created)
		AddOrderWindow(arg.pOrderWnd);
	else if (arg.type == VtOrderWindowEventType::Closed)
	{
		RemoveOrderWindow(arg.pOrderWnd);
		arg.pOrderWnd->DestroyWindow();
		delete arg.pOrderWnd;
	}
}

void VtOrderDialogManager::OnOrderWndEventReceived(VtOrderWndHdEventArgs& arg)
{
	if (arg.type == VtOrderWindowEventType::Created)
		AddOrderWnd(arg.pOrderWnd);
	else if (arg.type == VtOrderWindowEventType::Closed)
	{
		RemoveOrderWnd(arg.pOrderWnd);
		arg.pOrderWnd->DestroyWindow();
		delete arg.pOrderWnd;
	}
}

void VtOrderDialogManager::AddOrderWindow(VtOrderWnd* wnd)
{
	_OrderWindowMap[wnd] = wnd;
}

void VtOrderDialogManager::RemoveOrderWindow(VtOrderWnd* wnd)
{
	auto it = _OrderWindowMap.find(wnd);
	if (it != _OrderWindowMap.end())
	{
		_OrderWindowMap.erase(it);
	}
}

void VtOrderDialogManager::AddSymbolReceiverWindow(std::string fullCode, CVtOrderWnd* wnd)
{
	auto it = SymbolDispatcherMap.find(fullCode);
	if (it != SymbolDispatcherMap.end())
	{
		VtSymbolEventDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtSymbolEventDispatcher* sdp = new VtSymbolEventDispatcher();
		sdp->WndMap[wnd] = wnd;
		SymbolDispatcherMap[fullCode] = sdp;
	}
}

void VtOrderDialogManager::RemoveSymbolReceiveWindow(std::string fullCode, CVtOrderWnd* wnd)
{
	auto it = SymbolDispatcherMap.find(fullCode);
	if (it != SymbolDispatcherMap.end())
	{
		VtSymbolEventDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtOrderDialogManager::ChangeSymbolReceiverWindow(std::string oldCode, std::string newCode, CVtOrderWnd* wnd)
{
	auto it = SymbolDispatcherMap.find(oldCode);
	if (it != SymbolDispatcherMap.end())
	{
		VtSymbolEventDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}

		AddSymbolReceiverWindow(newCode, wnd);
	}
}

void VtOrderDialogManager::AddAccountReceiverWindow(std::string acntNo, CVtOrderWnd* wnd)
{
	auto it = AccountDispatcherMap.find(acntNo);
	if (it != AccountDispatcherMap.end())
	{
		VtAccountEventDispatcher* adp = it->second;
		adp->WndMap[wnd] = wnd;
	}
	else
	{
		VtAccountEventDispatcher* adp = new VtAccountEventDispatcher();
		adp->WndMap[wnd] = wnd;
		AccountDispatcherMap[acntNo] = adp;
	}
}

void VtOrderDialogManager::ChangeAccountReceiverWindow(std::string oldAcntNo, std::string newAcntNo, CVtOrderWnd* wnd)
{
	auto it = AccountDispatcherMap.find(oldAcntNo);
	if (it != AccountDispatcherMap.end())
	{
		VtAccountEventDispatcher* adp = it->second;
		auto innerIt = adp->WndMap.find(wnd);
		if (innerIt != adp->WndMap.end())
		{
			adp->WndMap.erase(wnd);
		}

		AddAccountReceiverWindow(newAcntNo, wnd);
	}
}

void VtOrderDialogManager::RemoveAccountReceiveWindow(std::string acntNo, CVtOrderWnd* wnd)
{
	auto it = AccountDispatcherMap.find(acntNo);
	if (it != AccountDispatcherMap.end())
	{
		VtAccountEventDispatcher* adp = it->second;
		auto innerIt = adp->WndMap.find(wnd);
		if (innerIt != adp->WndMap.end())
		{
			adp->WndMap.erase(wnd);
		}
	}
}

void VtOrderDialogManager::OnReceiveRealtimeHoga(VtHoga* hoga)
{
// 	auto it = SymbolDispatcherMap.find(hoga->FullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveRealtimeHoga(hoga);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveRealtimeHoga(hoga);
	}
	*/
}

void VtOrderDialogManager::OnReceiveHoga(VtHoga* hoga)
{
// 	auto it = SymbolDispatcherMap.find(hoga->FullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveHoga(hoga);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveHoga(hoga);
	}
	*/
}

void VtOrderDialogManager::OnOrderReceived(VtOrder* order)
{
// 	auto it = AccountDispatcherMap.find(order->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnOrderReceived(order);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnOrderReceived(order);
	}
	*/
}

void VtOrderDialogManager::OnAccepted(VtOrder* order)
{
// 	auto it = AccountDispatcherMap.find(order->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnAccepted(order);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnAccepted(order);
	}
	*/
}

void VtOrderDialogManager::OnFilled(VtOrder* order)
{
// 	auto it = AccountDispatcherMap.find(order->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnFilled(order);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnFilled(order);
	}
	*/
}

void VtOrderDialogManager::OnConfirmModify(VtOrder* order)
{
// 	auto it = AccountDispatcherMap.find(order->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnConfirmModify(order);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnConfirmModify(order);
	}
	*/
}

void VtOrderDialogManager::OnConfirmCancel(VtOrder* order)
{
// 	auto it = AccountDispatcherMap.find(order->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnConfirmCancel(order);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnOrderReceived(order);
	}
	*/
}

void VtOrderDialogManager::OnReceiveRealtimeQuote(VtQuote* quote)
{
// 	auto it = SymbolDispatcherMap.find(quote->fullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveRealtimeQuote(quote);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveRealtimeQuote(quote);
	}
	*/
}

void VtOrderDialogManager::OnReceiveQuote(VtQuote* quote)
{
// 	auto it = SymbolDispatcherMap.find(quote->fullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveQuote(quote);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveQuote(quote);
	}
	*/
}


void VtOrderDialogManager::OnReceiveMsg(CString msg)
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it) {
		VtOrderWndHd* wnd = it->second;
		wnd->OnReceiveMsg(msg);
	}
}

void VtOrderDialogManager::OnReceiveMsgWithReqId(int id, CString msg)
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it) {
		VtOrderWndHd* wnd = it->second;
		wnd->OnReceiveMsgWithReqId(id, msg);
	}
}

void VtOrderDialogManager::OnOrderReceivedHd(VtOrder* order)
{
	if (!order)
		return;

	std::string key;
	if (order->Type == 0 || order->Type == 1) // 실계좌 혹은 서브계좌 주문 
	{
		if (order->Type == 0)
		{
			// 실계좌 혹은 서브계좌에 대한 주문 메시지 처리
			// 이 계좌가 속한 서브 계좌와 펀드에 메시지를 보내야 한다.
			key = order->AccountNo;
			key.append(_T(":"));
			key.append(order->shortCode);
			auto it = OrderMap.find(key);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderAcceptedHd(order);
				}
			}

			// For the relate subaccount
			VtAccountManager* acntMgr = VtAccountManager::GetInstance();
			VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
			std::vector<VtAccount*> subAcntList = acnt->GetSubAccountList();
			std::vector<std::string> keyList;
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
				if (subAcnt->Fund())
					keyList.push_back(subAcnt->Fund()->Name);
			}

			for (auto it = keyList.begin(); it != keyList.end(); ++it)
			{
				key = *it;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto ita = OrderMap.find(key);
				if (ita != OrderMap.end())
				{
					VtOrderDistributer* sdp = ita->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderAcceptedHd(order);
					}
				}
			}
		}

		// 서브 계좌에 속한 펀드 업데이트를 위한 메시지 처리 
		// 이 서브 계좌를 가지고 있는 펀드는 실시간으로 업데이트가 되어야 한다.
		// 서브 계좌는 자기가 속한 펀드가 있는지 확인하고 펀드가 있으면 
		// 그쪽으로 메시지를 보낸다.
		if (order->Type == 1)
		{
			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
			VtAccount* subAcnt = subAcntMgr->FindAccount(order->AccountNo);
			if (subAcnt && subAcnt->Fund())
			{
				VtFund* fund = subAcnt->Fund();
				key = fund->Name;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto it = OrderMap.find(key);
				if (it != OrderMap.end())
				{
					VtOrderDistributer* sdp = it->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderAcceptedHd(order);
					}
				}
			}
		}
	}
	else // 펀드 주문
	{
		key = order->FundName;

		std::vector<std::string> keyList;
		keyList.push_back(key);
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* fund = fundMgr->FindFund(key);
		if (fund)
		{
			// 펀드에 속한 모든 서브 계좌들을 가져온다.
			std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
			for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
			}
		}

		// 펀드이름과 그 하위 계좌번호로 이루어진 이름 목록으로 각 주문창에 메시지를 보낸다.
		for (auto its = keyList.begin(); its != keyList.end(); ++its)
		{
			std::string keyName = *its;
			keyName.append(_T(":"));
			keyName.append(order->shortCode);
			auto it = OrderMap.find(keyName);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderReceivedHd(order);
				}
			}
		}
	}

	// 좌측 창을 위한 메시지 전달
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnOrderAccepted(order);
	}
}

void VtOrderDialogManager::OnOrderAcceptedHd(VtOrder* order)
{
	if (!order)
		return;

	std::string key;
	if (order->Type == 0 || order->Type == 1) // 실계좌 혹은 서브계좌 주문 
	{
		if (order->Type == 0)
		{
			// 실계좌 혹은 서브계좌에 대한 주문 메시지 처리
			// 이 계좌가 속한 서브 계좌와 펀드에 메시지를 보내야 한다.
			key = order->AccountNo;
			key.append(_T(":"));
			key.append(order->shortCode);
			auto it = OrderMap.find(key);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderAcceptedHd(order);
				}
			}

			// For the relate subaccount
			VtAccountManager* acntMgr = VtAccountManager::GetInstance();
			VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
			std::vector<VtAccount*> subAcntList = acnt->GetSubAccountList();
			std::vector<std::string> keyList;
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
				if (subAcnt->Fund())
					keyList.push_back(subAcnt->Fund()->Name);
			}

			for (auto it = keyList.begin(); it != keyList.end(); ++it)
			{
				key = *it;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto ita = OrderMap.find(key);
				if (ita != OrderMap.end())
				{
					VtOrderDistributer* sdp = ita->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderAcceptedHd(order);
					}
				}
			}
		}

		// 서브 계좌에 속한 펀드 업데이트를 위한 메시지 처리 
		// 이 서브 계좌를 가지고 있는 펀드는 실시간으로 업데이트가 되어야 한다.
		// 서브 계좌는 자기가 속한 펀드가 있는지 확인하고 펀드가 있으면 
		// 그쪽으로 메시지를 보낸다.
		if (order->Type == 1)
		{
			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
			VtAccount* subAcnt = subAcntMgr->FindAccount(order->AccountNo);
			if (subAcnt && subAcnt->Fund())
			{
				VtFund* fund = subAcnt->Fund();
				key = fund->Name;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto it = OrderMap.find(key);
				if (it != OrderMap.end())
				{
					VtOrderDistributer* sdp = it->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderAcceptedHd(order);
					}
				}
			}
		}
	}
	else // 펀드 주문
	{
		key = order->FundName;

		std::vector<std::string> keyList;
		keyList.push_back(key);
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* fund = fundMgr->FindFund(key);
		if (fund)
		{
			// 펀드에 속한 모든 서브 계좌들을 가져온다.
			std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
			for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
			}
		}

		// 펀드이름과 그 하위 계좌번호로 이루어진 이름 목록으로 각 주문창에 메시지를 보낸다.
		for (auto its = keyList.begin(); its != keyList.end(); ++its)
		{
			std::string keyName = *its;
			keyName.append(_T(":"));
			keyName.append(order->shortCode);
			auto it = OrderMap.find(keyName);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderAcceptedHd(order);
				}
			}
		}
	}
	

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnOrderAccepted(order);
	}
}

void VtOrderDialogManager::OnOrderUnfilledHd(VtOrder* order)
{
	/*
	std::string key = order->accountNo;
	key.append(_T(":"));
	key.append(order->shortCode);
	auto it = OrderMap.find(key);
	if (it != OrderMap.end())
	{
		VtOrderDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnOrderUnfilledHd(order);
		}
	}
	*/
	std::string key;
	if (order->Type == 0 || order->Type == 1) // 실계좌 혹은 서브계좌 주문 
	{
		if (order->Type == 0)
		{
			// 실계좌 혹은 서브계좌에 대한 주문 메시지 처리
			// 이 계좌가 속한 서브 계좌와 펀드에 메시지를 보내야 한다.
			key = order->AccountNo;
			key.append(_T(":"));
			key.append(order->shortCode);
			auto it = OrderMap.find(key);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderUnfilledHd(order);
				}
			}

			// For the relate subaccount
			VtAccountManager* acntMgr = VtAccountManager::GetInstance();
			VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
			std::vector<VtAccount*> subAcntList = acnt->GetSubAccountList();
			std::vector<std::string> keyList;
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
				if (subAcnt->Fund())
					keyList.push_back(subAcnt->Fund()->Name);
			}

			for (auto it = keyList.begin(); it != keyList.end(); ++it)
			{
				key = *it;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto ita = OrderMap.find(key);
				if (ita != OrderMap.end())
				{
					VtOrderDistributer* sdp = ita->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderUnfilledHd(order);
					}
				}
			}
		}

		// 서브 계좌에 속한 펀드 업데이트를 위한 메시지 처리 
		// 이 서브 계좌를 가지고 있는 펀드는 실시간으로 업데이트가 되어야 한다.
		// 서브 계좌는 자기가 속한 펀드가 있는지 확인하고 펀드가 있으면 
		// 그쪽으로 메시지를 보낸다.
		if (order->Type == 1)
		{
			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
			VtAccount* subAcnt = subAcntMgr->FindAccount(order->AccountNo);
			if (subAcnt && subAcnt->Fund())
			{
				VtFund* fund = subAcnt->Fund();
				key = fund->Name;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto it = OrderMap.find(key);
				if (it != OrderMap.end())
				{
					VtOrderDistributer* sdp = it->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderUnfilledHd(order);
					}
				}
			}
		}
	}
	else // 펀드 주문
	{
		key = order->FundName;

		std::vector<std::string> keyList;
		keyList.push_back(key);
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* fund = fundMgr->FindFund(key);
		if (fund)
		{
			// 펀드에 속한 모든 서브 계좌들을 가져온다.
			std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
			for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
			}
		}

		// 펀드이름과 그 하위 계좌번호로 이루어진 이름 목록으로 각 주문창에 메시지를 보낸다.
		for (auto its = keyList.begin(); its != keyList.end(); ++its)
		{
			std::string keyName = *its;
			keyName.append(_T(":"));
			keyName.append(order->shortCode);
			auto it = OrderMap.find(keyName);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderUnfilledHd(order);
				}
			}
		}
	}

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnOrderUnfilled(order);
	}
}

void VtOrderDialogManager::OnOrderFilledHd(VtOrder* order)
{
	/*
	std::string key; 
	if (order->type == 0)
		key = order->accountNo;
	else
		key = order->FundName;

	key.append(_T(":"));
	key.append(order->shortCode);
	auto it = OrderMap.find(key);
	if (it != OrderMap.end())
	{
		VtOrderDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnOrderFilledHd(order);
		}
	}
	*/

	std::string key;
	if (order->Type == 0 || order->Type == 1) // 실계좌 혹은 서브계좌 주문 
	{
		if (order->Type == 0)
		{
			// 실계좌 혹은 서브계좌에 대한 주문 메시지 처리
			// 이 계좌가 속한 서브 계좌와 펀드에 메시지를 보내야 한다.
			key = order->AccountNo;
			key.append(_T(":"));
			key.append(order->shortCode);
			auto it = OrderMap.find(key);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderFilledHd(order);
				}
			}

			// For the relate subaccount
			VtAccountManager* acntMgr = VtAccountManager::GetInstance();
			VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
			std::vector<VtAccount*> subAcntList = acnt->GetSubAccountList();
			std::vector<std::string> keyList;
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
				if (subAcnt->Fund())
					keyList.push_back(subAcnt->Fund()->Name);
			}

			for (auto it = keyList.begin(); it != keyList.end(); ++it)
			{
				key = *it;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto ita = OrderMap.find(key);
				if (ita != OrderMap.end())
				{
					VtOrderDistributer* sdp = ita->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderFilledHd(order);
					}
				}
			}
		}

		// 서브 계좌에 속한 펀드 업데이트를 위한 메시지 처리 
		// 이 서브 계좌를 가지고 있는 펀드는 실시간으로 업데이트가 되어야 한다.
		// 서브 계좌는 자기가 속한 펀드가 있는지 확인하고 펀드가 있으면 
		// 그쪽으로 메시지를 보낸다.
		if (order->Type == 1)
		{
			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
			VtAccount* subAcnt = subAcntMgr->FindAccount(order->AccountNo);
			// 펀드가 있는 경우와 없는 경우를 나누어서 처리해야 한다.
			if (subAcnt)
			{
				VtFund* fund = subAcnt->Fund();
				key = fund->Name;
				key.append(_T(":"));
				key.append(order->shortCode);
				auto it = OrderMap.find(key);
				if (it != OrderMap.end())
				{
					VtOrderDistributer* sdp = it->second;
					for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
					{
						VtOrderCenterWndHd* wnd = its->second;
						wnd->OnOrderFilledHd(order);
					}
				}
			}
		}
	}
	else // 펀드 주문
	{
		key = order->FundName;

		std::vector<std::string> keyList;
		keyList.push_back(key);
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* fund = fundMgr->FindFund(key);
		if (fund)
		{
			// 펀드에 속한 모든 서브 계좌들을 가져온다.
			std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
			for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				keyList.push_back(subAcnt->AccountNo);
			}
		}

		// 펀드이름과 그 하위 계좌번호로 이루어진 이름 목록으로 각 주문창에 메시지를 보낸다.
		for (auto its = keyList.begin(); its != keyList.end(); ++its)
		{
			std::string keyName = *its;
			keyName.append(_T(":"));
			keyName.append(order->shortCode);
			auto it = OrderMap.find(keyName);
			if (it != OrderMap.end())
			{
				VtOrderDistributer* sdp = it->second;
				for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
				{
					VtOrderCenterWndHd* wnd = its->second;
					wnd->OnOrderFilledHd(order);
				}
			}
		}
	}
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnOrderFilled(order);
	}
}

void VtOrderDialogManager::OnRemain(VtPosition* posi)
{
	if (!posi)
		return;

	std::string key = posi->AccountNo;
	key.append(_T(":"));
	key.append(posi->ShortCode);
	auto it = OrderMap.find(key);
	if (it != OrderMap.end())
	{
		VtOrderDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnRemain(posi);
		}
	}

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnRemain(posi);
	}
}

void VtOrderDialogManager::OnReceiveAccountInfo()
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnReceiveAccountInfo();
	}
}

void VtOrderDialogManager::OnOutstanding()
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
	}
}

void VtOrderDialogManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	CRect rcWnd;

	if (_MainFrm) {
		_MainFrm->GetWindowRect(rcWnd);
	}
	// 메인 윈도 전체 크기 저장
	ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
	ss << (int)_OrderWndMap.size();
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it) {
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->Save(ss);
	}
}

void VtOrderDialogManager::Load(simple::file_istream<same_endian_type>& ss)
{
	CRect rcWnd;
	ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;
	if (_MainFrm && rcWnd.left != 0 && rcWnd.top != 0) {
		//_MainFrm->MoveWindow(rcWnd);
		int width = VtGlobal::GetHorWidthByScaleFactor(376);
		int height = VtGlobal::GetVerHeightByScaleFactor(145);
		_MainFrm->SetWindowPos(nullptr, rcWnd.left, rcWnd.top, width, height, SWP_NOZORDER);
	}
	int count;
	ss >> count;
	VtOrderWndHd* orderWnd = nullptr;
	for (int i = 0; i < count; ++i) {
		orderWnd = new VtOrderWndHd();
		orderWnd->Load(ss);
		CreateOrderDialog(orderWnd);
		orderWnd->SetDefaultCenterWnd();
	}
}

VtOrderWndHd* VtOrderDialogManager::CreateOrderDialog()
{
	if (!_MainFrm)
		return nullptr;

	VtOrderWndHd* pOrderDlg = new VtOrderWndHd((CWnd*)_MainFrm);
	//pOrderDlg->CenterWndCount(0);
	pOrderDlg->Create(IDD_ORDER_WND_HD, (CWnd*)_MainFrm);
	pOrderDlg->ShowWindow(SW_SHOW);

	return pOrderDlg;
}

void VtOrderDialogManager::CreateOrderDialog(VtOrderWndHd* orderWnd)
{
	if (!orderWnd)
		return;

	orderWnd->Create(IDD_ORDER_WND_HD, (CWnd*)_MainFrm);
	orderWnd->ShowWindow(SW_SHOW);
}

void VtOrderDialogManager::OnFundAdded()
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnFundAdded();
	}
}

void VtOrderDialogManager::OnFundDeleted(VtFund* fund)
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		orderWnd->OnFundDeleted(fund);
	}
}

void VtOrderDialogManager::SendOrderMessage(VtOrderEvent event, VtOrder* order)
{
	if (!order)
		return;
	/*
	CString msg;
	msg.Format(_T("OrderMap count = %d :::\n"), OrderMap.size());
	TRACE(msg);
	for (auto it = OrderMap.begin(); it != OrderMap.end(); ++it)
	{
		VtOrderDistributer* sdp = it->second;
		msg.Format(_T("parent key = %s , size = %d\n"), it->first.c_str(), sdp->WndMap.size());
		TRACE(msg);
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			msg.Format(_T("centerWnd key = %x :::\n"), its->first);
			TRACE(msg);
		}
	}
	*/

	std::string key;
	std::vector<std::string> keyList;
	keyList.push_back(order->AccountNo);
	if (order->Type == 0) { // 실계좌
		if (order->SubAccountNo.length() > 0)
			keyList.push_back(order->SubAccountNo);
		if (order->FundName.length() > 0)
			keyList.push_back(order->FundName);
	} else { // 서브계좌나 펀드 주문
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		VtAccount* subAcnt = subAcntMgr->FindAccount(order->AccountNo);
		if (subAcnt && subAcnt->Fund())
			keyList.push_back(subAcnt->Fund()->Name);
	}

	for (auto it = keyList.begin(); it != keyList.end(); ++it) {
		key = *it;
		key.append(_T(":"));
		key.append(order->shortCode);
		auto ita = OrderMap.find(key);
		
		if (ita != OrderMap.end()) {
			VtOrderDistributer* sdp = ita->second;
			for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its) {
				VtOrderCenterWndHd* wnd = its->second;
				switch (event) {
				case VtOrderEvent::PutNew:
				case VtOrderEvent::Modified:
				case VtOrderEvent::Cancelled: {
					wnd->OnOrderReceivedHd(order);
				}
				break;
				case VtOrderEvent::Accepted: {
					wnd->OnOrderAcceptedHd(order);
				}
				break;
				case VtOrderEvent::Unfilled: {
					wnd->OnOrderUnfilledHd(order);
				}
				break;
				case VtOrderEvent::Filled: {
					wnd->OnOrderFilledHd(order);
				}
				break;
				default:
					break;
				}
			}
		}
	}

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it) {
		VtOrderWndHd* orderWnd = it->second;
		switch (event) {
		case VtOrderEvent::Accepted: {
			orderWnd->OnOrderAccepted(order);
		}
		break;
		case VtOrderEvent::Unfilled: {
			orderWnd->OnOrderUnfilled(order);
		}
		break;
		case VtOrderEvent::Filled: {
			orderWnd->OnOrderFilled(order);
		}
		break;
		default:
			break;
		}
	}
}

bool VtOrderDialogManager::FindFund(VtFund* oldFund)
{
	if (!oldFund)
		return false;
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* orderWnd = it->second;
		VtFund* fund = orderWnd->GetCurrentFund();
		if (fund == oldFund)
			return true;
	}

	return false;
}

void VtOrderDialogManager::SaveToXml(pugi::xml_node& node)
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it) {
		VtOrderWndHd* orderWnd = it->second;
		pugi::xml_node order_window = node.append_child("order_window");
		orderWnd->SaveToXml(order_window);
	}
}

void VtOrderDialogManager::LoadFromXml(pugi::xml_node& order_window_list_node)
{
	for (pugi::xml_node order_window_node = order_window_list_node.child("order_window"); order_window_node; order_window_node = order_window_node.next_sibling("order_window")) {
		VtOrderWndHd* orderWnd = new VtOrderWndHd();
		orderWnd->LoadFromXml(order_window_node);
		CreateOrderDialog(orderWnd);
		orderWnd->SetDefaultCenterWnd();
	}
}

void VtOrderDialogManager::OnReceiveSymbolMaster(VtSymbolMaster* master)
{
// 	auto it = SymbolDispatcherMap.find(master->FullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveSymbolMaster(master);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveSymbolMaster(master);
	}
	*/
}

void VtOrderDialogManager::OnReceiveRealtimeSymbolMaster(VtSymbolMaster* master)
{
// 	auto it = SymbolDispatcherMap.find(master->FullCode);
// 	if (it != SymbolDispatcherMap.end())
// 	{
// 		VtSymbolEventDispatcher* sdp = it->second;
// 		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveRealtimeSymbolMaster(master);
// 		}
// 	}
	/*
	for (auto it = OrderWindowMap.begin(); it != OrderWindowMap.end(); ++it)
	{
		CVtOrderWnd* wnd = it->second;
		wnd->OnReceiveRealtimeSymbolMaster(master);
	}
	*/
}


void VtOrderDialogManager::OnReceiveAccountDeposit(VtAccount* acnt)
{
// 	auto it = AccountDispatcherMap.find(acnt->AccountNo);
// 	if (it != AccountDispatcherMap.end())
// 	{
// 		VtAccountEventDispatcher* adp = it->second;
// 		for (auto its = adp->WndMap.begin(); its != adp->WndMap.end(); ++its)
// 		{
// 			CVtOrderWnd* wnd = its->second;
// 			wnd->OnReceiveAccountDeposit(acnt);
// 		}
// 	}
}

void VtOrderDialogManager::AddOrderWnd(VtOrderWndHd* wnd)
{
	_OrderWndMap[wnd] = wnd;
}

void VtOrderDialogManager::AddOrderWnd(std::string key, VtOrderCenterWndHd* wnd)
{
	auto it = OrderMap.find(key);
	if (it != OrderMap.end())
	{
		VtOrderDistributer* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtOrderDistributer* sdp = new VtOrderDistributer();
		sdp->WndMap[wnd] = wnd;
		OrderMap[key] = sdp;
	}
}

void VtOrderDialogManager::RemoveOrderWnd(VtOrderWndHd* wnd)
{
	auto it = _OrderWndMap.find(wnd);
	if (it != _OrderWndMap.end())
	{
		_OrderWndMap.erase(it);
	}
}


void VtOrderDialogManager::RemoveOrderWnd(std::string key, VtOrderCenterWndHd* wnd)
{
	auto it = OrderMap.find(key);
	if (it != OrderMap.end())
	{
		VtOrderDistributer* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(innerIt);
		}
		if (sdp->WndMap.size() == 0)
		{
			delete sdp;
			OrderMap.erase(it);
		}
	}
}

void VtOrderDialogManager::AddSymbolWnd(std::string symCode, VtOrderCenterWndHd* wnd)
{
	auto it = SymbolMap.find(symCode);
	if (it != SymbolMap.end())
	{
		VtSymbolDistributer* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtSymbolDistributer* sdp = new VtSymbolDistributer();
		sdp->WndMap[wnd] = wnd;
		SymbolMap[symCode] = sdp;
	}
}

void VtOrderDialogManager::RemoveSymbolWnd(std::string symCode, VtOrderCenterWndHd* wnd)
{
	auto it = SymbolMap.find(symCode);
	if (it != SymbolMap.end())
	{
		VtSymbolDistributer* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtOrderDialogManager::AddAccountWnd(std::string accountNo, VtOrderCenterWndHd* wnd)
{
	auto it = AccountMap.find(accountNo);
	if (it != AccountMap.end())
	{
		VtAccountDistributer* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtAccountDistributer* sdp = new VtAccountDistributer();
		sdp->WndMap[wnd] = wnd;
		AccountMap[accountNo] = sdp;
	}
}

void VtOrderDialogManager::RemoveAccountWnd(std::string accountNo, VtOrderCenterWndHd* wnd)
{
	auto it = AccountMap.find(accountNo);
	if (it != AccountMap.end())
	{
		VtAccountDistributer* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtOrderDialogManager::AddSymbolWindow(std::string symCode, VtOrderWndHd* wnd)
{
	auto it = SymbolWndMap.find(symCode);
	if (it != SymbolWndMap.end())
	{
		VtSymbolDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtSymbolDispatcher* sdp = new VtSymbolDispatcher();
		sdp->WndMap[wnd] = wnd;
		SymbolWndMap[symCode] = sdp;
	}
}

void VtOrderDialogManager::RemoveSymbolWindow(std::string symCode, VtOrderWndHd* wnd)
{
	auto it = SymbolWndMap.find(symCode);
	if (it != SymbolWndMap.end())
	{
		VtSymbolDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtOrderDialogManager::Begin()
{
	_HandleOrderMap.insert(std::make_pair("New", &VtOrderDialogManager::OnOrderReceivedHd));
}

void VtOrderDialogManager::End()
{
	ReleaseSymbolWnd();
	ReleaseAccountWnd();
	ReleaseOrderWnd();
	ReleaseMainOrderWnd();
}

void VtOrderDialogManager::ReleaseSymbolWnd()
{
	for (auto it = SymbolMap.begin(); it != SymbolMap.end(); ++it)
	{
		delete it->second;
	}
	SymbolMap.clear();
}

void VtOrderDialogManager::ReleaseAccountWnd()
{
	for (auto it = AccountMap.begin(); it != AccountMap.end(); ++it)
	{
		delete it->second;
	}
	AccountMap.clear();
}

void VtOrderDialogManager::ReleaseOrderWnd()
{
	for (auto it = OrderMap.begin(); it != OrderMap.end(); ++it)
	{
		delete it->second;
	}
	OrderMap.clear();
}

void VtOrderDialogManager::ReleaseMainOrderWnd()
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* wnd = it->second;
		wnd->BlockEvent();
		wnd->DestroyWindow();
		delete wnd;
	}

	_OrderWndMap.clear();
}

void VtOrderDialogManager::OnExpected(VtSymbol* sym)
{
	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* wnd = it->second;
		wnd->OnExpected(sym);
	}
}

void VtOrderDialogManager::OnReceiveHoga(VtSymbol* sym)
{
	if (!sym)
		return;

	auto it = SymbolMap.find(sym->ShortCode);
	if (it != SymbolMap.end())
	{
		VtSymbolDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnReceiveHoga(sym);
		}
	}
}

void VtOrderDialogManager::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	auto it = SymbolMap.find(sym->ShortCode);
	if (it != SymbolMap.end())
	{
		VtSymbolDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnReceiveQuote(sym);
		}
	}

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* wnd = it->second;
		wnd->OnReceiveQuote(sym);
	}
}


void VtOrderDialogManager::CloseAllWindow()
{
	for (auto it = _OrderWindowMap.begin(); it != _OrderWindowMap.end(); ++it)
	{
		VtOrderWnd* wnd = it->second;
		if (wnd->GetSafeHwnd())
		{
			wnd->DestroyWindow();
		}
		delete wnd;
	}

	_OrderWindowMap.clear();
}

void VtOrderDialogManager::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;

	for (auto it = _OrderWndMap.begin(); it != _OrderWndMap.end(); ++it)
	{
		VtOrderWndHd* wnd = it->second;
		wnd->OnSymbolMaster(sym);
	}

	auto it = SymbolMap.find(sym->ShortCode);
	if (it != SymbolMap.end())
	{
		VtSymbolDistributer* sdp = it->second;
		for (auto its = sdp->WndMap.begin(); its != sdp->WndMap.end(); ++its)
		{
			VtOrderCenterWndHd* wnd = its->second;
			wnd->OnSymbolMaster(sym);
		}
	}
}

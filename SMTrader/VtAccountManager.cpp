#include "stdafx.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "MainFrm.h"
#include "Poco/Delegate.h"
#include "VtWindowManager.h"
#include "VtKrClient.h"
#include "RemainOrderWnd.h"
#include "VtOrderDialogManager.h"
#include "VtSubAccountManager.h"
#include "VtGlobal.h"

using Poco::Delegate;

VtAccountManager::VtAccountManager()
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	CMainFrame* pMainFrm = (CMainFrame*)wndMgr->FindWindow(_T("MainFrame")); //(CMainFrame*)AfxGetMainWnd();
	//_AccountListReceivedEvent += delegate(pMainFrm, &CMainFrame::OnAccountListReceived);
}


VtAccountManager::~VtAccountManager()
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	CMainFrame* pMainFrm = (CMainFrame*)wndMgr->FindWindow(_T("MainFrame"));
	//_AccountListReceivedEvent -= delegate(pMainFrm, &CMainFrame::OnAccountListReceived);

	for (auto it = AccountMap.begin(); it != AccountMap.end(); ++it)
	{
		delete it->second;
	}

	AccountMap.clear();

	
}

void VtAccountManager::AddAccount(VtAccount* acnt)
{
	AccountMap[acnt->AccountNo] = acnt;
}

VtAccount* VtAccountManager::FindAccount(std::string acntNo)
{
	auto it = AccountMap.find(acntNo);
	if (it != AccountMap.end())
	{
		return it->second;
	}
	else
		return nullptr;
}

VtAccount* VtAccountManager::FindAddAccount(std::string acntNo)
{
	auto it = AccountMap.find(acntNo);
	if (it != AccountMap.end())
	{
		return it->second;
	}
	else
	{
		VtAccount* acnt = new VtAccount();
		acnt->AccountNo = acntNo;
		AccountMap[acntNo] = acnt;
		return acnt;
	}
}

VtAccount* VtAccountManager::FindAccountByName(std::string acntName)
{
	for (auto it = AccountMap.begin(); it != AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		if (acnt->AccountName == acntName)
		{
			return acnt;
		}
	}

	return nullptr;
}

void VtAccountManager::SendEvent()
{
	VtAccountEventArgs arg;
	arg.pAcntMgr = this;
	FireAccountListReceived(std::move(arg));
}

void VtAccountManager::RequestProductRemain(std::string accountNo)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestAccountRemain(accountNo);
}

void VtAccountManager::RequestProductRemain()
{
	VtAccount* acnt = _ActiveAccount;
	if (acnt)
	{
		RequestProductRemain(acnt->AccountNo);
	}
}

void VtAccountManager::ReceivedProductRemain()
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	CRemainOrderWnd* remainWnd = (CRemainOrderWnd*)wndMgr->FindWindow(_T("Remain"));
	remainWnd->ReceivedRemain();
}

void VtAccountManager::RequestAccountDeposit(std::string accountNo)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestAccountDeposit(accountNo);
}

void VtAccountManager::RequestAccountDeposit()
{
	VtAccount* acnt = _ActiveAccount;
	if (acnt)
	{
		RequestAccountDeposit(acnt->AccountNo);
	}
}

void VtAccountManager::UpdateAccountRemain(VtOrder* order)
{
	if (!order)
		return;

	VtAccount* acnt = FindAddAccount(order->AccountNo);
	acnt->SumOpenPL();
}

void VtAccountManager::OnReceiveAccountDeposit(VtAccount* acnt)
{
	if (!acnt)
		return;
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveAccountDeposit(acnt);
}

void VtAccountManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << (int)AccountMap.size();
	for (auto it = AccountMap.begin(); it != AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		if (acnt->Enable())
			acnt->Save(ss);
	}
}

void VtAccountManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count;
	ss >> count;
	for (int i = 0; i < count; i++)
	{
		VtAccount* acnt = new VtAccount();
		acnt->Load(ss);

// 		if (acnt->GetSubAccountCount() == 0)
// 		{
// 			std::string acntNo = acnt->AccountNo;
// 			acntNo.append(_T("_1"));
// 			VtAccount* sub = acnt->CreateSubAccount(acntNo, acnt->AccountName, true);
// 			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
// 			subAcntMgr->AddAccount(sub);
// 		}

		AddAccount(acnt);
	}
}

void VtAccountManager::FileterAccount()
{
	VtGlobal* global = VtGlobal::GetInstance();

	// 실계좌에 없는 것들은 지워준다.
	for (auto it = AccountMap.cbegin(); it != AccountMap.cend(); )
	{
		auto key = global->AcntList.find(it->first);
		VtAccount* delAcnt = it->second;
		if (key == global->AcntList.end()) { // 실계좌에 없으면 지워준다.
			AccountMap.erase(it++);
			delete delAcnt;
		}
		else {
			delAcnt->Enable(true);
			++it;
		}
	}
}

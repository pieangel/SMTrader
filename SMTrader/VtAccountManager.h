#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include "VtAccountEvent.h"
#include "Poco/BasicEvent.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
using Poco::BasicEvent;

class VtAccount;
struct VtOrder;

struct VtAccountInfo {
	std::string account_no;
	std::string account_name;
	int account_type;
};

class VtAccountManager : public TemplateSingleton<VtAccountManager>
{
public:
	VtAccountManager();
	~VtAccountManager();

	std::map<std::string, VtAccount*>& GetAccountMap() {
		return AccountMap;
	}

	std::map<std::string, VtAccount*> AccountMap;
	void AddAccount(VtAccount* acnt);
	VtAccount* FindAccount(std::string acntNo);
	VtAccount* FindAddAccount(std::string acntNo);
	VtAccount* FindAccountByName(std::string acntName);

	BasicEvent<VtAccountEventArgs> _AccountListReceivedEvent;
	void FireAccountListReceived(VtAccountEventArgs&& arg)
	{
		_AccountListReceivedEvent(this, arg);
	}

	void SendEvent();

	VtAccount* ActiveAccount() const { return _ActiveAccount; }
	void ActiveAccount(VtAccount* val) { _ActiveAccount = val; }

	/// <summary>
	/// 계좌에 대한 상품별 잔고
	/// </summary>
	/// <param name="accountNo"></param>
	void RequestProductRemain(std::string accountNo);
	void RequestProductRemain();
	/// <summary>
	/// 계좌에 대한 상품별 잔고 수신
	/// </summary>
	void ReceivedProductRemain();
	/// <summary>
	/// 계좌에 대한 상황 - 예탁금 및 증거금
	/// </summary>
	/// <param name="acntNo"></param>
	void RequestAccountDeposit(std::string acntNo);
	void RequestAccountDeposit();
	void UpdateAccountRemain(VtOrder* order);
	void OnReceiveAccountDeposit(VtAccount* acnt);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void FileterAccount();
	std::vector<std::string> GetEmptyPasswordAccountList();
	std::set<std::string> ServerAccountSet;
	std::map<std::string, VtAccountInfo> ServerAccountMap;

	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);

private:
	VtAccount* _ActiveAccount = nullptr;
};


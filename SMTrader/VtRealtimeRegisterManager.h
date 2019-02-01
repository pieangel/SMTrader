#pragma once
#include "Global/TemplateSingleton.h"
#include<map>
struct VtRegisterItem
{
	std::string Name;
	int RefCount;
};
class VtRealtimeRegisterManager : public TemplateSingleton<VtRealtimeRegisterManager>
{
public:
	VtRealtimeRegisterManager();
	~VtRealtimeRegisterManager();

private:
	std::map<std::string, VtRegisterItem> _ProductMap;
	std::map<std::string, VtRegisterItem> _AccountMap;

	std::map<std::string, std::string> AccountMap;
	std::map<std::string, std::string> ProductMap;
	std::map<std::string, std::string> RecentProductMap;
	void AddAccount(std::string acnt);;
	void AddProduct(std::string prdt);
	void RemoveAccount(std::string acnt);
	void RemoveProduct(std::string prdt);
public:
	void RegisterRecentProduct(std::string prdt);
	void UnregisterRecentProduct(std::string prdt);
	void RegisterAccount(std::string acnt);
	void RegisterProduct(std::string prdt);
	void UnregisterAccount(std::string acnt);
	void UnregisterProduct(std::string prdt);
	void UnRegisterAll();
	void RegisterCurrent();
	void UnregisterCurrent();
};


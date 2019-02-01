#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtOrderManager;
class VtAccount;
class VtOrderManagerSelector : public TemplateSingleton<VtOrderManagerSelector>
{
public:
	VtOrderManagerSelector();
	~VtOrderManagerSelector();

	VtOrderManager* ActiveOrderManager() const { return _ActiveOrderManager; }
	void ActiveOrderManager(VtOrderManager* val) { _ActiveOrderManager = val; }
	VtOrderManager* FindAddOrderManager(std::string acntNo);
	VtOrderManager* CreateOrderManager(VtAccount* acnt);
	void SetActiveOrderManager(VtAccount* acnt);
	VtOrderManager* FindOrderManager(std::string acntNo);
private:
	VtOrderManager* _ActiveOrderManager = nullptr;
public:
	std::map<std::string, VtOrderManager*> _OrderManagerMap;

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};


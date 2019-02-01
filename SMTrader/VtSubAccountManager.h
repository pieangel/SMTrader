#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include<string>

class VtAccount;
class VtSubAccountManager : public TemplateSingleton<VtSubAccountManager>
{
public:
	VtSubAccountManager();
	~VtSubAccountManager();

	std::map<std::string, VtAccount*> AccountMap;
	VtAccount* FindAccount(std::string acntNo);
	void AddAccount(VtAccount* acnt);
	void RemoveAccount(std::string acntNo);
};


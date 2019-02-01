#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
class VtClient;
class VtLoginManager : public TemplateSingleton<VtLoginManager>
{
public:
	VtLoginManager();
	~VtLoginManager();
	void CreateClient(int target);
	int Login(std::string id, std::string password, std::string cert, int serverType);
	void Logout();

private:
	VtClient* _Client = nullptr;
};


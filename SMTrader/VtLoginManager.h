#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "Xml/pugixml.hpp"
class VtClient;
class VtLoginManager : public TemplateSingleton<VtLoginManager>
{
public:
	VtLoginManager();
	~VtLoginManager();
	void CreateClient(int target);
	int Login(std::string id, std::string password, std::string cert, int serverType);
	void Logout();
	std::string ID = "test_id";
	std::string Password = "test_password";
	std::string Cert = "12345678";
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	VtClient* _Client = nullptr;
};


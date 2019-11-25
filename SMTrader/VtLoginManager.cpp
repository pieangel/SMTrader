#include "stdafx.h"
#include "VtLoginManager.h"
#include "VtKrClient.h"


VtLoginManager::VtLoginManager()
{
}


VtLoginManager::~VtLoginManager()
{
}

void VtLoginManager::CreateClient(int target)
{
	if (target == 1)
	{
		if (_Client == nullptr)
			_Client = VtKrClient::GetInstance();
	}
}

int VtLoginManager::Login(std::string id, std::string password, std::string cert, int serverType)
{
	if (_Client)
	{
		return _Client->ConnectToServer(id, password, cert, serverType);
	}
	return -1;
}

void VtLoginManager::Logout()
{
	if (_Client)
	{
		_Client->DisconnectFromServer();
	}
}

void VtLoginManager::SaveToXml(pugi::xml_node& login_info_node)
{
	pugi::xml_node login_info_child = login_info_node.append_child("login_id");
	login_info_child.append_child(pugi::node_pcdata).set_value(ID.c_str());
	login_info_child = login_info_node.append_child("login_password");
	login_info_child.append_child(pugi::node_pcdata).set_value(Password.c_str());
	login_info_child = login_info_node.append_child("login_cert");
	login_info_child.append_child(pugi::node_pcdata).set_value(Cert.c_str());
}

void VtLoginManager::LoadFromXml(pugi::xml_node& login_info_node)
{
	std::string id = login_info_node.child_value("login_id");
	std::string pwd = login_info_node.child_value("login_password");
	std::string cert = login_info_node.child_value("login_cert");
	ID = id;
	Password = pwd;
	Cert = cert;
}

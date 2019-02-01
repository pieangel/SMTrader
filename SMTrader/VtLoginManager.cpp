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

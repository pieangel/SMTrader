#include "stdafx.h"
#include "VtClient.h"


VtClient::VtClient()
{
}


VtClient::~VtClient()
{
}

bool VtClient::PutOrder(VtOrderRequest&& orderRequest)
{
	return true;
}

bool VtClient::PutOrder(VtOrderDirectRequest&& orderRequest)
{
	return true;
}

bool VtClient::ChangeOrder(VtOrderRequest&& orderRequest)
{
	return true;
}

bool VtClient::ChangeOrder(VtOrderDirectRequest&& orderRequest)
{
	return true;
}

bool VtClient::CancelOrder(VtOrderRequest&& orderRequest)
{
	return true;
}

bool VtClient::CancelOrder(VtOrderDirectRequest&& orderRequest)
{
	return true;
}

void VtClient::RequestSymbolMaster(std::string productCode)
{

}

int VtClient::ConnectToServer(std::string id, std::string pwd, std::string cert, int serverKind /*= 2*/)
{
	return 1;
}

void VtClient::DisconnectFromServer()
{

}

void VtClient::RegisterProductAutoUpdate(std::string productCode)
{

}

void VtClient::UnReisterProductAutoUpdate(std::string productCode)
{

}

void VtClient::RegisterAccountAutoUpdate(std::string AccountCode)
{

}

void VtClient::UnRegisterAccountAutoUpdate(std::string AccountCode)
{

}

void VtClient::RequestAccountRemain(std::string accountNo)
{

}

void VtClient::RequestAccountDeposit(std::string accountNo)
{

}

void VtClient::RequestFilledOrder(std::string accountNo)
{

}

void VtClient::RequestFilledOrder(std::string accountNo, int Range, int OrderBy)
{

}

void VtClient::RequestSymbolQuote(std::string productCode)
{

}

void VtClient::RequestSymbolTickQuote(std::string productCode)
{

}

void VtClient::RequestSymbolHoga(std::string productCode)
{

}

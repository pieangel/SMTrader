#include "stdafx.h"
#include "VtKrClient.h"
//#include "VtKrClientCtrl.h"
#include "resource.h"
//#include "esapiexpctrl_kr.h"
#include "Util/VtTime.h"
#include <string>
#include "Global/ZmStream.h"
#include "VtOrder.h"
#include "Poco/NumberFormatter.h"
#include "Global/ZmStream.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "Global/VtDefine.h"

using Poco::NumberFormatter;


void VtKrClient::CreateCtrl()
{
	
}

VtKrClient::VtKrClient()
{
	CreateCtrl();
}


VtKrClient::~VtKrClient()
{
	
}

bool VtKrClient::PutOrder(VtOrderRequest&& orderRequest)
{
	
	return true;
}

bool VtKrClient::PutOrder(VtOrderDirectRequest&& orderRequest)
{
	

	return true;
}

bool VtKrClient::ChangeOrder(VtOrderRequest&& orderRequest)
{
	
	
	return true;
}

bool VtKrClient::ChangeOrder(VtOrderDirectRequest&& orderRequest)
{
	
	return true;
}

bool VtKrClient::CancelOrder(VtOrderRequest&& orderRequest)
{
	
	return true;
}

bool VtKrClient::CancelOrder(VtOrderDirectRequest&& orderRequest)
{
	
	return true;
}

void VtKrClient::SetHeaderInfo(std::string msgID, lpESHeader_st header)
{
	
}

void VtKrClient::SetHeaderInfo(lpESHeader_st header)
{
	
}

void VtKrClient::SetHeaderInfo(VtOrderRequest&& orderRequest, lpESHeader_st header)
{
	
}

void VtKrClient::SetHeaderInfo(VtOrderDirectRequest&& orderRequest, lpESHeader_st header)
{
	
}

void VtKrClient::GetChartData(VtChartDataRequest&& chartReqeust)
{
	
}

int VtKrClient::ConnectToServer(std::string id, std::string pwd, std::string cert, int serverKind /*= 2*/)
{
	int result = 0;
	
	return result;
}

void VtKrClient::DisconnectFromServer()
{
	
}

void VtKrClient::RegisterProductAutoUpdate(std::string productCode)
{
	
}

void VtKrClient::UnRegisterProductAutoUpdate(std::string productCode)
{
	
}

void VtKrClient::RegisterAccountAutoUpdate(std::string AccountCode)
{
	
}

void VtKrClient::UnRegisterAccountAutoUpdate(std::string AccountCode)
{
	
}

void VtKrClient::RequestSymbolMaster(std::string productCode)
{

}

void VtKrClient::GetSymbolMasterByShortCode(std::string productCode)
{
	
}

void VtKrClient::RequestAccountRemain(std::string accountNo)
{
	
}

void VtKrClient::RequestAccountDeposit(std::string accountNo)
{
	
}

void VtKrClient::RequestFilledOrder(std::string accountNo)
{
	
}

void VtKrClient::RequestFilledOrder(std::string accountNo, int Range, int OrderBy)
{
	

}

void VtKrClient::RequestSymbolQuote(std::string productCode)
{
	
}

void VtKrClient::RequestSymbolTickQuote(std::string productCode)
{
	
}

void VtKrClient::RequestSymbolHoga(std::string productCode)
{
	
}

void VtKrClient::padTo(std::string &str, const size_t num, const char paddingChar /*= ' '*/)
{
	if (num > str.size())
		str.insert(0, num - str.size(), paddingChar);
}

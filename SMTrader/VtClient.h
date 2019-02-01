#pragma once
#include "VtOrder.h"
class VtClient 
{
public:
	VtClient();
	virtual ~VtClient();

	virtual bool PutOrder(VtOrderDirectRequest&& orderRequest);
	virtual bool ChangeOrder(VtOrderDirectRequest&& orderRequest);
	virtual bool CancelOrder(VtOrderDirectRequest&& orderRequest);

	virtual bool PutOrder(VtOrderRequest&& orderRequest);
	virtual bool ChangeOrder(VtOrderRequest&& orderRequest);
	virtual bool CancelOrder(VtOrderRequest&& orderRequest);
	virtual void RequestSymbolMaster(std::string productCode);
	/// <summary>
	/// 서버 접속 함수
	/// </summary>
	/// <param name="id">아이디</param>
	/// <param name="pwd">비밀번호</param>
	/// <param name="cert">공인인증 비밀번호</param>
	/// <param name="serverKind">서버종류</param>
	/// <returns></returns>
	virtual int ConnectToServer(std::string id, std::string pwd, std::string cert, int serverKind = 2);
	/// <summary>
	/// 서버 접속 종료 함수
	/// </summary>
	virtual void DisconnectFromServer();

	virtual void RegisterProductAutoUpdate(std::string productCode);
	virtual void UnReisterProductAutoUpdate(std::string productCode);
	virtual void RegisterAccountAutoUpdate(std::string AccountCode);
	virtual void UnRegisterAccountAutoUpdate(std::string AccountCode);

	virtual void RequestAccountRemain(std::string accountNo);
	virtual void RequestAccountDeposit(std::string accountNo);
	virtual void RequestFilledOrder(std::string accountNo);
	virtual void RequestFilledOrder(std::string accountNo, int Range, int OrderBy);

	virtual void RequestSymbolQuote(std::string productCode);
	virtual void RequestSymbolTickQuote(std::string productCode);
	virtual void RequestSymbolHoga(std::string productCode);
};


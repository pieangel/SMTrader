#pragma once
#include "VtClient.h"
#include "Global/TemplateSingleton.h"
#include "VtChartDataRequest.h"
#include "ESApiLayout.h"
class VtKrClientCtrl;
class VtKrClient :	public VtClient, public TemplateSingleton<VtKrClient>
{
private:
	void CreateCtrl();
public:
	VtKrClient();
	~VtKrClient();

	virtual bool PutOrder(VtOrderRequest&& orderRequest);
	virtual bool ChangeOrder(VtOrderRequest&& orderRequest);
	virtual bool CancelOrder(VtOrderRequest&& orderRequest);

	virtual bool PutOrder(VtOrderDirectRequest&& orderRequest);
	virtual bool ChangeOrder(VtOrderDirectRequest&& orderRequest);
	virtual bool CancelOrder(VtOrderDirectRequest&& orderRequest);

	virtual void GetChartData(VtChartDataRequest&& chartReqeust);
	void SetHeaderInfo(std::string msgID, lpESHeader_st header);
	void SetHeaderInfo(lpESHeader_st header);
	void SetHeaderInfo(VtOrderRequest&& orderRequest, lpESHeader_st header);
	void SetHeaderInfo(VtOrderDirectRequest&& orderRequest, lpESHeader_st header);
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
	virtual void UnRegisterProductAutoUpdate(std::string productCode);
	virtual void RegisterAccountAutoUpdate(std::string AccountCode);
	virtual void UnRegisterAccountAutoUpdate(std::string AccountCode);
	virtual void RequestSymbolMaster(std::string productCode);
	void GetSymbolMasterByShortCode(std::string productCode);
	virtual void RequestAccountRemain(std::string accountNo);
	virtual void RequestAccountDeposit(std::string accountNo);
	virtual void RequestFilledOrder(std::string accountNo);
	virtual void RequestFilledOrder(std::string accountNo, int Range, int OrderBy);

	virtual void RequestSymbolQuote(std::string productCode);
	virtual void RequestSymbolTickQuote(std::string productCode);
	virtual void RequestSymbolHoga(std::string productCode);
private:
	VtKrClientCtrl* _Ctrl = nullptr;

	void padTo(std::string &str, const size_t num, const char paddingChar = ' ');
};


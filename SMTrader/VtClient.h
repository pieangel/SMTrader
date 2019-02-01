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
	/// ���� ���� �Լ�
	/// </summary>
	/// <param name="id">���̵�</param>
	/// <param name="pwd">��й�ȣ</param>
	/// <param name="cert">�������� ��й�ȣ</param>
	/// <param name="serverKind">��������</param>
	/// <returns></returns>
	virtual int ConnectToServer(std::string id, std::string pwd, std::string cert, int serverKind = 2);
	/// <summary>
	/// ���� ���� ���� �Լ�
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


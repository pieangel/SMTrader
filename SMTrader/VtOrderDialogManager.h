#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include "Global/VtDefine.h"
#include "VtQuoteEvent.h"
#include "VtOrderWndEvent.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtOrderWnd;
class CVtOrderWnd;
struct VtHoga;
struct VtQuote;
struct VtOrder;
class VtSymbolMaster;
class VtSymbol;
class VtAccount;
class VtOrderWndHd;
class VtOrderCenterWndHd;
struct VtPosition;
class CMainFrame;
class VtFund;

struct VtSymbolEventDispatcher
{
	std::map<CVtOrderWnd*, CVtOrderWnd*> WndMap;
};

struct VtAccountEventDispatcher
{
	std::map<CVtOrderWnd*, CVtOrderWnd*> WndMap;
};

struct VtSymbolDispatcher
{
	std::map<VtOrderWndHd*, VtOrderWndHd*> WndMap;
};

struct VtAccountDispatcher
{
	std::map<VtOrderWndHd*, VtOrderWndHd*> WndMap;
};

struct VtSymbolDistributer
{
	std::map<VtOrderCenterWndHd*, VtOrderCenterWndHd*> WndMap;
};

struct VtAccountDistributer
{
	std::map<VtOrderCenterWndHd*, VtOrderCenterWndHd*> WndMap;
};

struct VtOrderDistributer
{
	std::map<VtOrderCenterWndHd*, VtOrderCenterWndHd*> WndMap;
};

class VtOrderDialogManager : public TemplateSingleton<VtOrderDialogManager>
{
public:
	VtOrderDialogManager();
	~VtOrderDialogManager();
	void OnRealtimeQuoteReceived(VtRealtimeQuoteEventArgs& arg);

	void OnOrderWindowEventReceived(VtOrderWndEventArgs& arg);


	void OnOrderWndEventReceived(VtOrderWndHdEventArgs& arg);
	void AddOrderWindow(VtOrderWnd* wnd);
	void RemoveOrderWindow(VtOrderWnd* wnd);

	void AddSymbolReceiverWindow(std::string fullCode, CVtOrderWnd* wnd);
	void RemoveSymbolReceiveWindow(std::string fullCode, CVtOrderWnd* wnd);
	void ChangeSymbolReceiverWindow(std::string oldCode, std::string newCode, CVtOrderWnd* wnd);
	void AddAccountReceiverWindow(std::string acntNo, CVtOrderWnd* wnd);
	void ChangeAccountReceiverWindow(std::string oldAcntNo, std::string newAcntNo, CVtOrderWnd* wnd);
	void RemoveAccountReceiveWindow(std::string acntNo, CVtOrderWnd* wnd);

	void OnReceiveRealtimeHoga(VtHoga* hoga);
	void OnReceiveHoga(VtHoga* hoga);
	void OnOrderReceived(VtOrder* order);
	void OnAccepted(VtOrder* order);
	void OnFilled(VtOrder* order);
	void OnConfirmModify(VtOrder* order);
	void OnConfirmCancel(VtOrder* order);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void OnReceiveQuote(VtQuote* quote);
	void OnReceiveSymbolMaster(VtSymbolMaster* master);
	void OnReceiveRealtimeSymbolMaster(VtSymbolMaster* master);
	void OnReceiveAccountDeposit(VtAccount* acnt);
	void CloseAllWindow();
	void OnSymbolMaster(VtSymbol* sym);

	std::map<std::string, VtAccountEventDispatcher*> AccountDispatcherMap;
	std::map<std::string, VtSymbolEventDispatcher*> SymbolDispatcherMap;

	std::map<std::string, VtSymbolDispatcher*> SymbolWndMap;
	std::map<std::string, VtSymbolDistributer*> SymbolMap;
	std::map<std::string, VtAccountDistributer*> AccountMap;
	std::map<std::string, VtOrderDistributer*> OrderMap;
	void AddOrderWnd(VtOrderWndHd* wnd);
	void RemoveOrderWnd(VtOrderWndHd* wnd);
	void AddSymbolWnd(std::string symCode, VtOrderCenterWndHd* wnd);
	void RemoveSymbolWnd(std::string symCode,  VtOrderCenterWndHd* wnd);
	void AddAccountWnd(std::string accountNo, VtOrderCenterWndHd* wnd);
	void RemoveAccountWnd(std::string accountNo, VtOrderCenterWndHd* wnd);

	void AddOrderWnd(std::string key, VtOrderCenterWndHd* wnd);
	void RemoveOrderWnd(std::string key, VtOrderCenterWndHd* wnd);

	void AddSymbolWindow(std::string symCode, VtOrderWndHd* wnd);
	void RemoveSymbolWindow(std::string symCode, VtOrderWndHd* wnd);
	void Begin();
	void End();
	void ReleaseSymbolWnd();
	void ReleaseAccountWnd();
	void ReleaseOrderWnd();
	void ReleaseMainOrderWnd();

	void OnExpected(VtSymbol* sym);
	void OnReceiveHoga(VtSymbol* sym);
	void OnReceiveQuote(VtSymbol* sym);
	void OnReceiveMsg(CString msg);
	void OnReceiveMsgWithReqId(int id, CString msg);

	void OnOrderReceivedHd(VtOrder* order);
	void OnOrderAcceptedHd(VtOrder* order);
	void OnOrderUnfilledHd(VtOrder* order);
	void OnOrderFilledHd(VtOrder* order);
	void OnRemain(VtPosition* posi);
	void OnReceiveAccountInfo();

	void OnOutstanding();

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

	VtOrderWndHd* CreateOrderDialog();
	CMainFrame* MainFrm() const { return _MainFrm; }
	void MainFrm(CMainFrame* val) { _MainFrm = val; }

	
	typedef void (VtOrderDialogManager::*HandleOrder)(VtOrder* order);

	void OnFundAdded();
	void OnFundDeleted(VtFund* fund);
	void SendOrderMessage(VtOrderEvent event, VtOrder* order);
	bool FindFund(VtFund* oldFund);
	std::map<VtOrderWndHd*, VtOrderWndHd*>& GetWndMap() {
		return _OrderWndMap;
	}

	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);

private:
	std::map<VtOrderWndHd*, VtOrderWndHd*> _OrderWndMap;
	CMainFrame* _MainFrm;
	std::map<std::string, HandleOrder> _HandleOrderMap;
	void CreateOrderDialog(VtOrderWndHd* orderWnd);
	std::map<VtOrderWnd*, VtOrderWnd*> _OrderWindowMap;
};


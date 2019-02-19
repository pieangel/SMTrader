#pragma once
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include "Global/TemplateSingleton.h"
#include "Global/VtDefine.h"
#include "VtIndexDefine.h"

class VtSymbolSelector;
class VtSymbol;
class cWaitDialog;
class VtAccount;
class VtGlobal : public TemplateSingleton<VtGlobal>
{
public:
	static int CenterWndYPos;
	VtGlobal();
	~VtGlobal();

	void OpenWaitDialog(CString msg);
	void OpenWaitDialog();
	void CloseWaitDialog();
	void OpenWaitDialog(int state);
	void CloseWaitDialog(int state);


	VtSymbolSelector* GetSymSelector(CWnd* wnd);
	void HideSymSelector();
	std::vector<VtSymbol*> FavoriteSymbolList;
	void InitFavorite();
	void AddSymbol(VtSymbol* sym);
	void RemoveSymbol(VtSymbol* sym);
	void ClearSymbol();
	std::map<std::string, std::string> FavoriteDefaultSymbols;

	std::string vcName;
	std::string vcTime;
	int vcNameIndex;
	//std::string DefaultAccountNo;
	VtServerType ServerType() const { return _ServerType; }
	void ServerType(VtServerType val) { 
		_ServerType = val; 
	}

	void InitStressList();
	void InitBandList();
	void InitSystemList();
	void InitIndexList();
	COLORREF ColorTable[45];
	
	std::string SelProduct() const { return _SelProduct; }
	void SelProduct(std::string val) { _SelProduct = val; }
	VtChartType SelType() const { return _SelType; }
	void SelType(VtChartType val) { _SelType = val; }
	int SelCycle() const { return _SelCycle; }
	void SelCycle(int val) { _SelCycle = val; }
	std::string SelExchange() const { return _SelExchange; }
	void SelExchange(std::string val) { _SelExchange = val; }
	VtExchangeName SeLExcIndex() const { return _SeLExcIndex; }
	void SeLExcIndex(VtExchangeName val) { _SeLExcIndex = val; }
	std::vector<std::string> ExchNameVector;
	std::vector<std::string> ExchAliasVector;

	static int DefaultChartDataCount;
	bool Waiting() const { return _Waiting; }
	void Waiting(bool val) { _Waiting = val; }
	std::vector<VtIndexBase*> IndexList;
	std::vector<VtSystemTemplate> SystemList;
	std::vector<VtBandBase*> BandList;
	std::vector<VtStressBase*> StressList;
	static COLORREF GridTitleBackColor;
	static COLORREF GridTitleTextColor;
	static COLORREF SelDialogBackColor;
	static COLORREF SelCellBackColor;
	static COLORREF MainBackColor;
	static COLORREF MainTextColor;
// 	static COLORREF CallBackColor;
// 	static COLORREF CallRemainGrayColor;
// 	static COLORREF CallRemainEmptyColor;
// 	static COLORREF CallRemainGrayColor;
	static std::pair<int, int> GetScaleFact();
	static int GetHorWidthByScaleFactor(int width);
	static int GetVerHeightByScaleFactor(int height);
	static VtTime GetTime(int time);
	static VtDate GetDate(int date);
	static VtTime GetLocalTime();
	static VtDate GetLocalDate();
	static int GetDate(VtDate date);
	static int GetTime(VtTime time);
	std::map<std::string, VtAccount*> AcntList;
	/// <summary>
	/// 영업 시작시간
	/// </summary>
	static VtTime OpenTime;
	/// <summary>
	/// 영업 종료 시간
	/// </summary>
	static VtTime CloseTime;
private:

	VtExchangeName _SeLExcIndex;
	std::string _SelExchange;
	std::string _SelProduct;
	VtChartType _SelType;
	int         _SelCycle;

	VtSymbolSelector* _SymSelector = nullptr;
	VtSymbol* FindSymbol(VtSymbol* sym);

	cWaitDialog* _WaitDlg = nullptr;
	bool _Waiting = false;
	VtServerType _ServerType;

};


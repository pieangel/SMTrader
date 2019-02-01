#pragma once
#include "Skel/VtGrid.h"
class VtSymbolMaster;
class VtSymbol;
struct VtRealtimeSymbolMaster;
class VtOrderConfigManager;
class VtSymbolMasterGrid : public VtGrid
{
public:
	VtSymbolMasterGrid();
	~VtSymbolMasterGrid();

	virtual void OnSetup();
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 11;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);
	void SetSymbolMaster(VtSymbolMaster* symMaster);
	void InitSymMaster(VtSymbol* sym);
private:
	VtSymbol* _Symbol = nullptr;
	VtSymbolMaster* _SymbolMaster = nullptr;
public:
	void OnReceivedSymbolMaster(VtSymbolMaster* symMaster);
	void OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster);
	void OnReceiveRealtimeSymbolMaster(VtSymbolMaster* symMaster);
	void ResetSymbol();

	void SetOrderConfigMgr(VtOrderConfigManager* val);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};


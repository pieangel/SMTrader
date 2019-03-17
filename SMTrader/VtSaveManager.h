#pragma once
#include "Global/TemplateSingleton.h"
#include <vector>
#include <string>
#include <fstream>
class CMainFrame;
class VtSymbol;
class VtSaveManager : public TemplateSingleton<VtSaveManager>
{
public:
	VtSaveManager();
	~VtSaveManager();
	void SaveAccountList();
	void SaveFundList();
	void SaveOrders();
	void SaveOrders(std::string fileName);
	void SaveFundList(std::string fileName);
	void LoadFundList(std::string fileName);
	void SaveAccountList(std::string fileName);
	void LoadAccountList(std::string fileName);
	void SaveOrderWndList(std::string fileName, CMainFrame* mainFrm);
	void LoadOrderWndList(std::string fileName, CMainFrame* mainFrm);
	void SaveDialogList(std::string fileName, CMainFrame* mainFrm);
	void LoadDialogList(std::string fileName, CMainFrame* mainFrm);
	void SaveTotal(std::string fileName, CMainFrame* mainFrm);
	void SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save);
	void LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save);
	void SaveSystems(std::string fileName);
	void LoadSystems(std::string fileName);
	void SaveOutSystems(std::string fileName);
	void LoadOutSystems(std::string fileName);
	void SaveOutSignal(std::string fileName);
	void LoadOutSignal(std::string fileName);
	void CreateUserDirectory();
	std::vector<VtSymbol*>& GetSymbolVector()
	{
		return _SymbolVector;
	}
	bool fexists(const std::string& filename) {
		std::ifstream ifile(filename.c_str());
		return (bool)ifile;
	}
	bool IsAccountFileExist();
private:
	std::vector<VtSymbol*> _SymbolVector;
	void GetSymbolMasters();
};


#pragma once
#include "Global/TemplateSingleton.h"
#include "HdWindowEvent.h"
#include <map>
#include <string>
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtUsdStrategyConfigDlg;
typedef std::map<VtUsdStrategyConfigDlg*, std::pair<HdWindowType, VtUsdStrategyConfigDlg*>> StrategyWindowMap;
class CMainFrame;
class VtSystem;
class VtStrategyWndManager : public TemplateSingleton<VtStrategyWndManager>
{
public:
	VtStrategyWndManager();
	~VtStrategyWndManager();
	StrategyWindowMap& GetWindowMap()
	{
		return _WindowMap;
	}
	void DestroyAllWindow();
	void OnWindowEvent(HdWindowEventArgs& arg);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	CMainFrame* MainFrm() const { return _MainFrm; }
	void MainFrm(CMainFrame* val) { _MainFrm = val; }
	void AddSystemDialog(std::string sysName, VtUsdStrategyConfigDlg* dlg);
	void RemoveSystemDialog(std::string sysName, VtUsdStrategyConfigDlg* dlg);
	void UpdateDialog(VtSystem* sys);
	int WndCnt() const { return _WndCnt; }
	void WndCnt(int val) { _WndCnt = val; }
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	std::map<std::string, std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*>> _SystemToSetDialogMap;
	StrategyWindowMap _WindowMap;
	void AddWindow(HdWindowType wndType, VtUsdStrategyConfigDlg* wnd);
	void RemoveWindow(VtUsdStrategyConfigDlg* wnd);
	void RestoreDialog(HdWindowType type, CRect rcWnd);
	void RestoreDialog(std::string sysName, CRect rcWnd);
	CMainFrame* _MainFrm = nullptr;
	int _WndCnt = 0;
};



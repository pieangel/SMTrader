#include "stdafx.h"
#include "VtStrategyWndManager.h"

#include "VtAccountAssetDlg.h"
#include "HdAccountPLDlg.h"
#include "VtFundDesigner.h"
#include "VtFundMiniJango.h"
#include "VtChartWindow.h"
#include "resource.h"
#include "VtStrategyToolWnd.h"
#include "VtUsdStrategyConfigDlg.h"
#include "System/VtSystem.h"
#include "System/VtSystemManager.h"

VtStrategyWndManager::VtStrategyWndManager()
{
}


VtStrategyWndManager::~VtStrategyWndManager()
{
	DestroyAllWindow();
}

void VtStrategyWndManager::DestroyAllWindow()
{
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
		auto item = it->second;
		CWnd* wnd = item.second;
		if (wnd->GetSafeHwnd()) {
			wnd->DestroyWindow();
		}
		delete wnd;
	}

	_WindowMap.clear();
}

void VtStrategyWndManager::OnWindowEvent(HdWindowEventArgs& arg)
{
	if (!arg.pWnd)
		return;

	if (arg.eventType == HdWindowEventType::Created)
		AddWindow(arg.wndType, (VtUsdStrategyConfigDlg*)arg.pWnd);
	else if (arg.eventType == HdWindowEventType::Closed) {
		RemoveWindow((VtUsdStrategyConfigDlg*)arg.pWnd);
		arg.pWnd->DestroyWindow();
		delete arg.pWnd;
	}
}

void VtStrategyWndManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	// 대화상자 갯수 저장
	ss << (int)_WindowMap.size();
	// 개별 윈도우 타입과 위치 저장
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
		auto item = it->second;
		
		int dlgType = (int)std::get<0>(item);
		CRect rcWnd;
		std::get<1>(item)->GetWindowRect(rcWnd);
		ss << dlgType;
		ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
		if (std::get<1>(item)->System())
			ss << std::get<1>(item)->System()->Name();
		else
			ss << _T("NoSystem");
	}
}

void VtStrategyWndManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count;
	ss >> count;
	_WndCnt = count;
	for (int i = 0; i < count; ++i) {
		int type = 0;
		ss >> type;
		HdWindowType wndType = (HdWindowType)type;
		CRect rcWnd;
		ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;
		std::string systemName;
		ss >> systemName;
		RestoreDialog(systemName, rcWnd);
	}
}

void VtStrategyWndManager::AddSystemDialog(std::string sysName, VtUsdStrategyConfigDlg* dlg)
{
	auto it = _SystemToSetDialogMap.find(sysName);
	if (it != _SystemToSetDialogMap.end()) {
		std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*>& dlgMap = it->second;
		dlgMap[dlg] = dlg;
	} 
	else {
		std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*> dlgMap;
		dlgMap[dlg] = dlg;
		_SystemToSetDialogMap[sysName] = dlgMap;
	}
}

void VtStrategyWndManager::RemoveSystemDialog(std::string sysName, VtUsdStrategyConfigDlg* dlg)
{
	auto it = _SystemToSetDialogMap.find(sysName);
	if (it != _SystemToSetDialogMap.end()) {
		std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*>& dlgMap = it->second;
		auto itd = dlgMap.find(dlg);
		if (itd != dlgMap.end()) {
			dlgMap.erase(itd);
		}
	}
}

void VtStrategyWndManager::UpdateDialog(VtSystem* sys)
{
	if (!sys)
		return;
	auto it = _SystemToSetDialogMap.find(sys->Name());
	if (it != _SystemToSetDialogMap.end()) {
		std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*>& dlgMap = it->second;
		for (auto itd = dlgMap.begin(); itd != dlgMap.end(); ++itd) {
			VtUsdStrategyConfigDlg* dlg = itd->second;
			dlg->UpdateRunCheck(sys);
		}
	}
}

/*
// 대화상자 갯수 저장
ss << (int)_WindowMap.size();
// 개별 윈도우 타입과 위치 저장
for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
auto item = it->second;

int dlgType = (int)std::get<0>(item);
CRect rcWnd;
std::get<1>(item)->GetWindowRect(rcWnd);
ss << dlgType;
ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
if (std::get<1>(item)->System())
ss << std::get<1>(item)->System()->Name();
else
ss << _T("NoSystem");
}
*/
void VtStrategyWndManager::SaveToXml(pugi::xml_node& stratege_window_list_node)
{
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
		auto item = it->second;

		int dlgType = (int)std::get<0>(item);
		CRect rcWnd;
		std::get<1>(item)->GetWindowRect(rcWnd);
		pugi::xml_node stratege_window_node = stratege_window_list_node.append_child("stratege_window");	
		pugi::xml_node stratege_window_child_node = stratege_window_node.append_child("stratege_window_pos");
		stratege_window_child_node.append_attribute("left") = rcWnd.left;
		stratege_window_child_node.append_attribute("top") = rcWnd.top;
		stratege_window_child_node.append_attribute("right") = rcWnd.right;
		stratege_window_child_node.append_attribute("bottom") = rcWnd.bottom;
		stratege_window_child_node = stratege_window_node.append_child("dialog_type");
		stratege_window_child_node.append_child(pugi::node_pcdata).set_value(std::to_string(dlgType).c_str());
		stratege_window_child_node = stratege_window_node.append_child("system_name");
		if (std::get<1>(item)->System()) {
			stratege_window_child_node.append_child(pugi::node_pcdata).set_value(std::get<1>(item)->System()->Name().c_str());
		}
		else {
			stratege_window_child_node.append_child(pugi::node_pcdata).set_value("no_system");
		}
	}
}

void VtStrategyWndManager::LoadFromXml(pugi::xml_node& node)
{

}

void VtStrategyWndManager::AddWindow(HdWindowType wndType, VtUsdStrategyConfigDlg* wnd)
{
	if (!wnd)
		return;
	_WindowMap[wnd] = std::make_pair(wndType, wnd);
}

void VtStrategyWndManager::RemoveWindow(VtUsdStrategyConfigDlg* wnd)
{
	auto it = _WindowMap.find(wnd);
	if (it != _WindowMap.end()) {
		if (wnd->System()) {
			RemoveSystemDialog(wnd->System()->Name(), wnd);
		}
		_WindowMap.erase(it);
	}
}

void VtStrategyWndManager::RestoreDialog(HdWindowType type, CRect rcWnd)
{
	
}

void VtStrategyWndManager::RestoreDialog(std::string sysName, CRect rcWnd)
{
	VtUsdStrategyConfigDlg* dlg = new VtUsdStrategyConfigDlg((CWnd*)_MainFrm);
	dlg->Create(IDD_SYSTEM_USD, (CWnd*)_MainFrm);
	dlg->MoveWindow(rcWnd);

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	VtSystem* sys = sysMgr->GetSystem(sysName);
	if (sys) {
		dlg->ClearArgMap();
		dlg->SetSystem(sys);
		AddSystemDialog(sysName, dlg);
	}
	dlg->ShowWindow(SW_SHOW);
	_WndCnt--;
}

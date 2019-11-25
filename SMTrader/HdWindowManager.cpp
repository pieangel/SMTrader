#include "stdafx.h"
#include "HdWindowManager.h"
#include "VtAccountAssetDlg.h"
#include "HdAccountPLDlg.h"
#include "VtFundDesigner.h"
#include "VtFundMiniJango.h"
#include "VtChartWindow.h"
#include "resource.h"
#include "VtStrategyToolWnd.h"
#include "VtUsdStrategyConfigDlg.h"
#include "VtAccount.h"
#include "VtFund.h"

HdWindowManager::HdWindowManager()
{
}


HdWindowManager::~HdWindowManager()
{
	DestroyAllWindow();
}

void HdWindowManager::DestroyAllWindow()
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

void HdWindowManager::OnWindowEvent(HdWindowEventArgs& arg)
{
	if (!arg.pWnd)
		return;

	if (arg.eventType == HdWindowEventType::Created)
		AddWindow(arg.wndType, arg.pWnd);
	else if (arg.eventType == HdWindowEventType::Closed) {
		RemoveWindow(arg.pWnd);
		arg.pWnd->DestroyWindow();
		delete arg.pWnd;
	}
}

void HdWindowManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	// 대화상자 갯수 저장
	ss << (int)_WindowMap.size();
	// 개별 윈도우 타입과 위치 저장
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
		auto item = it->second;
		int dlgType = (int)std::get<0>(item);
		ss << dlgType;
		Save(ss, std::get<0>(item), std::get<1>(item));
	}
}

void HdWindowManager::Save(simple::file_ostream<same_endian_type>& ss, HdWindowType wndType, CWnd* wnd)
{

	if (!wnd)
		return;

	switch (wndType) {
	case HdWindowType::ChartWindow: {
		((VtChartWindow*) wnd)->Save(ss);
	}
	break;
	case HdWindowType::FundMiniJangoWindow: {
		((VtFundMiniJango*)wnd)->Save(ss);
	}
	break;
	case HdWindowType::MiniJangoWindow: {
		((HdAccountPLDlg*)wnd)->Save(ss);
	}
	break;
	case HdWindowType::AssetWindow: {
		((VtAccountAssetDlg*)wnd)->Save(ss);
	}
	break;
	case HdWindowType::StrategyToolWindow: {
		((VtStrategyToolWnd*)wnd)->Save(ss);
	}
	break;
	}
}

void HdWindowManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count;
	ss >> count;
	for (int i = 0; i < count; ++i) {
		int type = 0;
		ss >> type;
		HdWindowType wndType = (HdWindowType)type;
		Load(ss, wndType);
	}
}

void HdWindowManager::Load(simple::file_istream<same_endian_type>& ss, HdWindowType wndType)
{
	
	switch (wndType) {
	case HdWindowType::ChartWindow: {
		VtChartWindow* dlg = new VtChartWindow((CWnd*)_MainFrm);
		dlg->Create(IDD_DIALOG_CHART, (CWnd*)_MainFrm);
		dlg->Load(ss);
	}
	break;
	case HdWindowType::FundMiniJangoWindow: {
		VtFundMiniJango* dlg = new VtFundMiniJango((CWnd*)_MainFrm);
		dlg->Create(IDD_MINI_JANGO_FUND, (CWnd*)_MainFrm);
		dlg->Load(ss);
	}
	break;
	case HdWindowType::MiniJangoWindow: {
		HdAccountPLDlg* dlg = new HdAccountPLDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_MINI_JANGO, (CWnd*)_MainFrm);
		dlg->Load(ss);
	}
	break;
	case HdWindowType::AssetWindow: {
		VtAccountAssetDlg* dlg = new VtAccountAssetDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_ACCOUNT_ASSET, (CWnd*)_MainFrm);
		dlg->Load(ss);
	}
	break;
	case HdWindowType::StrategyToolWindow: {
		VtStrategyToolWnd* dlg = new VtStrategyToolWnd((CWnd*)_MainFrm);
		dlg->Create(IDD_STRATEGE_TOOLS, (CWnd*)_MainFrm);
		dlg->Load(ss);
	}
	break;
	}
}

void HdWindowManager::SaveToXml(pugi::xml_node& window_list_node)
{
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it) {
		auto item = it->second;
		std::string window_type = "";
		switch (std::get<0>(item)) {
		case HdWindowType::ChartWindow:
			window_type = "chart_window";
			break;
		case HdWindowType::FundMiniJangoWindow:
			window_type = "fund_mini_jango_window";
			break;
		case HdWindowType::MiniJangoWindow:
			window_type = "account_mini_jango_window";
			break;
		case HdWindowType::AssetWindow:
			window_type = "asset_window";
			break;
		case HdWindowType::StrategyToolWindow:
			window_type = "stratege_tool_window";
			break;
		}
		pugi::xml_node window_node = window_list_node.append_child("window");
		window_node.append_attribute("window_type") = (int)std::get<0>(item);
		window_node.append_attribute("window_type_name") = window_type.c_str();
		SaveToXml(window_node, std::get<0>(item), std::get<1>(item));
	}
}

void HdWindowManager::SaveToXml(pugi::xml_node& window_node, HdWindowType wndType, CWnd* wnd)
{
	if (!wnd)
		return;

	switch (wndType) {
		case HdWindowType::ChartWindow: {
			((VtChartWindow*)wnd)->SaveToXml(window_node);
		}
		break;
		case HdWindowType::FundMiniJangoWindow: {
			((VtFundMiniJango*)wnd)->SaveToXml(window_node);
		}
		break;
		case HdWindowType::MiniJangoWindow: {
			((HdAccountPLDlg*)wnd)->SaveToXml(window_node);
		}
		break;
		case HdWindowType::AssetWindow: {
			((VtAccountAssetDlg*)wnd)->SaveToXml(window_node);
		}
		break;
		case HdWindowType::StrategyToolWindow: {
			((VtStrategyToolWnd*)wnd)->SaveToXml(window_node);
		}
		break;
	}
}

void HdWindowManager::LoadFromXml(pugi::xml_node& etc_window_list_node)
{
	for (pugi::xml_node window_node = etc_window_list_node.child("window"); window_node; window_node = window_node.next_sibling("window")) {
		HdWindowType window_type = (HdWindowType)window_node.attribute("window_type").as_int();
		LoadFromXml(window_node, window_type);
	}
}

void HdWindowManager::LoadFromXml(pugi::xml_node& window_node, HdWindowType wndType)
{
	
	switch (wndType) {
	case HdWindowType::ChartWindow: {
		VtChartWindow* dlg = new VtChartWindow((CWnd*)_MainFrm);
		dlg->Create(IDD_DIALOG_CHART, (CWnd*)_MainFrm);
		dlg->LoadFromXml(window_node);
	}
									break;
	case HdWindowType::FundMiniJangoWindow: {
		VtFundMiniJango* dlg = new VtFundMiniJango((CWnd*)_MainFrm);
		dlg->Create(IDD_MINI_JANGO_FUND, (CWnd*)_MainFrm);
		dlg->LoadFromXml(window_node);
	}
											break;
	case HdWindowType::MiniJangoWindow: {
		HdAccountPLDlg* dlg = new HdAccountPLDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_MINI_JANGO, (CWnd*)_MainFrm);
		dlg->LoadFromXml(window_node);
	}
										break;
	case HdWindowType::AssetWindow: {
		VtAccountAssetDlg* dlg = new VtAccountAssetDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_ACCOUNT_ASSET, (CWnd*)_MainFrm);
		dlg->LoadFromXml(window_node);
	}
									break;
	case HdWindowType::StrategyToolWindow: {
		VtStrategyToolWnd* dlg = new VtStrategyToolWnd((CWnd*)_MainFrm);
		dlg->Create(IDD_STRATEGE_TOOLS, (CWnd*)_MainFrm);
		dlg->LoadFromXml(window_node);
	}
										   break;
	}
}

void HdWindowManager::AddWindow(HdWindowType wndType, CWnd* wnd)
{
	if (!wnd)
		return;
	_WindowMap[wnd] = std::make_pair(wndType, wnd);
}

void HdWindowManager::RemoveWindow(CWnd* wnd)
{
	auto it = _WindowMap.find(wnd);
	if (it != _WindowMap.end()) {
		_WindowMap.erase(it);
	}
}

void HdWindowManager::RestoreDialog(HdWindowType type, CRect rcWnd, std::string info)
{
	if (!_MainFrm)
		return;
	switch (type) {
	case HdWindowType::ChartWindow: {
		VtChartWindow* dlg = new VtChartWindow((CWnd*)_MainFrm);
		dlg->Create(IDD_DIALOG_CHART, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::FundDesigner: {
		VtFundDesigner* dlg = new VtFundDesigner((CWnd*)_MainFrm);
		dlg->Create(IDD_FUND_DESIGNER, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::FundMiniJangoWindow: {
		VtFundMiniJango* dlg = new VtFundMiniJango((CWnd*)_MainFrm);
		dlg->_DefaultFund = info;
		dlg->Create(IDD_MINI_JANGO_FUND, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::MiniJangoWindow: {
		HdAccountPLDlg* dlg = new HdAccountPLDlg((CWnd*)_MainFrm);
		dlg->_DefaultAccount = info;
		dlg->Create(IDD_MINI_JANGO, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::AssetWindow: {
		VtAccountAssetDlg* dlg = new VtAccountAssetDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_ACCOUNT_ASSET, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::StrategyToolWindow: {
		VtStrategyToolWnd* dlg = new VtStrategyToolWnd((CWnd*)_MainFrm);
		dlg->Create(IDD_STRATEGE_TOOLS, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	case HdWindowType::StrategySetWindow: {
		VtUsdStrategyConfigDlg* dlg = new VtUsdStrategyConfigDlg((CWnd*)_MainFrm);
		dlg->Create(IDD_SYSTEM_USD, (CWnd*)_MainFrm);
		dlg->MoveWindow(rcWnd);
		dlg->ShowWindow(SW_SHOW);
	}
		break;
	}
}

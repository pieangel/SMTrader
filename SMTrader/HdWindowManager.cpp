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
		CRect rcWnd;
		std::get<1>(item)->GetWindowRect(rcWnd);
		ss << dlgType;
		ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
// 		if ((HdWindowType)dlgType == HdWindowType::MiniJangoWindow) {
// 			VtAccount* acnt = ((HdAccountPLDlg*)std::get<1>(item))->Account();
// 			if (acnt)
// 				ss << acnt->AccountNo;
// 		}
// 		else if ((HdWindowType)dlgType == HdWindowType::FundMiniJangoWindow) {
// 			VtFund* fund = ((VtFundMiniJango*)std::get<1>(item))->Fund();
// 			if (fund)
// 				ss << fund->Name;
// 		}
// 		else {
// 			std::string info = _T("no");
// 		}
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
		CRect rcWnd;
		ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;
		std::string info;
		/*ss >> info;*/
		RestoreDialog(wndType, rcWnd, info);
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

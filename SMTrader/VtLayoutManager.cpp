#include "stdafx.h"
#include "VtLayoutManager.h"


VtLayoutManager::VtLayoutManager(CWnd* parentWnd)
{
	_ParentWnd = parentWnd;
}


VtLayoutManager::~VtLayoutManager()
{
}

void VtLayoutManager::AddWindow(int resourceID)
{
	if (!_ParentWnd)
		return;

	_WndIDVec.push_back(resourceID);

	/*
	CRect rcWnd;
	CWnd* wnd = (CWnd*)GetDlgItem(_ParentWnd->GetSafeHwnd(), resourceID);
	if (wnd) {
		wnd->GetWindowRect(rcWnd);
		_ParentWnd->ScreenToClient(rcWnd);
	}
	*/
}

void VtLayoutManager::AddWindow(std::string name, int resourceID, CRect rc)
{
	_WndMap[resourceID] = std::make_pair(name, rc);
}

BOOL VtLayoutManager::RedrawAll()
{
	if (!_ParentWnd)
		return FALSE;

	HDWP hdwp = ::BeginDeferWindowPos(_WndMap.size());
	if (hdwp) {
		for (auto it = _WndMap.begin(); it != _WndMap.end(); ++it) {
			std::pair<std::string, CRect> item = it->second;
			CWnd* pWnd = (CWnd*)::GetDlgItem(_ParentWnd->GetSafeHwnd(), it->first);
			ASSERT(pWnd);
			if (pWnd->GetSafeHwnd()) {
				CRect rectWC = std::get<1>(item);
				::DeferWindowPos(hdwp, pWnd->m_hWnd, NULL,
					rectWC.left, rectWC.top, rectWC.Width(), rectWC.Height(),
					SWP_NOZORDER);
				pWnd->RedrawWindow();

			}
		}
		return ::EndDeferWindowPos(hdwp);
	}

	return FALSE;
}

int VtLayoutManager::GetWidth(int resourceID)
{
	auto it = _WndMap.find(resourceID);
	if (it != _WndMap.end()) {
		return std::get<1>(it->second).Width();
	} else {
		return -1;
	}
}

int VtLayoutManager::GetHeight(int resourceID)
{
	auto it = _WndMap.find(resourceID);
	if (it != _WndMap.end()) {
		return std::get<1>(it->second).Height();
	}
	else {
		return -1;
	}
}

CRect& VtLayoutManager::GetRect(int resourceID)
{
	std::pair<std::string, CRect>& item = _WndMap[resourceID];
	return std::get<1>(item);
}

void VtLayoutManager::SetRect(int resourceID, CRect rcCtrl)
{
	if (!FindControl(resourceID))
		return;

	std::pair<std::string, CRect>& item = _WndMap[resourceID];
	CRect& rcWnd = std::get<1>(item);
	rcWnd.left = rcCtrl.left;
	rcWnd.top = rcCtrl.top;
	rcWnd.right = rcCtrl.right;
	rcWnd.bottom = rcCtrl.bottom;
}

bool VtLayoutManager::FindControl(int resourceID)
{
	auto it = _WndMap.find(resourceID);
	if (it != _WndMap.end()) {
		return true;
	} else {
		return false;
	}
}

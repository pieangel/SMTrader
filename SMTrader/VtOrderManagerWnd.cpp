// VtOrderMangerWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderManagerWnd.h"
#include "VtHdClient.h"

// VtOrderMangerWnd

IMPLEMENT_DYNAMIC(VtOrderManagerWnd, CStatic)

VtOrderManagerWnd::VtOrderManagerWnd()
{

}

VtOrderManagerWnd::~VtOrderManagerWnd()
{
}


LRESULT VtOrderManagerWnd::OnOrderAccepted(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (!_OrderQ.empty()) {
		_OrderQ.pop();
		if (!_OrderQ.empty()) {
			HdOrderRequest req = _OrderQ.front();
			VtHdClient* client = VtHdClient::GetInstance();
			client->ProcessOrder(std::move(req));
		}
	}

	return 0;
}

LRESULT VtOrderManagerWnd::OnOrderAdded(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	if (_OrderQ.empty())
		return 0;

	HdOrderRequest req = _OrderQ.front();
	VtHdClient* client = VtHdClient::GetInstance();
	client->ProcessOrder(std::move(req));
	return 0;
}

void VtOrderManagerWnd::AddOrderRequest(HdOrderRequest&& req)
{
	_OrderQ.push(req);
}

BEGIN_MESSAGE_MAP(VtOrderManagerWnd, CStatic)
	ON_MESSAGE(WM_ORDER_ACCEPTED, OnOrderAccepted)
END_MESSAGE_MAP()



// VtOrderMangerWnd message handlers


